#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent* event);

private slots:
    void on_sequentialWalkBt_clicked();
    void on_nearestPeakBt_clicked();
    void on_backtrackingBt_clicked();
    void setRouteText(QString str);

private:
    Ui::MainWindow *ui;

    void checkPoints();
    void disabledAllBt();
    void enabledAllBt();
    qreal distanceFromModel(int qRow, int qColumn);
};
