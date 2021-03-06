#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "PortReaderWriter.h"
#include "qcustomplot.h"
#include <QTextDocument>
#include <QFileDialog>
#include <QThread>
#include <QMutex>
#include "qcpdocumentobject.h"
#include "obd2client.h"


namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setupSpeedGraph(QCustomPlot *customPlot);
    void setupRPMGraph(QCustomPlot *customPlot);
    void setupGraph(QCustomPlot *customPlot, QString dataName, bool dataClicked);

private slots:

    void monitorData();
    void testData();

    void on_checkEngineButton_clicked();
    void on_monitorButton_clicked();
    void on_submitButton_clicked();
    void sendRawData();
    void on_connectButton_clicked();
    void on_speedBox_clicked();
    void on_rpmBox_clicked();
    void on_addGraphButton_clicked();
    void on_saveGraphButton_clicked();
    void on_uploadButton_clicked();
    void on_barometricPressureBox_clicked();
    void on_distanceTraveledBox_clicked();
    void on_engineCoolantBox_clicked();
    void on_engineLoadBox_clicked();
    void on_engineOilBox_clicked();
    void on_ethanolFuelBox_clicked();
    void on_fuelAirBox_clicked();
    void on_fuelLevelBox_clicked();
    void on_fuelPressureBox_clicked();
    void on_intakeAirBox_clicked();
    void on_intakeManifoldBox_clicked();
    void on_runTimeBox_clicked();
    void on_throtlePositionBox_clicked();


    void on_inputEdit_returnPressed();
    void on_actionAbout_triggered();

    void on_sbTimeout_valueChanged(int newVal);
    void on_jsonSave_clicked();

private:
    QThread * t;
    QMutex mtx;

    bool visibility;
    bool monitorDataLoop;

    Ui::MainWindow *ui;
    serial::PortReaderWriter *conn;
    bool connected;

    QVector<double> vspeed;
    QVector<double> vrpm;
    QVector<double> vpressure;
    QVector<double> vdistance;
    QVector<double> vcooltemp;
    QVector<double> vload;
    QVector<double> voiltemp;
    QVector<double> vethanolpercent;
    QVector<double> vratio;
    QVector<double> vfuelpercent;
    QVector<double> vlevel;
    QVector<double> vfuelpressure;
    QVector<double> vairtemp;
    QVector<double> vmanifoldpressure;
    QVector<double> vruntime;
    QVector<double> vthrotlepercent;
    OBD2Client clientsocet;

    QVector<QPair<QString,QVector<double> > > CollectData();

};

#endif // MAINWINDOW_H
