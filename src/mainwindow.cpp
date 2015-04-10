#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include "qcustomplot.h"

QVector<double> a(101), b(101);
QVector<double> c(101), d(101);
bool visibility;
bool speedClicked;
bool rpmClicked;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->conn = new serial::PortReaderWriter();

    ui->setupUi(this);
    setWindowTitle("Group 2 Software Engineering ODBII Reader");

    /* disable these widgets when there is no connection present */
    ui->submitButton->setDisabled(true);
    ui->checkEngineButton->setDisabled(true);
    ui->monitorButton->setDisabled(true);

    speedCount = 1;
    rpmCount = 1;
    a[0] = 0;
    b[0] = 0;
    c[0] = 0;
    d[0] = 0;

    visibility = true;
    speedClicked = false;
    rpmClicked = false;

    QFont font;
    font.setPointSize(12);
    ui->statusLabel->setFont(font);
    ui->statusLabel->setText("Connection Status: Disconnected");

    ui->connectStatus->setValue(0);

    ui->customPlot->addGraph();
    ui->customPlot->addGraph();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// sample graph for testing demonstration
void MainWindow::setupQuadraticDemo(QCustomPlot *customPlot)
{
  // generate some data:
  QVector<double> x(101), y(101); // initialize with entries 0..100

  for (int i=0; i<10; i++)
  {
      x[i] = i;
      switch (i)
      {
      case 0: y[i] = 0; break;
      case 1: y[i] = 0; break;
      case 2: y[i] = 1; break;
      case 3: y[i] = 2; break;
      case 4: y[i] = 4; break;
      case 5: y[i] = 6; break;
      case 6: y[i] = 5; break;
      case 7: y[i] = 4; break;
      case 8: y[i] = 1; break;
      case 9: y[i] = 0; break;
      }
  }
  // create graph and assign data to it:
  customPlot->addGraph();
  customPlot->graph(0)->setData(x, y);
  // give the axes some labels:
  customPlot->xAxis->setLabel("Time");
  customPlot->yAxis->setLabel("Value");
  // set axes ranges, so we see all data:
  customPlot->xAxis->setRange(0, 10);
  customPlot->yAxis->setRange(0, 7);
  customPlot->graph(0)->setPen(QPen(Qt::red)); // line color blue for first graph
  customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
  customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
}

void MainWindow::on_checkEngineButton_clicked()
{
    QString trobelCode = "";
    { // Try to get the Trouble Code
        conn->sendCommand("01 01");
        // TODO: Needs to be multiline aware
        QByteArray buff = conn->readLine();
        trobelCode = conn->decodeErr(buff);
        ui->outputBrowser->setText(trobelCode);
    }
}

void MainWindow::on_monitorButton_clicked()
{
    int vehicleSpeed = 0;
   { // Try to get the RPM
       conn->sendCommand("01 0D");
       QByteArray buff = conn->readLine();
       vehicleSpeed = conn->decodeVehicleSpeed(buff);
   }
   ui->outputBrowser->setText(  QString::number(vehicleSpeed)  );
    if(vehicleSpeed < 0)
    {
        vspeed.append(0);
        speedCount++;
    }
    else
    {
        vspeed.append(vehicleSpeed);
        speedCount++;
    }

    int rpmVal = 0;
    { // Try to get the RPM
        conn->sendCommand("01 0C");
        QByteArray buff = conn->readLine();
        rpmVal = conn->decodeRPM(buff);
    }
    ui->outputBrowser->setText( QString::number(rpmVal) );
    if (rpmVal < 0)
    {
       vrpm.append(rpmCount);
       rpmCount++;
    }
    else
    {
        vrpm.append(rpmVal/1000);
        rpmCount++;
    }


    // not necessary now, but the code to set
    // the other buttons to disabled while
    // monitoring the RPM
    ui->checkEngineButton->setDisabled(visibility);
    ui->submitButton->setDisabled(visibility);
    ui->connectButton->setDisabled(visibility);
    visibility = !visibility;

    speedClicked = !speedClicked;
    rpmClicked = !rpmClicked;
    on_rpmBox_clicked();
    on_speedBox_clicked();
}

void MainWindow::on_submitButton_clicked()
{
    sendcommand();
}

void MainWindow::on_speedBox_clicked()
{
    setupSpeedGraph(ui->customPlot);
}

void MainWindow::on_rpmBox_clicked()
{
    setupRPMGraph(ui->customPlot);
}

