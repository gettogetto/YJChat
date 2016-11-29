
#include "FileTcpSocketDialog.h"
#include<qhostaddress.h>
#include<qfiledialog.h>
#include<qmessagebox.h>

FileTcpSocketDialog::FileTcpSocketDialog(PersonInformation personInformationSelf, PersonInformation personInformationOppo,QDialog* parent):
	m_personInformationSelf(personInformationSelf),
	m_personInformationOppo(personInformationOppo),
	QDialog(parent),
	ui(new Ui_FileDialog)
{
	ui->setupUi(this);
	this->setWindowFlags(Qt::Dialog|Qt::WindowTitleHint);
	init_tcp();
	init_connection();
	ui->m_fileSendProgressBar->reset();
}


FileTcpSocketDialog::~FileTcpSocketDialog()
{

}
void FileTcpSocketDialog::init_tcp() {
	m_file_tcp_port = 3333;
	m_file_tcp_socket = new QTcpSocket(this);
	m_file_tcp_socket->bind(QHostAddress(m_personInformationSelf.m_ip), m_file_tcp_port);
	
	m_file_tcp_socket->connectToHost(QHostAddress(m_personInformationOppo.m_ip), m_file_tcp_port);

}

void FileTcpSocketDialog::init_connection() {
	connect(ui->m_openFileButton, SIGNAL(clicked()), this, SLOT(open_file_button_clicked()));
	connect(ui->m_sendButton, SIGNAL(clicked()), this, SLOT(send_file_button_clicked()));
	connect(ui->m_closeButton, SIGNAL(clicked()), this, SLOT(close_button_clicked()));
	connect(m_file_tcp_socket, SIGNAL(bytesWritten(qint64)), this, SLOT(update_send_file_process(qint64)));
	connect(m_file_tcp_socket, SIGNAL(readyRead()), this, SLOT(update_receive_file_process()));

}

void FileTcpSocketDialog::open_file_button_clicked() {
	
	m_fileName=QFileDialog::getOpenFileName(this);
	if (!m_fileName.isEmpty()) {
		ui->m_sendButton->setEnabled(true);
		qDebug() << "open file success";
		return;
	}
	qDebug() << "open file failed";
}

void FileTcpSocketDialog::send_file_button_clicked() {
	qDebug() << "void FileTcpSocketDialog::send_file_button_clicked()";
	if (!m_file_tcp_socket->isOpen()) {
		if (!m_file_tcp_socket->open(QIODevice::WriteOnly)) {
			qDebug() << "socket open failed";
			return;
		}
	}


	//m_file->setFileName(m_fileName);
	//ui->m_fileSendProgressBar->resetFormat();

	m_file = new QFile(m_fileName);
	if (!m_file->open(QIODevice::ReadOnly)) {
		QMessageBox::warning(this, tr("send file"), tr("can not read the file %1:\n%2.").arg(m_fileName).arg(m_file->errorString()));
		return;
	}
	ui->m_sendButton->setEnabled(false);
	m_file_left_bytes = m_file_total_bytes = m_file->size();

	QDataStream outStream(&m_outBlock, QIODevice::WriteOnly);

	outStream.setVersion(QDataStream::Version::Qt_5_6);

	QString onlyFileName = m_fileName.right(m_fileName.size() - m_fileName.lastIndexOf('/') - 1);

	outStream << quint64(0) << quint64(0) << onlyFileName;//totalsize filenamesize filename

	m_file_total_bytes += m_outBlock.size();
	outStream.device()->seek(0);

	outStream << m_file_total_bytes << (m_outBlock.size() - 2 * sizeof(quint64));

	m_file_left_bytes = m_file_total_bytes - m_file_tcp_socket->write(m_outBlock);
	m_outBlock.resize(0);
	qDebug() << "void FileTcpSocketDialog::send_file_button_clicked() left";
}

void FileTcpSocketDialog::update_send_file_process(qint64 bytesWritten) {

	qDebug() << "void FileTcpSocketDialog::update_send_file_process";
	m_file_bytes_written += bytesWritten;

	if (m_file_left_bytes > 0) {
		m_outBlock = m_file->read(qMin((int)m_file_left_bytes,4096));
		m_file_left_bytes -= m_file_tcp_socket->write(m_outBlock);
		m_outBlock.resize(0);
	}
	else {
		m_file->close();
	}
	ui->m_fileSendProgressBar->setMaximum(m_file_total_bytes);
	ui->m_fileSendProgressBar->setValue(int(m_file_bytes_written));
	
	qDebug() << m_file_bytes_written << "sended" << m_file_total_bytes;
	if (m_file_bytes_written == m_file_total_bytes) {
		m_file->close();
		m_file_left_bytes = 0;
		m_file_total_bytes = 0;
		m_file_bytes_written = 0;
	}
}

void FileTcpSocketDialog::update_receive_file_process() {
	qDebug() << "void FileTcpSocketDialog::update_receive_file_process()";
	QDataStream inStream(m_file_tcp_socket);
	inStream.setVersion(QDataStream::Qt_5_6);

	if (m_receive_file_bytes_received <= sizeof(quint64) * 2) {
		if ((m_file_tcp_socket->bytesAvailable() >= sizeof(quint64) * 2) && (m_receive_fileNameSize == 0)) {
			inStream >> m_receive_file_total_bytes >> m_receive_fileNameSize;
			m_receive_file_bytes_received += sizeof(quint64) * 2;
		}
		if ((m_file_tcp_socket->bytesAvailable() >= m_receive_fileNameSize) && (m_receive_fileNameSize != 0)) {
			inStream >> m_receive_fileName;
			m_receive_file_bytes_received += m_receive_fileNameSize;
			//m_receive_file->setFileName(m_receive_fileName);//!!
			m_receive_file = new QFile(m_receive_fileName);
			if (!m_receive_file->open(QFile::WriteOnly)) {
				QMessageBox::warning(this, tr("receive file"), tr("can not read the file %1:\n%2.").arg(m_receive_fileName).arg(m_receive_file->errorString()));
				return;
			}
		}
		else {
			return;
		}
	}

	if (m_receive_file_bytes_received < m_receive_file_total_bytes) {
		m_receive_file_bytes_received += m_file_tcp_socket->bytesAvailable();
		m_inBlock = m_file_tcp_socket->readAll();
		m_receive_file->write(m_inBlock);
		m_inBlock.resize(0);
	}

	qDebug() << m_receive_file_bytes_received << "received" << m_receive_file_total_bytes;



	if (m_receive_file_bytes_received == m_receive_file_total_bytes)
	{
		
		 m_receive_file->close();  //接收完文件后，关闭
		 m_receive_file_total_bytes = 0;
		 //m_receive_file_left_bytes = 0;
		 m_receive_file_bytes_received = 0;
		 m_receive_fileNameSize = 0;
	}
}

void FileTcpSocketDialog::close_button_clicked() {
	if(m_file&&m_file->isOpen())
		m_file->close();
	if (m_receive_file&&m_receive_file->isOpen())
		m_receive_file->close();
	
	//m_file_tcp_socket->disconnectFromHost();
	//delete m_file_tcp_socket; 
	//m_file_tcp_socket = nullptr;

	this->close();
	
	qDebug() << "void FileTcpSocketDialog::close_button_clicked() left";
	
	//delete m_file;
	//delete m_receive_file;
}