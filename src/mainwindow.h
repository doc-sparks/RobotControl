#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class NXTControlThread;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(NXTControlThread *th, QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;

    NXTControlThread *nxtThread_;

public:
    void moveRobot(int dist);
    void rotateRobot(int ang);

public slots:
    void moveForward();
    void moveBackward();
    void rotateLeft();
    void rotateRight();
};

#endif // MAINWINDOW_H
