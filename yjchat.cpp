#include<qudpsocket.h>
#include<qtextbrowser.h>
#include<qtextedit.h>
#include<qmessagebox.h>
#include<qhostaddress.h>
#include<qhostinfo.h>
#include<qprocess.h>
#include<qnetworkinterface.h>
#include<qdatetime.h>

#include"ClientToServer.h"
#include"ClientToClient.h"
#include "yjchat.h"


YJChat::YJChat(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	ui.m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_client_to_server = new ClientToServer(this);
	//m_client_to_client = new ClientToClient(this);

	init_udp();
	init_connection();
}

YJChat::~YJChat()
{
	emit one_left();//when a user left emit the signal one_left()
	
}
void YJChat::init_udp() {
	/********************************UDP*********************************************/
	m_udp_socket = new QUdpSocket(this);
	m_port = 5555;//every client port is 5555
	m_udp_socket->bind(m_port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
	/****************ReuseAddressHint****************
	Qt assistant explain£º
	Provides a hint to QAbstractSocket that it should try to rebind the service even if the address
	and port are already bound by another socket. On Windows and Unix,
	this is equivalent to the SO_REUSEADDR socket option.
	*/
	emit new_one_connected();//When a new udpsocket occured,emit the signal new_one_connect();
}

void YJChat::init_connection() {
	connect(ui.m_sendButton, SIGNAL(clicked()), this, SLOT(sendButton_clicked()));
	connect(ui.m_serverIp_okButton, SIGNAL(clicked()), m_client_to_server, SLOT(set_server_ip()));
	connect(m_udp_socket, SIGNAL(readyRead()), this, SLOT(read_and_process_datagram()));
	connect(ui.m_tableWidget, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(new_client_to_client_dialog(QTableWidgetItem *)));
	//to do rename emit signal
	//
}
QString YJChat::get_userName() {
	QStringList envVariables;
	envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
		<< "HOSTNAME.*" << "DOMAINNAME.*";
	QStringList environment = QProcess::systemEnvironment();
	foreach(QString string, envVariables) {
		int index = environment.indexOf(QRegExp(string));
		if (index != -1) {
			QStringList stringList = environment.at(index).split('=');
			if (stringList.size() == 2) {
				return stringList.at(1);
				break;
			}
		}
	}
	return "Unknown user";
}




QString YJChat::get_ip() {
	QList<QHostAddress> list = QNetworkInterface::allAddresses();
	foreach(QHostAddress ip, list) {
		if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
			return ip.toString();
		}
	}

	return 0;
}

void YJChat::sendButton_clicked() {
	qDebug() << "sendButton_clicked()";
	if (ui.m_textEdit->toPlainText().isEmpty()) {
		QMessageBox::warning(this, tr("Warning"), tr("Content is empty!"));
		return;
	}
	else {
		QByteArray datagram;
		QDataStream dataStream(&datagram, QIODevice::WriteOnly);
		dataStream.setVersion(QDataStream::Qt_5_6);
		QString userName = m_self_name/*get_userName()*/;
		QString localHostName = m_self_hostName/*QHostInfo::localHostName()*/;
		QString ip = m_self_ip/*get_ip()*/;
		QString message = ui.m_textEdit->toPlainText();

		dataStream << MESSAGETYPE::MESSAGE << userName << localHostName << ip << message;

		ui.m_textEdit->clear();

		m_udp_socket->writeDatagram(datagram, QHostAddress::Broadcast, m_port);

	}
}
//receive from other client's message or server's newest tablewidget information(online,offline,update) 
void YJChat::read_and_process_datagram() {
	QMutexLocker mutexLocker(&m_mutex);
	while (m_udp_socket->hasPendingDatagrams()) {

		QByteArray datagram;
		datagram.resize(m_udp_socket->pendingDatagramSize());
		//read the coming datagram
		m_udp_socket->readDatagram(datagram.data(), datagram.size());
		QDataStream dataStream(&datagram, QIODevice::ReadOnly);
		dataStream.setVersion(QDataStream::Qt_5_6);
		//get the time
		QDateTime current_date_time = QDateTime::currentDateTime();
		QString current_time = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
		//get the message type
		qint32 messageType;
		dataStream >> messageType;//message type
		switch (messageType) {
		case MESSAGE:
		{
			QString userName;
			QString localHostName;
			QString ip;
			QString message;

			dataStream >> userName >> localHostName >> ip >> message;

			ui.m_textBrowser->setTextColor(Qt::darkGreen);
			ui.m_textBrowser->setCurrentFont(QFont("Times New Roman", 12));
			ui.m_textBrowser->append("[ " + userName + " ] " + current_time);
			ui.m_textBrowser->setTextColor(Qt::black);
			ui.m_textBrowser->append(message);
			break;
		}
		case ONEONLINE:
		{
			QString userName;
			QString localHostName;
			QString ip;

			dataStream >> userName >> localHostName >> ip;

			ui.m_textBrowser->setTextColor(Qt::darkGray);
			ui.m_textBrowser->setCurrentFont(QFont("Times New Roman", 12));
			ui.m_textBrowser->append("[ " + userName + " ] " + "online! " + current_time);
			//let the server to fixed the tablewidget
			//tableWidget_add_one(userName, localHostName, ip);

			break;
		}
		case ONEOFFLINE:
		{
			QString userName;
			QString localHostName;
			QString ip;

			dataStream >> userName >> localHostName >> ip;
			//let the server to fixed the tablewidget
			//tableWidget_delete_one(ip);

			ui.m_textBrowser->setTextColor(Qt::darkGray);
			ui.m_textBrowser->setCurrentFont(QFont("Times New Roman", 12));
			ui.m_textBrowser->append("[ " + userName + " ] " + "offline! " + current_time);
			break;
		}
		//let the server to fixed the tablewidget
		case UPDATE:
		{
			//QString userName;
			//QString localHostName;
			//QString ip;

			//dataStream >> userName >> localHostName >> ip;
			//tableWidget_update_userName(userName, ip, current_time);

			break;
		}
		case ALLTABLEUPDATE:
		{
			qDebug() << "ALLTABLEUPDATE";
			//for (int i = 0; i < ui.m_tableWidget->rowCount(); i++) {
				//ui.m_tableWidget->removeRow(0);
			//}
			ui.m_tableWidget->clearContents();
			QString userName;
			QString localHostName;
			QString ip;
			while (!dataStream.atEnd()) {
				dataStream >> userName >> localHostName >> ip;
				qDebug() << userName << localHostName << ip;
				tableWidget_add_one(userName, localHostName, ip);
			}
			break;
		}
		default:break;
		}
	}

}
//update the server tablewidget
void YJChat::send_new_username(QTableWidgetItem* item) {
	if (item->column() != 0) return;

	QByteArray datagram;
	QDataStream dataStream(&datagram, QIODevice::WriteOnly);
	dataStream.setVersion(QDataStream::Qt_5_6);

	QString userName = item->text();
	QString localHostName = QHostInfo::localHostName();
	QString ip = get_ip();


	dataStream << MESSAGETYPE::UPDATE << userName << localHostName << ip;

	//m_udp_socket->writeDatagram(datagram, QHostAddress::Broadcast, m_port);

	m_client_to_server->send_message_to_server(datagram);
}
//when double click the table,emit this func
void YJChat::new_client_to_client_dialog(QTableWidgetItem* item) {

	qDebug() << "new_client_to_client_dialog";
	Ui::p2pDialog *ui_dialog = new Ui::p2pDialog();//will delete when ClientToClient destroy
	QDialog* dialog = new QDialog();//will delete when ClientToClient destroy
	PersonInformation personSelf(
		m_self_name,
		m_self_hostName,
		m_self_ip
		);
	PersonInformation personOppo(
		ui.m_tableWidget->item(item->row(), 0)->text(),
		ui.m_tableWidget->item(item->row(), 1)->text(),
		ui.m_tableWidget->item(item->row(), 2)->text()
		);
	//to fixed
	ClientToClient* newClientToClient = new ClientToClient(personSelf, personOppo, ui_dialog, dialog, this);

	dialog->setWindowTitle(tr("Talking with ") + personOppo.m_name + '@' + personOppo.m_ip);//set the dialog title =talking with some one
	dialog->show();
}


