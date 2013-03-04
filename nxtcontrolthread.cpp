#include "nxtcontrolthread.h"

NXTControlThread::NXTControlThread(QObject *parent) :
    pymodLocator_(NULL), pymodMotor_(NULL), pyNXTBrick_(NULL), errorText_(""),
    keepRunning_(true),
    QThread(parent)
{
}

NXTControlThread::~NXTControlThread()
{
    cleanPythonRefs();
}

// make sure all python refs have been removed
void NXTControlThread::cleanPythonRefs()
{
    Py_XDECREF(pymodLocator_);
    Py_XDECREF(pymodMotor_);
    Py_XDECREF(pyNXTBrick_);
}

// initialise everything for python nxt
bool NXTControlThread::init()
{
    // start by initialising python
    Py_Initialize();

    // now import the pythonnxt modules
    pymodLocator_ = PyImport_ImportModule("nxt.locator");
    if (!pymodLocator_) {
        errorText_ = "Could not import nxt.locator module";
        return false;
    }

    pymodMotor_ = PyImport_ImportModule("nxt.motor");
    if (!pymodLocator_) {
        errorText_ = "Could not import nxt.motor module";
        return false;
    }

    // find the brick
    PyObject *fn = PyObject_GetAttrString(pymodLocator_, "find_one_brick");
    if (!fn) {
        errorText_ = "Could not find find_one_brick method";
        return false;
    }
    pyNXTBrick_ = PyObject_CallObject(fn, NULL);
    if (!pyNXTBrick_) {
        errorText_ = "Could not find the NXT brick";
        return false;
    }
    Py_XDECREF(fn);

    // blank the staet objects
    for (int i = 0; i < NUM_OUTPUT_PORTS; i++) {
        currStateVal_[i] = NULL;
    }

    updateTachosAndState();

    for (int i = 0; i < NUM_OUTPUT_PORTS; i++) {
        targetTacho_[i] = currTachoVal_[i];
        targetPower_[i] = currPowerVal_[i];
    }

    return true;
}

// update the current tacho values
void NXTControlThread::updateTachosAndState() {

    // loop over all motors
    for (int i = 0; i < 3; i++) {

        // update tachos for each motor - cribbed from motor.py in pythonnxt
        PyObject *fn = PyObject_GetAttrString(pyNXTBrick_, "get_output_state");
        PyObject *args = Py_BuildValue("(i)", i);
        PyObject *motor_values = PyObject_CallObject(fn, args);
        if (!motor_values) {
            PyErr_Print();
            continue;
        }
        Py_XDECREF(fn);
        Py_XDECREF(args);

        fn = PyObject_GetAttrString(pymodMotor_, "get_tacho_and_state");
        // should just be able to pass motor_values, but need to create a tuple out of it for some reason
        args = Py_BuildValue("(O)", motor_values);
        PyObject *state_tuple = PyObject_CallObject(fn, args);
        if (!state_tuple) {
            PyErr_Print();
            continue;
        }
        PyObject* curr_tacho_obj = PyTuple_GetItem(state_tuple, 1);
        PyObject *state_obj = PyTuple_GetItem(state_tuple, 0);
        PyObject* py_tacho_val = PyObject_GetAttrString(curr_tacho_obj, "tacho_count");
        currTachoVal_[i] = ((PyIntObject *)py_tacho_val)->ob_ival;
        PyObject* py_power_val = PyObject_GetAttrString(state_obj, "power");
        currPowerVal_[i] = ((PyIntObject *)py_power_val)->ob_ival;

        Py_XDECREF(fn);
        Py_XDECREF(args);
        Py_XDECREF(motor_values);
        Py_XDECREF(state_tuple);
        Py_XDECREF(curr_tacho_obj);
        Py_XDECREF(py_tacho_val);
        Py_XDECREF(py_power_val);
        Py_XDECREF(state_obj);
    }
}

// set the tacho limit for a given motor and approach with a given power
void NXTControlThread::setMotorTachoLimit(PortEnum port, int tacho_limit, int power)
{
    targetTacho_[port] = currTachoVal_[port] + ( power < 0 ? - tacho_limit : tacho_limit );
    targetPower_[port] = power;
}

