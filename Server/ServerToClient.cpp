#include<qudpsocket.h>
#include <qobject.h>
#include<qbytearray.h>
#include "ServerToClient.h"
#include"yjchatserver.h"


ServerToClient::ServerToClient(YJChatServer* parent) :QObject(parent)
{
	m_parent = parent;
	init_udp();
	init_connection();
}


ServerToClient::~ServerToClient()
{

}

void ServerToClient::send_newest_tableWidget_notice_to_all_clients(const QByteArray& datagram) {
	qDebug() << "send_newest_tableWidget_information_to_all_clients";
	m_udp_socket_to_clients_for_notice->writeDatagram(datagram, QHostAddress::Broadcast, m_client_port);
}
void ServerToClient::send_newest_tableWidget_information_to_all_clients() {

	QByteArray datagram;
	QDataStream dataStream(&datagram, QIODevice::WriteOnly);
	dataStream.setVersion(QDataStream::Qt_5_6);
	dataStream << YJChatServer::MESSAGETYPE::ALLTABLEUPDATE;
	int userNumbers = m_parent->ui.m_tableWidget->rowCount();
	qDebug() << userNumbers;
	for (int i = 0; i < userNumbers; i++) {

		dataStream<<m_parent->ui.m_tableWidget->item(i, 0)->text()
			<< m_parent->ui.m_tableWidget->item(i, 1)->text()
			<< m_parent->ui.m_tableWidget->item(i, 2)->text();
	}

	m_udp_socket_to_client_for_information->writeDatagram(datagram, QHostAddress::Broadcast, m_client_port);
}

void ServerToClient::init_udp() {
	m_client_port = 5555;
	m_udp_socket_to_clients_for_notice = new QUdpSocket(m_parent);
	m_udp_socket_to_client_for_information = new QUdpSocket(m_parent);
}

void ServerToClient::init_connection() {


}