void MainWindow::connect()
{
    QString infoout="";

    if (!conn->serialConnect())
    {
        ui->outputBrowser->setText("Could not connect!");
    }


    if (this->conn->isConnected())
    {
        infoout = "Connected to serial port!" +  this->conn->getConnectedPortName();
        ui->outputBrowser->setText(infoout);
    }
    else
    {
        ui->outputBrowser->setText("Not connected to a serial port!");
    }
}

void MainWindow::sendcommand()
{

    QString tempstr="";

    QString instr = ui->inputEdit->text();
    QByteArray qbin = instr.toUtf8();

    if(!conn->sendCommand( qbin ))
    {
        ui->outputBrowser->append( "Problem writing !!!!");
    }
    QByteArray buff = conn->readLine();
    tempstr = "Buff size 1: ";
    tempstr += QString::number(buff.size());
    ui->outputBrowser->append( tempstr );

    buff.remove(0, instr.size()+1);

    tempstr = "Buff size 2: ";
    tempstr += QString::number( buff.size() );

    ui->outputBrowser->append( tempstr );

    tempstr = "(";
    tempstr += buff;
    tempstr += ")" ;

    ui->outputBrowser->append( tempstr );

    ui->inputEdit->selectAll();
}

void MainWindow::setupSpeedGraph(QCustomPlot *customPlot)
{
  if (speedClicked == false){
      for (int i=0; i<=speedCount; i++)
      {
        c[i] = i;
      }

      // create graph and assign data to it:
      //customPlot->addGraph();
      customPlot->graph(1)->setData(c,vspeed);

      // give the axes some labels:
      customPlot->xAxis->setLabel("Count");
      customPlot->yAxis->setLabel("Speed (MPH)");

      // set axes ranges, so we see all data:
      customPlot->xAxis->setRange(0, speedCount - 1);
      customPlot->yAxis->setRange(0, 80);
      customPlot->graph(1)->setPen(QPen(Qt::red)); // line color blue for first graph
      customPlot->graph(1)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
      customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
      ui->customPlot->replot();
  }
  else{
      ui->customPlot->removeGraph(1);
      ui->customPlot->addGraph();
      ui->customPlot->replot();
  }
  speedClicked = !speedClicked;
}

void MainWindow::setupRPMGraph(QCustomPlot *customPlot)
{
  if (rpmClicked == false){
      for (int i=0; i<=rpmCount; i++)
      {
          a[i] = i;
      }
      // create graph and assign data to it:
      //customPlot->addGraph();
      customPlot->graph(0)->setData(a, vrpm);
      // give the axes some labels:
      customPlot->xAxis->setLabel("Count");
      customPlot->yAxis->setLabel("RPM (x1000)");
      // set axes ranges, so we see all data:
      customPlot->xAxis->setRange(0, rpmCount - 1);
      customPlot->yAxis->setRange(0, 10);
      customPlot->graph(0)->setPen(QPen(Qt::red));
      // first graph will be filled with translucent blue
      customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
      ui->customPlot->replot();
  } else{
      ui->customPlot->removeGraph(0);
      ui->customPlot->addGraph();
      ui->customPlot->replot();
  }
  rpmClicked = !rpmClicked;
}

void MainWindow::on_connectButton_clicked()
{
    QString input_device = "";
    QVector<QSerialPortInfo> ports = serial::PortReaderWriter::getAvailPorts();

    foreach (const QSerialPortInfo &info, ports) {
        if (info.manufacturer().contains("FTDI")) {
            input_device = info.portName();
        }
    }

    if (this->conn->setPort(input_device.toStdString())) {
        QFont font;
        font.setPointSize(12);
        QString text = "Connected: " + input_device;
        ui->statusLabel->setText(text);
        ui->statusLabel->setFont(font);
        ui->connectStatus->setValue(100);
        ui->connectButton->setText(QApplication::translate("MainWindow", "Disconnect", 0));

        /* enable these widgets when there is a connection present */
        ui->checkEngineButton->setEnabled(true);
        ui->submitButton->setEnabled(true);
        ui->monitorButton->setEnabled(true);
        return;
    }

    QFont font;
    font.setPointSize(12);
    ui->statusLabel->setText("Connection Status: Disconnected");
    ui->statusLabel->setFont(font);
    ui->connectStatus->setValue(0);
    ui->connectButton->setText(QApplication::translate("MainWindow", "Connect", 0));

    /* We disconnected.  We should clean up the PortReaderWriter and get new buffers. */
    delete this->conn;
    this->conn = new serial::PortReaderWriter();

    /* disable these widgets when there is no connection */
    ui->checkEngineButton->setDisabled(true);
    ui->submitButton->setDisabled(true);
    ui->monitorButton->setDisabled(true);

    return;
}

void MainWindow::on_inputEdit_returnPressed()
{
   sendcommand();
}
