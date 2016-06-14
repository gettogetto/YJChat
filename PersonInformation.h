#pragma once
#include<QString>
class PersonInformation
{
public:
	PersonInformation(QString name="", QString hostName="",QString ip="");
	~PersonInformation();
	QString m_name;
	QString m_hostName;
	QString m_ip;
};

