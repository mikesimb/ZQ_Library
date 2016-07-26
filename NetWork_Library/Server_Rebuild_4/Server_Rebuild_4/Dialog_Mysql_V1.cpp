// Dialog_Mysql_V1.cpp : implementation file
//

#include "stdafx.h"
#include "Server_Rebuild_4.h"
#include "Dialog_Mysql_V1.h"
#include "afxdialogex.h"



// CDialog_Mysql_V1 dialog

IMPLEMENT_DYNAMIC(CDialog_Mysql_V1, CDialogEx)

CDialog_Mysql_V1::CDialog_Mysql_V1(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG3, pParent)
	, m_IPAddress(_T(""))
	, m_Port(0)
	, m_DBName(_T(""))
	, m_Username(_T(""))
	, m_password(_T(""))
{
	m_IPAddress = G_Mysql_IPaddress;
	m_Port = G_Mysql_port;
	m_Username = G_Mysql_UserName;
	m_password = G_Mysql_Password;
	mysql = mysql_init(mysql);
	m_DBName = "mirdemos";

}

CDialog_Mysql_V1::~CDialog_Mysql_V1()
{
}

void CDialog_Mysql_V1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_IPAddress);
	DDX_Text(pDX, IDC_EDIT4, m_Port);
	DDX_Text(pDX, IDC_EDIT6, m_DBName);
	DDX_Text(pDX, IDC_EDIT5, m_Username);
	DDX_Text(pDX, IDC_EDIT3, m_password);
	DDX_Control(pDX, IDC_LIST2, m_output_info);
}


BEGIN_MESSAGE_MAP(CDialog_Mysql_V1, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDialog_Mysql_V1::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CDialog_Mysql_V1::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDialog_Mysql_V1::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDialog_Mysql_V1::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CDialog_Mysql_V1::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CDialog_Mysql_V1::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CDialog_Mysql_V1::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CDialog_Mysql_V1::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CDialog_Mysql_V1::OnBnClickedButton10)
END_MESSAGE_MAP()


// CDialog_Mysql_V1 message handlers
void CDialog_Mysql_V1::outputinfo(char info[])
{
	m_output_info.InsertString(m_output_info.GetCount(), info);
}

void CDialog_Mysql_V1::getmysqlErrorinfo()
{
	CString errorcode = "ErrorCode:%s\n";
	errorcode.Format(mysql_error(mysql));
	outputinfo(errorcode.GetBuffer());
}

void CDialog_Mysql_V1::selectDatabase()
{
	char sql[] = "use mirdemo;";
	int result = mysql_query(mysql, sql);
	if (result == 0)
	{
		outputinfo("select database success");

	}
	else
		outputinfo("select database failed");

}

void CDialog_Mysql_V1::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	MYSQL * Handle = mysql_real_connect(mysql, m_IPAddress, m_Username, m_password, NULL, m_Port, 0, 0);
	if (Handle)
	{
		outputinfo("�������ݿ�ɹ�");
        bool isExists = mysql_select_db(mysql, m_DBName);
			if (isExists)
			{
				//����������ݿ�����
				outputinfo("���ݿ����");

			}
			else
			{
				outputinfo("���ݿⲻ����");
			}

	}
	else
	{
		
		outputinfo("�������ݿ�ʧ��");
		getmysqlErrorinfo();
	}
	

	//������������Ҫ���DB�Ƿ����
	
}


void CDialog_Mysql_V1::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	// �������ݿ�
	int result = mysql_query(mysql, "CREATE DATABASE mirdemo");
	if (result == 0)
	{
		outputinfo("�������ݿ�ɹ�");

	}
	else 
		outputinfo("�������ݿ�ʧ��");
}


void CDialog_Mysql_V1::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	int result = mysql_query(mysql, "DROP DATABASE mirdemo");
	if (result == 0)
	{
		outputinfo("ɾ�����ݿ�ɹ�");

	}
	else
		outputinfo("ɾ�����ݿ�ʧ��");
}


