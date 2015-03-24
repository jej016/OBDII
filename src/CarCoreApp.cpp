#include "CarCoreApp.h"
#include <iostream>

CarCoreApp::CarCoreApp(int &argc, char** argv[]) : QCoreApplication(argc, *argv) {
    connect(this, SIGNAL( aboutToQuit() ), SLOT( cleanupProgramAtExit() ), Qt::QueuedConnection );
    connect(this, SIGNAL( done() ), this, SLOT( quit() ), Qt::QueuedConnection );
}

CarCoreApp::~CarCoreApp(){
    // Nothing to do here after Port cleanup
}

void CarCoreApp::run() {

    this->conn = new serial::PortReaderWriter();
/* */
    if (!conn->serialConnect()) {
        qDebug() << "Could not connect!";
        this->exit(1);
    }


    if (this->conn->isConnected()) {
        qDebug() << "Connected to serial port "
                 << this->conn->getConnectedPortName();
    } else {
        qDebug() << "Not connected to a serial port!";
        this->exit(2);
    }


    { // Test sending characters, The device should identify itself

        QString instr="ATI";
        QTextStream qtin(stdin);

        /********************************************************
        bool go = true;
        while (go)
        {
            go = QString::compare(instr, "exit", Qt::CaseInsensitive);

            if (!go) QCoreApplication::quit();
            else
            {
                cout << ":) => ";
                qtin >> instr;
                QByteArray qbin = instr.toUtf8();
                if(!conn->sendCommand( qbin ))
                {
                 qDebug() << "Problem writing !!!!";
                }
                cout << "_____________________________________\n";
                QByteArray buff = conn->readLine();
                cout <<"Buff size 1: "<<buff.size()<<endl;
                buff.remove(0, instr.size()+1);
                cout << "Buff size 2: "<<buff.size()<<endl;
                cout <<"("<<buff.toStdString()<<")"<<endl;
            }
        }
        /*********************************************************/

    }
/* *
    int rpmVal = 0;
    { // Try to get the RPM
        conn->sendCommand("01 0C");
        QByteArray buff = conn->readLine();
        rpmVal = conn->decodeRPM(buff);
    }
/* *
    QString trobelCode = "";
    { // Try to get the Trouble Code
        conn->sendCommand("01 01");
        // TODO: Needs to be multiline aware
        QByteArray buff = conn->readLine();
        trobelCode = conn->decodeErr(buff);

    }
/*  */

    qDebug().noquote() << this->conn->decodeErr("41 01 83 07 65 04");

/* */
    emit done();
}

void CarCoreApp::cleanupProgramAtExit(){
    // Close down any open ports so we can GTFO
    conn->~PortReaderWriter();
}
