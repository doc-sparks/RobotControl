#include <QtGui/QApplication>
#include "mainwindow.h"
#include "nxtcontrolthread.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{    
    QApplication a(argc, argv);

    // create a new thread for the NXT Control
    NXTControlThread th;

    // initialise it
    if (!th.init()) {
        QMessageBox msgBox;
        msgBox.setText(th.getErrorText());
        msgBox.exec();
        return 1;
    }

    // now set the thread running
    th.start();

    // create the main window
    MainWindow w(&th);
    w.show();
    
    return a.exec();
}
