#pragma once

#include"ui_yjchatserver.h"
class QUdpSocket;
class YJChatServer;
class QByteArray;
class ServerToClient :public QObject
{
	Q_OBJECT
public:
	ServerToClient(YJChatServer* parent = nullptr);
	~ServerToClient();
	public slots:
	void send_newest_tableWidget_notice_to_all_clients(const QByteArray& datagram);
	void send_newest_tableWidget_information_to_all_clients();
private:
	YJChatServer* m_parent;
	QUdpSocket* m_udp_socket_to_clients_for_notice;
	QUdpSocket* m_udp_socket_to_client_for_information;
	quint16 m_client_port;
private:
	void init_udp();
	void init_connection();

};

