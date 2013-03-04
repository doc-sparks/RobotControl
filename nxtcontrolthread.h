#ifndef NXTCONTROLTHREAD_H
#define NXTCONTROLTHREAD_H

#include <QThread>
#include <python2.7/Python.h>

#define NUM_OUTPUT_PORTS 3

enum PortEnum {
    portA = 0,
    portB = 1,
    portC = 2
};

class NXTControlThread : public QThread
{
    Q_OBJECT
public:
    explicit NXTControlThread(QObject *parent = 0);
    ~NXTControlThread();

    void run();
    void cleanPythonRefs();
    bool init();

    QString getErrorText() { return errorText_; }

protected:
    void updateTachosAndState();
    void setPortPower(PortEnum port, int power, bool running);

public:
    // control methods
    void setMotorTachoLimit(PortEnum port, int tacho_limit, int power);

private:
    PyObject *pymodLocator_;
    PyObject *pymodMotor_;
    PyObject *pyNXTBrick_;
    PyObject *currStateVal_[NUM_OUTPUT_PORTS];

    int currTachoVal_[NUM_OUTPUT_PORTS];
    int currPowerVal_[NUM_OUTPUT_PORTS];

    // target values
    int targetTacho_[NUM_OUTPUT_PORTS];
    int targetPower_[NUM_OUTPUT_PORTS];

    QString errorText_;
    bool keepRunning_;

signals:
    
public slots:
    
};

#endif // NXTCONTROLTHREAD_H
