#pragma once
#include<vector>

#include<qwidget.h>
#include"ui_client2client.h"
#include"PersonInformation.h"
#include"FileTcpSocketDialog.h"
class YJChat;
class QTableWidgetItem;
class QTcpSocket;
class QUdpSocket;
class QDialog;
class QCloseEvent;

class ClientToClient:public QWidget
{
	Q_OBJECT
public:
	Ui::p2pDialog* m_p2pui;
	QDialog* m_p2pDialog;
private:
	//udp
	QUdpSocket* m_udp_socket;
	qint16 m_udp_port;
	//tcp
	FileTcpSocketDialog* m_file_tcp_socket_dialog;

	PersonInformation m_personInformationOppo;//opposite side information

	PersonInformation m_personInformationSelf;//self information
public:
	ClientToClient(const PersonInformation& personInformationSelf,
		const PersonInformation& personInformationOppo, 
		Ui::p2pDialog* p2pui=nullptr , 
		QDialog* p2pDialog=nullptr,
		YJChat* parent=nullptr
		);
	~ClientToClient();

	void init_udp();
	void init_tcp();

	void init_connection();
	public slots:
	void send_button_clicked();
	void read_and_process_datagram();
	void close_button_clicked();


	friend class YJChat;
};

