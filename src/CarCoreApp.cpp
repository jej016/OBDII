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

    if (!conn->serialConnect()) {
        qDebug() << "Could not connect!";
        this->exit(1);
    }

    { // Test sending characters, The device should identify itself
        conn->sendCommand(QByteArray("AT I"));
        QByteArray buff = conn->readLine();
        qDebug() << buff << endl;
    }

    if (this->conn->isConnected()) {
        qDebug() << "Connected to serial port "
                 << this->conn->getConnectedPortName();
    } else {
        qDebug() << "Not connected to a serial port!";
        this->exit(2);
    }

    {
	QString instr="ATI";
	QTextStream qtin(stdin);

	bool go = true;
	while (go) {
	    std::cout << ":) => ";
	    qtin >> instr;

	    go = QString::compare(instr, "exit", Qt::CaseInsensitive);

	    if (!go) QCoreApplication::quit();
	    else
	    {
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
    }

    emit done();
}

void CarCoreApp::cleanupProgramAtExit(){
    // Close down any open ports so we can GTFO
    conn->~PortReaderWriter();
}
