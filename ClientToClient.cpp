#include "ClientToClient.h"
#include"yjchat.h"
#include<qobject.h>
#include<qudpsocket.h>
#include<qtcpsocket.h>
#include<qdatastream.h>
#include<qmessagebox.h>
#include<qmessageauthenticationcode.h>
#include<qtablewidget.h>
#include<qdatetime.h>
#include<qevent.h>

#include"ClientToClient.h"

ClientToClient::ClientToClient(const PersonInformation& personInformationSelf, const PersonInformation& personInformationOppo, Ui::p2pDialog* p2pui , QDialog* p2pDialog, YJChat* parent) :QWidget(parent)
{
	m_personInformationSelf = personInformationSelf;
	m_personInformationOppo = personInformationOppo;
	m_p2pui = p2pui;
	m_p2pDialog = p2pDialog; m_p2pDialog->setWindowFlags(Qt::Dialog|Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);
	m_p2pui->setupUi(m_p2pDialog);

	init_udp();
	init_tcp();
	init_connection();
}


ClientToClient::~ClientToClient()
{

}

void ClientToClient::init_udp() {
	m_udp_socket = new QUdpSocket();
	m_udp_port = 4444;
	m_udp_socket->bind(QHostAddress(m_personInformationSelf.m_ip),m_udp_port);
	//m_udp_socket->bind(m_udp_port);
}

void ClientToClient::init_tcp() {
	m_tcp_socket = new QTcpSocket();
	m_tcp_port = 3333;
	m_tcp_socket->bind(QHostAddress(m_personInformationSelf.m_ip), m_tcp_port);
}

void ClientToClient::init_connection() {
	connect(m_p2pui->m_sendButton, SIGNAL(clicked()), this, SLOT(send_button_clicked()));
	connect(m_udp_socket, SIGNAL(readyRead()), this, SLOT(read_and_process_datagram()));
	connect(m_p2pui->m_closeButton, SIGNAL(clicked()),this, SLOT(close_button_clicked()));

}
//sendMessage
void ClientToClient::send_button_clicked() {
	qDebug() << "send_button_clicked";
	//if empty
	if (m_p2pui->m_textEdit->toPlainText().isEmpty()) {
		QMessageBox::warning(this, tr("Warning"), tr("Content is empty!"));
		return;
	}


	//m_udp_socket
	QByteArray datagram;
	QDataStream outDataStream(&datagram,QIODevice::WriteOnly);
	outDataStream.setVersion(QDataStream::Version::Qt_5_6);
	QString message=m_p2pui->m_textEdit->toPlainText();
	outDataStream <<m_personInformationSelf.m_name 
		<< m_personInformationSelf.m_hostName 
		<< m_personInformationSelf.m_ip 
		<<message;
	m_p2pui->m_textEdit->clear();

	m_udp_socket->writeDatagram(datagram, QHostAddress(m_personInformationOppo.m_ip), m_udp_port);

	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_time = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");

	m_p2pui->m_textBrowser->setTextColor(Qt::darkGreen);
	m_p2pui->m_textBrowser->setCurrentFont(QFont("Times New Roman", 12));
	m_p2pui->m_textBrowser->append("I [ " + m_personInformationSelf.m_name + " ] " + current_time);
	m_p2pui->m_textBrowser->setTextColor(Qt::darkGreen);
	m_p2pui->m_textBrowser->append(message);
}
//receive the message
void ClientToClient::read_and_process_datagram() {
	while (m_udp_socket->hasPendingDatagrams()) {
		QByteArray datagram;
		datagram.resize(m_udp_socket->pendingDatagramSize());
		QDataStream inDataStream(&datagram, QIODevice::ReadOnly);
		inDataStream.setVersion(QDataStream::Version::Qt_5_6);
		
		m_udp_socket->readDatagram(datagram.data(), datagram.size(),&QHostAddress(m_personInformationOppo.m_ip), (quint16*)(&m_udp_port));

		QDateTime current_date_time = QDateTime::currentDateTime();
		QString current_time = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");

		QString userName;
		QString localHostName;
		QString ip;
		QString message;

		inDataStream >> userName >> localHostName >> ip >> message;

		m_p2pui->m_textBrowser->setTextColor(Qt::black);
		m_p2pui->m_textBrowser->setCurrentFont(QFont("Times New Roman", 12));
		m_p2pui->m_textBrowser->append("He [ " + userName + " ] " + current_time);
		m_p2pui->m_textBrowser->setTextColor(Qt::black);
		m_p2pui->m_textBrowser->append(message);
	}
}

void ClientToClient::close_button_clicked() {
	qDebug() << "void ClientToClient::close_button_clicked()";
	m_udp_socket->close();
	m_tcp_socket->close();

	delete m_udp_socket;
	delete m_tcp_socket;
	delete m_p2pui;
	delete m_p2pDialog;
}
