#ifndef YJCHAT_H
#define YJCHAT_H

#include <QtWidgets/QMainWindow>
#include<qmutex.h>
#include<vector>
#include "ui_yjchat.h"
#include"ui_client2client.h"
#include"PersonInformation.h"
class QUdpSocket;
class ClientToServer;
class ClientToClient;

class YJChat : public QMainWindow
{
	Q_OBJECT

public:
	enum MESSAGETYPE{MESSAGE,ONEONLINE,ONEOFFLINE,UPDATE, ALLTABLEUPDATE};
	YJChat(QWidget *parent = 0);
	~YJChat();

	public slots:
	void sendButton_clicked();
	void read_and_process_datagram();
	void send_new_username(QTableWidgetItem*);

	void new_client_to_client_dialog(QTableWidgetItem *);

private:
	Ui::YJChatClass ui;

	QUdpSocket *m_udp_socket;
	ClientToServer* m_client_to_server;
	ClientToClient* m_newClientToClient;
	std::vector<ClientToClient*> m_client_to_client_vec;
	qint16 m_port;
	QMutex m_mutex;

	QString m_self_name;
	QString m_self_hostName;
	QString m_self_ip;

private:
	void init_udp();
	void init_connection();

	QString get_userName();

	QString get_ip();


	void new_one_connected();//tell server something changes
	void one_left();//tell server something changes

	void tableWidget_add_one(const QString& userName, const QString& hostName, const QString& ip);
	void tableWidget_delete_one(const QString& ip);
	void tableWidget_update_userName(const QString& userName, const QString& ip,const QString& currentTime);


	friend class ClientToServer;
};

#endif // YJCHAT_H
