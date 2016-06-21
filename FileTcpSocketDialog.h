#pragma once
#include<QTcpSocket>
#include<qdialog.h>
#include<qfile.h>
#include"ui_FileDialog.h"
#include"PersonInformation.h"

class FileTcpSocketDialog:public QDialog
{
	Q_OBJECT
public:
	FileTcpSocketDialog(PersonInformation personInformationSelf, PersonInformation personInformationOppo, QDialog* parent=nullptr);
	~FileTcpSocketDialog();
private:
	Ui_FileDialog* ui;
	QTcpSocket* m_file_tcp_socket=nullptr;//·¢ËÍ
	quint16 m_file_tcp_port;
	PersonInformation m_personInformationSelf;
	PersonInformation m_personInformationOppo;

	QString m_fileName;
	QFile *m_file=nullptr;// = new QFile(this);
	quint64 m_file_total_bytes = 0;
	quint64 m_file_left_bytes = 0;
	quint64 m_file_bytes_written = 0;
	QByteArray m_outBlock,m_inBlock;
	/**************receive******************/
	QString m_receive_fileName;
	QFile *m_receive_file=nullptr;// = new QFile(this);
	quint64 m_receive_file_total_bytes = 0;
	//quint64 m_receive_file_left_bytes = 0;
	quint64 m_receive_file_bytes_received = 0;
	quint64 m_receive_fileNameSize=0;

private:
	void init_tcp();
	void init_connection();
	public slots:
	void open_file_button_clicked();

	void send_file_button_clicked();

	void update_send_file_process(qint64);

	void update_receive_file_process();

	void close_button_clicked();
	friend class ClientToClient;
};

