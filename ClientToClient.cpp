#include "ClientToClient.h"
#include"yjchat.h"
#include<qobject.h>
#include<qudpsocket.h>
#include<qtcpsocket.h>
#include<qdatastream.h>
#include<qmessagebox.h>
#include<qmessageauthenticationcode.h>
#include<qtablewidget.h>
#include"ClientToClient.h"

ClientToClient::ClientToClient(const PersonInformation& personInformationSelf, const PersonInformation& personInformationOppo, YJChat* parent) :QWidget(parent)
{
	m_personInformationSelf = personInformationSelf;
	m_personInformationOppo = personInformationOppo;

	init_udp();
	init_tcp();
	init_connection();
}


ClientToClient::~ClientToClient()
{

}

void ClientToClient::init_udp() {
	m_udp_socket = new QUdpSocket(this);
	m_udp_port = 4444;
	m_udp_socket->bind(QHostAddress(m_personInformationSelf.m_ip),m_udp_port);
}

void ClientToClient::init_tcp() {
	m_tcp_socket = new QTcpSocket(this);
	m_tcp_port = 3333;
	m_tcp_socket->bind(QHostAddress(m_personInformationSelf.m_ip), m_tcp_port);
}

void ClientToClient::init_connection() {
	connect(m_p2pui->m_sendButton, SIGNAL(clicked()), this, SLOT(send_button_clicked()));
}

void ClientToClient::send_button_clicked() {
	qDebug() << "send_button_clicked";
	if (m_p2pui->m_textEdit->toPlainText().isEmpty()) {
		QMessageBox::warning(this, tr("Warning"), tr("Content is empty!"));
		return;
	}

}
