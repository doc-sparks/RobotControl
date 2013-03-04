#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nxtcontrolthread.h"

MainWindow::MainWindow(NXTControlThread *th, QWidget *parent) :
    nxtThread_(th),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // connect the buttons
    QObject::connect(ui->btnMoveForward, SIGNAL(clicked()), this, SLOT(moveForward()));
    QObject::connect(ui->btnMoveBackward, SIGNAL(clicked()), this, SLOT(moveBackward()));
    QObject::connect(ui->btnRotateLeft, SIGNAL(clicked()), this, SLOT(rotateLeft()));
    QObject::connect(ui->btnRotateRight, SIGNAL(clicked()), this, SLOT(rotateRight()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::moveRobot(int dist)
{
    // move the robot the given distance
    if (dist > 0)
    {
        nxtThread_->setMotorTachoLimit(portB, 300, 100);
        nxtThread_->setMotorTachoLimit(portC, 300, 100);
    } else {
        nxtThread_->setMotorTachoLimit(portB, 300, -100);
        nxtThread_->setMotorTachoLimit(portC, 300, -100);
    }

}

void MainWindow::rotateRobot(int ang)
{
    // move the robot the given distance
    if (ang > 0)
    {
        nxtThread_->setMotorTachoLimit(portB, 300, 100);
        nxtThread_->setMotorTachoLimit(portC, 300, -100);
    } else {
        nxtThread_->setMotorTachoLimit(portB, 300, -100);
        nxtThread_->setMotorTachoLimit(portC, 300, 100);
    }

}

void MainWindow::moveForward()
{
    // move robot forward
    moveRobot( 10 );
}

void MainWindow::moveBackward()
{
    // move robot forward
    moveRobot( -10 );
}

void MainWindow::rotateLeft()
{
    // move robot forward
    rotateRobot( -10 );
}

void MainWindow::rotateRight()
{
    // move robot forward
    rotateRobot( 10 );
}
