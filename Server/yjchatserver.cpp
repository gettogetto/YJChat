/*qt designer 先：布局-窗体布局中布局，再：布局-垂直布局，这样就可以使得qt器件随窗口比例变化
每个layout中的layoutStretch可以调整各部件的大小比例
*/
#include<qudpsocket.h>
#include<qdatetime.h>
#include<qlineedit.h>
#include<qnetworkinterface.h>
#include<qmutex.h>
#include"ServerToClient.h"
#include "yjchatserver.h"

YJChatServer::YJChatServer(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_server_to_client = new ServerToClient(this);
	init_udp();
	init_ip_port();
	init_connection();

}

YJChatServer::~YJChatServer()
{

}

void YJChatServer::init_udp() {
	m_port = 6666;
	m_udp_socket = new QUdpSocket(this);
	m_udp_socket->bind(QHostAddress(get_ip()), m_port);
}
void YJChatServer::init_connection() {
	connect(m_udp_socket, SIGNAL(readyRead()), this, SLOT(read_and_process_datagram()));
}
void YJChatServer::init_ip_port() {

	ui.m_IP_lineEdit->setText(m_udp_socket->localAddress().toString());
	ui.m_port_lineEdit->setText(tr("%1").arg(m_udp_socket->localPort()));
}

QString YJChatServer::get_ip() {
	QList<QHostAddress> list = QNetworkInterface::allAddresses();
	foreach(QHostAddress ip, list) {
		if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
			return ip.toString();
		}
	}

	return 0;
}

void YJChatServer::read_and_process_datagram() {
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

		case ONEONLINE:
		{
			qDebug() << "ONEONLINE";
			QString userName;
			QString localHostName;
			QString ip;

			dataStream >> userName >> localHostName >> ip;

			ui.m_textBrowser->setTextColor(Qt::darkGray);
			ui.m_textBrowser->setCurrentFont(QFont("Times New Roman", 12));
			ui.m_textBrowser->append("[ " + userName + " ] " + "online! " + current_time);
			tableWidget_add_one(userName, localHostName, ip);
			//send the newest tablewidget information to all clients
			m_server_to_client->send_newest_tableWidget_notice_to_all_clients(datagram);
			break;
		}
		case ONEOFFLINE:
		{
			qDebug() << "ONEOFFLINE";
			QString userName;
			QString localHostName;
			QString ip;

			dataStream >> userName >> localHostName >> ip;
			tableWidget_delete_one(ip);

			ui.m_textBrowser->setTextColor(Qt::darkGray);
			ui.m_textBrowser->setCurrentFont(QFont("Times New Roman", 12));
			ui.m_textBrowser->append("[ " + userName + " ] " + "offline! " + current_time);

			//send the newest tablewidget information to all clients
			m_server_to_client->send_newest_tableWidget_notice_to_all_clients(datagram);
			break;
		}
		case UPDATE:
		{
			qDebug() << "UPDATE";
			QString userName;
			QString localHostName;
			QString ip;

			dataStream >> userName >> localHostName >> ip;
			tableWidget_update_userName(userName, ip, current_time);

			//send the newest tablewidget information to all clients
			m_server_to_client->send_newest_tableWidget_notice_to_all_clients(datagram);
			break;
		}
		default:break;
		}

		emit m_server_to_client->send_newest_tableWidget_information_to_all_clients();
	}

	//every time the server table update ,send the signal to all clients to update their table
	

}

void YJChatServer::tableWidget_add_one(const QString& userName, const QString& hostName, const QString& ip) {

	ui.m_tableWidget->insertRow(0);
	ui.m_tableWidget->setItem(0, 0, new QTableWidgetItem(userName));
	ui.m_tableWidget->setItem(0, 1, new QTableWidgetItem(hostName));
	ui.m_tableWidget->setItem(0, 2, new QTableWidgetItem(ip));

}

void YJChatServer::tableWidget_delete_one(const QString& ip) {
	auto list = ui.m_tableWidget->findItems(ip, Qt::MatchExactly);
	if (!list.isEmpty()) {
		ui.m_tableWidget->removeRow(list[0]->row());
		//qDebug() << ui.m_tableWidget->rowCount();
	}
}

void YJChatServer::tableWidget_update_userName(const QString& userName, const QString& ip, const QString& current_time) {
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