void CDialog_Mysql_V1::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	//�����������������˺þã����������һ��Ҫ��ִ����� use mirdemo
	//������ִ��ɾ���ʹ��������ͺ���
	//����ִ�к�mysql_error���ص���ʾ��not database selected 
	//�мǣ��мǣ��мǣ� ��Ҫ������˵����
	selectDatabase();
	char sql[] = "DROP TABLE IF EXISTS user";
	char sql1[] = "CREATE TABLE `user`(\
					`idx` bigint(20) NOT NULL AUTO_INCREMENT,\
					`username` varchar(20) DEFAULT NULL,\
					`password` varchar(20) DEFAULT NULL,\
					`RegisterDate` date NOT NULL,\
					PRIMARY KEY(`idx`)) ENGINE = InnoDB DEFAULT CHARSET = latin1; ";
	int result = mysql_query(mysql, sql);
	result = mysql_query(mysql, sql1);
	if (result == 0)
	{
		
		outputinfo("����USER��ɹ�");

	}
	else
	{
		getmysqlErrorinfo();
		outputinfo("����USER��ʧ��");
	}
		
}


void CDialog_Mysql_V1::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	selectDatabase();
	char sql1[] = "INSERT INTO user(idx,username,password,registerdate) VALUES(0,'����','1234','1998-01-05')"; 
	char sql3[] = "INSERT INTO user(idx,username,password,registerdate) VALUES(0,'����','2345','1998-01-05')";
	int result =  mysql_query(mysql, sql1);
	if (result == 0)
	{

		outputinfo("�������ݳɹ�");

	}
	else
	{
		getmysqlErrorinfo();
		outputinfo("��������ʧ��");
	}
	result = mysql_query(mysql, sql3);
	if (result == 0)
	{

		outputinfo("�������ݳɹ�");


	}
	else
	{
		getmysqlErrorinfo();
		outputinfo("��������ʧ��");
	}
}


void CDialog_Mysql_V1::OnBnClickedButton7()
{
	// TODO: Add your control notification handler code here
	selectDatabase();
	char sql1[] = "select * from user";
	int result = mysql_query(mysql, sql1);
	if (result == 0)
	{
		MYSQL_RES *res = mysql_store_result(mysql);
		MYSQL_ROW row;
		unsigned int num_fields;
		unsigned int i;

		num_fields = mysql_num_fields(res);
		while ((row = mysql_fetch_row(res)))
		{
			unsigned long *lengths;
			lengths = mysql_fetch_lengths(res);
			for (i = 0; i < num_fields; i++)
			{
// 				printf("[%.*s] ", (int)lengths[i],
// 					row[i] ? row[i] : "NULL");
				outputinfo(row[i]);
			}
		}


	}
	else
	{
		getmysqlErrorinfo();
		outputinfo("����USER��ʧ��");
	}
}


void CDialog_Mysql_V1::OnBnClickedButton8()
{
	// TODO: Add your control notification handler code here
	selectDatabase();
	//char sql[] = "delete from user where 1";//�����䲻�Ὣ���������ݱ�Ϊ1
	char sql[] = "TRUNCATE user";//������Ὣ���������ݱ�Ϊ1
	int result = mysql_query(mysql, sql);
	if (result == 0)
	{
		outputinfo("ɾ��user�����ݳɹ�");
	}
	else
	{
		getmysqlErrorinfo();
		outputinfo("ɾ��USER������ʧ��");
	}
}

//ɾ����������
void CDialog_Mysql_V1::OnBnClickedButton9()
{
	// TODO: Add your control notification handler code here
	selectDatabase();
	//char sql[] = "delete from user where 1";//�����䲻�Ὣ���������ݱ�Ϊ1
	char sql[] = "delete from user where username = '����';";//������Ὣ���������ݱ�Ϊ1
	int result = mysql_query(mysql, sql);
	if (result == 0)
	{
		outputinfo("ɾ��user�����ݳɹ�");
	}
	else
	{
		getmysqlErrorinfo();
		outputinfo("ɾ��USER������ʧ��");
	}
}


void CDialog_Mysql_V1::OnBnClickedButton10()
{
	// TODO: Add your control notification handler code hereselectDatabase();
	//char sql[] = "delete from user where 1";//�����䲻�Ὣ���������ݱ�Ϊ1
	char sql[] = "update user set username =  '��������' where idx = 1;";//������Ὣ���������ݱ�Ϊ1
	int result = mysql_query(mysql, sql);
	if (result == 0)
	{
		outputinfo("ɾ��user�����ݳɹ�");
	}
	else
	{
		getmysqlErrorinfo();
		outputinfo("ɾ��USER������ʧ��");
	}
}
