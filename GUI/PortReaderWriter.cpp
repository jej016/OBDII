/* 
 * Author: Vitaly, Zac Slade
 * 
 */

#include "PortReaderWriter.h"
#include <iostream>
#include <string>
#include <QString>


using namespace std;

namespace serial
{

    PortReaderWriter::PortReaderWriter(QSerialPort *reqPort
                                       , QByteArray *dataForWrite
                                       , QObject *parent)
        : QObject(parent)
        , port(reqPort)
        , writeData(dataForWrite)
    {
	if ( 0 == this->port ) { 
	    this->m_timer.start(timeoutMillis);
	    return;
	}

        connect(port
                , SIGNAL( readReady() )
                , SLOT( handleReadReady() )
                , Qt::QueuedConnection);

        connect(port
                , SIGNAL( error(QSerialPort::SerialPortError) )
                , SLOT(handleError(QSerialPort::SerialPortError) )
                , Qt::QueuedConnection);

        connect(&m_timer
                , SIGNAL( timeout() )
                , SLOT( handleTimeout() )
                , Qt::QueuedConnection);

	this->m_timer.start(timeoutMillis);
    }

    PortReaderWriter::~PortReaderWriter(void) {
        if (this->port != 0) { this->port->close(); }
    }

    bool PortReaderWriter::serialConnect(void)
	{ 
        string input_device = "";
        bool gotit= false;
        QStringList str;
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            str.append( "Name        : " + info.portName());
            if (info.manufacturer().contains("FTDI"))
            {
                input_device = info.portName().toStdString();
                gotit = true;
            }
            str.append("Description : " + info.description());
            str.append("Manufacturer: " + info.manufacturer() );
        }
        // If did not finde ask user for input
        if(!gotit)
        {
            foreach(const QString elem, str)
            {
                qDebug() << elem << endl;
                cout << "Which serial device Name should we use?" << endl;
                cin >> input_device;
            }
        }

        // Just try it!
        this->port = new QSerialPort(QString(input_device.c_str()));
        // Default for the device under test is 10400 baud
        this->port->open(QIODevice::ReadWrite);
        port->setBaudRate(QSerialPort::Baud38400);

        if (port->isOpen()){
	    connect(port
		    , SIGNAL( readyRead() )
		    , SLOT( handleReadReady() )
		    , Qt::QueuedConnection);

	    connect(&m_timer
		    , SIGNAL( timeout() )
		    , SLOT( handleTimeout() )
		    , Qt::QueuedConnection);

	    return true;
	}
        return false;
    }

    bool PortReaderWriter::sendCommand(const QByteArray &data) {
	if (0 == this->port) { return false; }

	this->port->open(QIODevice::ReadWrite);
        if (!this->port->isOpen()) { return false; }
        if (-1 < port->write(data)) { return true; }
	if (this->port->waitForBytesWritten(timeoutMillis)) { return true; }
        return false;
    }

    QByteArray PortReaderWriter::readLine() {
	if (0 == this->port) { return "No port set!"; }

	this->port->open(QIODevice::ReadWrite);
        if (!this->port->isOpen()) { return "Could not open port for write"; }
	this->port->waitForReadyRead(timeoutMillis);
	QByteArray scratch = this->port->readAll();
	while (this->port->waitForReadyRead(timeoutMillis) ) { scratch.append(this->port->readAll()); }
	return scratch;
    }

    /***********************************************/
    int PortReaderWriter::decodeRPM(const QByteArray line_data) {
        //QString comm = "01 0C"; // the code for rpm

        sendCommand(line_data);



        QString retval = "1af8";
        int x = 0;
        bool ok;

        x = (retval.toInt(&ok, 16))/4;

        if(ok) return x;
        else return -1;
    }

    int PortReaderWriter::decodeTempEngin(const QByteArray line_data) {

        //QString comm = "01 05 1"; // the code Enginr Tempereture

        QString retval = "1af8";
        int x = 0;
        bool ok;

        x = (retval.toInt(&ok, 16)) - 40;

        if(ok) return x;
        else return -1;
    }

    QString PortReaderWriter::decodeErr(const QByteArray line_data)
    {

        //QString comm = "01 01"; // the code for Error code
        /*
         *  41 01 respons to the request
         *  81 number of corrent troble codes
         * 	81 - 80 or (129)- 120
         * 	most segnificant bit indicate that the  Ceck Engine Light on or of.
         */

        QByteArray teststr = "a133";
        QString newtest;
        QString retval = line_data;
        QString r_str;
        QString *errors;
        bool ok;
        QString str;
        int x=0;


        /*
         * > 03 is will give you all the actual troble codes
         *  like 01 33 00 00 00 00
         *  most of it is junk we need only  0133
         * 	and 0 = P0 and rest is as is
         * 	 so the code will be P0133
         */

        /*
         * here I just plaing around trying to see how it will work and what
         * to do
         */

        retval.replace(" ","");
        str = retval.replace(" ","");
        r_str = str.mid(4,2);

        x = (r_str.toInt(&ok, 16)) - 128;

        errors = new QString[x];




        for (int i=0; i<x; i++)
        {
            sendCommand("03");
            teststr = readLine();
            if(r_str.left(1) == "0")
            {
                newtest = r_str.mid(1);

                if (teststr.left(1) == "0")
                    newtest.prepend("P0"); else
                if (teststr.left(1) == "1")
                    newtest.prepend("P1"); else
                if (teststr.left(1) == "2")
                    newtest.prepend("P2");  else
                if (teststr.left(1) == "3")
                    newtest.prepend("P3"); else
                ///////////////////////////////////////
                if (teststr.left(1) == "4")
                    newtest.prepend("C0"); else
                if (teststr.left(1) == "5")
                    newtest.prepend("C1"); else
                if (teststr.left(1) == "6")
                    newtest.prepend("C2"); else
                if (teststr.left(1) == "7")
                    newtest.prepend("C3"); else
                    ///////////////////////////////////////
                if (teststr.left(1) == "8")
                    newtest.prepend("B0"); else
                if (teststr.left(1) == "9")
                    newtest.prepend("B1"); else
                if (teststr.left(1) == "A" || teststr.left(1) == "a")
                    newtest.prepend("B2"); else
                if (teststr.left(1) == "B" || teststr.left(1) == "b")
                    newtest.prepend("B3"); else
                    ////////////////////////////////////////
                if (teststr.left(1) == "C" || teststr.left(1) == "c")
                    newtest.prepend("U0");else
                if (teststr.left(1) == "D" || teststr.left(1) == "d")
                    newtest.prepend("U1");else
                if (teststr.left(1) == "E" || teststr.left(1) == "e")
                    newtest.prepend("U2");else
                if (teststr.left(1) == "F" || teststr.left(1) == "f")
                    newtest.prepend("U3");
            }
            errorCodes[i] = newtest;
        }

        /*   Just testing
        for (int i=0; i<x; i++)
        {
            qDebug() << "Error Code "<< i << " : "  << errors[i];
        }
        */


        return newtest;
    }

    QString PortReaderWriter::getConnectedPortName() {
        if (0 == port) return "";
        if (!this->port->isOpen()) return "";
        return this->port->portName();
    }

    bool PortReaderWriter::isConnected(){
        if (0 == port) return false;
        if (this->port->isOpen()) return true;
        return false;
    }

    void PortReaderWriter::handleError(QSerialPort::SerialPortError err) {
	// What to do with this error?

    }

    void PortReaderWriter::handleTimeout() {
	// Hrm...  A Timeout

    }

    void PortReaderWriter::handleReadReady() {
	// Read data
    }
}