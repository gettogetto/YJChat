#ifndef YJCHATSERVER_H
#define YJCHATSERVER_H

#include <QtWidgets/QMainWindow>
#include<qmutex.h>
#include "ui_yjchatserver.h"
class QUdpSocket;
class ServerToClient;
class YJChatServer : public QMainWindow
{
	Q_OBJECT

public:
	enum MESSAGETYPE { MESSAGE, ONEONLINE, ONEOFFLINE, UPDATE,ALLTABLEUPDATE };
	YJChatServer(QWidget *parent = 0);
	~YJChatServer();
	public slots:
	void read_and_process_datagram();
	//void send_all_tablewidget_information();
private:
	Ui::YJChatServerClass ui;
	ServerToClient *m_server_to_client;
	QUdpSocket *m_udp_socket;//the socket to read from client
	qint16 m_port;

	QMutex m_mutex;

	
private:
	QString get_ip();
	void init_udp();
	void init_connection();
	void init_ip_port();
	void tableWidget_add_one(const QString& userName, const QString& hostName, const QString& ip);
	void tableWidget_delete_one(const QString& ip);
	void tableWidget_update_userName(const QString& userName, const QString& ip, const QString& currentTime);



	friend class ServerToClient;
};

#endif // YJCHATSERVER_H
