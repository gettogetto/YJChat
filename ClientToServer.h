#pragma once
#include"ui_yjchat.h"
class YJChat;
class QUdpSocket;
class QDataStream;
class ClientToServer:public QObject
{
	Q_OBJECT
public:
	enum MESSAGETYPE { MESSAGE, ONEONLINE, ONEOFFLINE, UPDATE };
public:
	ClientToServer(YJChat* parent = nullptr);
	~ClientToServer();
	void send_message_to_server(const QByteArray& datagram);
private:
	YJChat* m_parent;
	QUdpSocket *m_udp_socket_toServer;
	qint16 m_server_port;
	QString m_server_ip;
private:
	void init_udp();
	void init_connection();
	public slots:
	void set_server_ip();
};