void YJChat::new_one_connected() {
	qDebug() << "new_one_connected()";
	QByteArray datagram;
	QDataStream dataStream(&datagram, QIODevice::WriteOnly);
	dataStream.setVersion(QDataStream::Qt_5_6);

	QString userName = get_userName(); 
	m_self_name = userName;
	QString localHostName = QHostInfo::localHostName();
	m_self_hostName = localHostName;
	QString ip = get_ip(); 
	m_self_ip = ip;

	dataStream << MESSAGETYPE::ONEONLINE << userName << localHostName << ip;

	//m_udp_socket->writeDatagram(datagram, QHostAddress::Broadcast, m_port);
	m_client_to_server->send_message_to_server(datagram);
}

void YJChat::one_left() {
	qDebug() << "one_left()";
	QByteArray datagram;
	QDataStream dataStream(&datagram, QIODevice::WriteOnly);
	dataStream.setVersion(QDataStream::Qt_5_6);
	QString userName = m_self_name/*get_userName()*/;
	QString localHostName = m_self_hostName/*QHostInfo::localHostName()*/;
	QString ip = m_self_ip/*get_ip()*/;

	dataStream << MESSAGETYPE::ONEOFFLINE << userName << localHostName << ip;

	m_client_to_server->send_message_to_server(datagram);
}
/*receive from the server and update the tablewidget*/
void YJChat::tableWidget_add_one(const QString& userName, const QString& hostName, const QString& ip) {

	ui.m_tableWidget->insertRow(0);
	ui.m_tableWidget->setItem(0, 0, new QTableWidgetItem(userName));
	ui.m_tableWidget->setItem(0, 1, new QTableWidgetItem(hostName));
	ui.m_tableWidget->setItem(0, 2, new QTableWidgetItem(ip));
	
}

void YJChat::tableWidget_delete_one(const QString& ip) {
	auto list = ui.m_tableWidget->findItems(ip, Qt::MatchExactly);
	if (!list.isEmpty()) {
		ui.m_tableWidget->removeRow(list[0]->row());
	}
}

void YJChat::tableWidget_update_userName(const QString& userName, const QString& ip, const QString& current_time) {
	qDebug() << "tableWidget_update_userName";
	auto list = ui.m_tableWidget->findItems(ip, Qt::MatchExactly);
	if (!list.isEmpty()) {
		if (ui.m_tableWidget->itemAt(list[0]->row(), 0)->text() == userName) return;
		ui.m_textBrowser->setTextColor(Qt::darkGray);
		ui.m_textBrowser->setCurrentFont(QFont("Times New Roman", 12));
		ui.m_textBrowser->append("[ " + ui.m_tableWidget->itemAt(list[0]->row(), 0)->text() + " ] " + " renamed to be" + "[" + userName + "]" + current_time);


		ui.m_tableWidget->itemAt(QPoint(list[0]->row(), list[0]->column()))->setText(QString(userName));
	}
}