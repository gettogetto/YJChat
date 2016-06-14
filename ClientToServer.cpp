#include<qudpsocket.h>
#include<qdatastream.h>
#include <qobject.h>
#include"yjchat.h"
#include "ClientToServer.h"



ClientToServer::ClientToServer(YJChat* parent):QObject(parent)
{
	m_parent = parent;
	init_udp();
	init_connection();
}


ClientToServer::~ClientToServer()
{

}

void ClientToServer::init_udp() {
	
	m_server_port = 6666;
	m_server_ip = "222.205.105.254";//get the server ip from the lineEdit
	m_udp_socket_toServer = new QUdpSocket(m_parent);

}

void ClientToServer::init_connection() {

}
void ClientToServer::set_server_ip() {
	m_server_ip = m_parent->ui.m_serverIpEdit->text();
	emit m_parent->new_one_connected();//conenct to the server
	qDebug() << "set_server_ip" << m_server_ip;
}

void ClientToServer::send_message_to_server(const QByteArray& datagram) {
	
	if (m_server_ip.isEmpty()) {
		return;
	}
	
	qDebug() << "send_message_to_server";
	m_udp_socket_toServer->writeDatagram(datagram,QHostAddress(m_server_ip),m_server_port);
}