// set the tacho limit for a given motor and approach with a given powerpower on the motor
void NXTControlThread::setPortPower(PortEnum port, int power, bool running)
{

    // update tachos for each motor - cribbed from motor.py in pythonnxt
    PyObject *fn = PyObject_GetAttrString(pyNXTBrick_, "get_output_state");
    PyObject *args = Py_BuildValue("(i)", port);
    PyObject *motor_values = PyObject_CallObject(fn, args);
    if (!motor_values) {
        PyErr_Print();
    }
    Py_XDECREF(fn);
    Py_XDECREF(args);

    fn = PyObject_GetAttrString(pymodMotor_, "get_tacho_and_state");
    // should just be able to pass motor_values, but need to create a tuple out of it for some reason
    args = Py_BuildValue("(O)", motor_values);
    PyObject *state_tuple = PyObject_CallObject(fn, args);
    if (!state_tuple) {
        PyErr_Print();
    }
    PyObject *state_obj = PyTuple_GetItem(state_tuple, 0);

    // set the power value
    PyObject *val = PyInt_FromLong(power);
    PyObject_SetAttrString(state_obj, "power", val);
    Py_XDECREF(val);
    if (running) {
        val = PyInt_FromLong(32);
    } else {
        val = PyInt_FromLong(0);
    }
    PyObject_SetAttrString(state_obj, "run_state", val);
    Py_XDECREF(val);
    val = PyInt_FromLong(0);
    PyObject_SetAttrString(state_obj, "tacho_limit", val);
    Py_XDECREF(val);
    if (running) {
        val = PyInt_FromLong(5);
    } else {
        val = PyInt_FromLong(0);
    }
    PyObject_SetAttrString(state_obj, "mode", val);
    Py_XDECREF(val);
    if (running) {
        val = PyInt_FromLong(1);
    } else {
        val = PyInt_FromLong(0);
    }
    PyObject_SetAttrString(state_obj, "regulation", val);
    Py_XDECREF(val);

    // create tuple
    fn = PyObject_GetAttrString(state_obj, "to_list");
    PyObject *state_list = PyObject_CallObject(fn, NULL);
    Py_XDECREF(fn);
    args = Py_BuildValue("(i, i, i, i, i, i, i)", (int)port,
                         ((PyIntObject *)PyList_GetItem(state_list, 0))->ob_ival,
                         ((PyIntObject *)PyList_GetItem(state_list, 1))->ob_ival,
                         ((PyIntObject *)PyList_GetItem(state_list, 2))->ob_ival,
                         ((PyIntObject *)PyList_GetItem(state_list, 3))->ob_ival,
                         ((PyIntObject *)PyList_GetItem(state_list, 4))->ob_ival,
                         ((PyIntObject *)PyList_GetItem(state_list, 5))->ob_ival
                         );
    Py_XDECREF(state_list);

    // and now set motor
    fn = PyObject_GetAttrString(pyNXTBrick_, "set_output_state");
    PyObject_CallObject(fn, args);
    Py_XDECREF(fn);
    Py_XDECREF(args);
}

// override run for the main loop
void NXTControlThread::run() {

    // go through the main loop
    while (keepRunning_) {

        // update the tacho values before anything else
        updateTachosAndState();

        for (int i = 0; i < NUM_OUTPUT_PORTS; i++) {

            // do we have a different power set
            if ((targetPower_[i] != currPowerVal_[i]) && (abs(targetTacho_[i] - currTachoVal_[i]) > 10)) {
                // we do, so set the power
                setPortPower((PortEnum)i, targetPower_[i], true);
            }

            // check if we've hit the target tacho limit and stop the motor if so
            if ((abs(targetTacho_[i] - currTachoVal_[i]) < 10) && (currPowerVal_[i] != 0)) {
                targetPower_[i] = 0;
                targetTacho_[i] = currTachoVal_[i];
                setPortPower((PortEnum)i, 0, false);
            }
        }
    }

    // ending the loop so deref everything
    cleanPythonRefs();
}
