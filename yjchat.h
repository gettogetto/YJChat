#ifndef YJCHAT_H
#define YJCHAT_H

#include <QtWidgets/QMainWindow>
#include "ui_yjchat.h"
class QUdpSocket;
class ClientToServer;
class YJChat : public QMainWindow
{
	Q_OBJECT

public:
	enum MESSAGETYPE{MESSAGE,ONEONLINE,ONEOFFLINE,UPDATE};
	YJChat(QWidget *parent = 0);
	~YJChat();

	public slots:
	void sendButton_clicked();
	void read_and_process_datagram();
	void send_new_username(QTableWidgetItem*);

private:
	Ui::YJChatClass ui;
	QUdpSocket *m_udp_socket;//client to client
	ClientToServer* m_client_to_server;
	qint16 m_port;

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
