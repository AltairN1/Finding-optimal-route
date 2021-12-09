#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "tableModel.h"
#include "SequentialWalkThread.h"
#include "nearestPeakThread.h"
#include "BacktrackingThread.h"

#include <QMessageBox>

namespace {
const QList<QPointF> basePoints = {{56,34}, {9,60}, {60,89}, {13,77}, {52,35},
                    {69,93}, {34,90}, {71,80}, {96,32}, {58,3}};
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setFixedSize(size());
    ui->setupUi(this);
    tableModel* model = new tableModel(basePoints);
    ui->distanceBetweenNodes->setModel(model);
    ui->distanceBetweenNodes->resizeColumnsToContents();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    QApplication::quit();
}

void MainWindow::on_sequentialWalkBt_clicked()
{
    checkPoints();
    disabledAllBt();
    SequentialWalkThread* thread = new SequentialWalkThread(ui->distanceBetweenNodes->model(), basePoints.size());
    connect(thread, &SequentialWalkThread::route, this, &MainWindow::setRouteText);
    connect(thread, &SequentialWalkThread::finished, thread, &SequentialWalkThread::deleteLater);
    thread->start();
}

void MainWindow::on_nearestPeakBt_clicked()
{
    checkPoints();
    disabledAllBt();
    NearestPeakThread* thread = new NearestPeakThread(ui->distanceBetweenNodes->model(), basePoints.size());
    connect(thread, &NearestPeakThread::route, this, &MainWindow::setRouteText);
    connect(thread, &NearestPeakThread::finished, thread, &NearestPeakThread::deleteLater);
    thread->start();
}

void MainWindow::on_backtrackingBt_clicked()
{
    checkPoints();
    disabledAllBt();
    BacktrackingThread* thread = new BacktrackingThread(ui->distanceBetweenNodes->model(), basePoints.size());
    connect(thread, &BacktrackingThread::route, this, &MainWindow::setRouteText);
    connect(thread, &BacktrackingThread::finished, thread, &BacktrackingThread::deleteLater);
    thread->start();
}

void MainWindow::checkPoints()
{
    if (basePoints.size() < 2 ) {
        QMessageBox::warning(this, "Внимание" , "Вершин не может быть меньше 2!");
        return;
    } else if (ui->distanceBetweenNodes->model()->columnCount() < (basePoints.size() - 1)) {
        QMessageBox::warning(this, "Внимание", "Вершин не может быть больше, чем столбцов у модели!");
        return;
    }
}

void MainWindow::setRouteText(QString str)
{
    ui->routeAndDistance->setText(str);
    enabledAllBt();
}

void MainWindow::disabledAllBt()
{
    ui->sequentialWalkBt->setDisabled(true);
    ui->nearestPeakBt->setDisabled(true);
    ui->backtrackingBt->setDisabled(true);
}

void MainWindow::enabledAllBt()
{
    ui->sequentialWalkBt->setEnabled(true);
    ui->nearestPeakBt->setEnabled(true);
    ui->backtrackingBt->setEnabled(true);
}


