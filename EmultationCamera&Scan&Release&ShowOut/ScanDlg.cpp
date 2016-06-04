// ScanDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Emultation.h"
#include "ScanDlg.h"
#include "afxdialogex.h"


// CScanDlg 对话框

IMPLEMENT_DYNAMIC(CScanDlg, CDialogEx)

CScanDlg::CScanDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScanDlg::IDD, pParent)
	, m_scanEndX(0)
	, m_scanEndY(0)
	, m_scanEndZ(0)
	, m_scanStepX(0)
	, m_scanStepY(0)
	, m_scanSendX(0)
	, m_scanSendY(0)
	, m_scanSendZ(0)
	, m_scanAngleY(0)
	, m_scanAngleZ(0)
	, m_scanAngleX(0)
{

	//  m_scanAngleX = 0.0f;
}

CScanDlg::~CScanDlg()
{
}

void CScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SCAN_SENDX, m_scanSendX);
	DDX_Text(pDX, IDC_SCAN_SENDY, m_scanSendY);
	DDX_Text(pDX, IDC_SCAN_SENDZ, m_scanSendZ);
	DDX_Text(pDX, IDC_SCAN_ANGLEX, m_scanAngleX);
	DDX_Text(pDX, IDC_SCAN_ANGLEY, m_scanAngleY);
	DDX_Text(pDX, IDC_SCAN_ANGLEZ, m_scanAngleZ);
	DDX_Text(pDX, IDC_SCAN_ENDX, m_scanEndX);
	DDX_Text(pDX, IDC_SCAN_ENDY, m_scanEndY);
	DDX_Text(pDX, IDC_SCAN_ENDZ, m_scanEndZ);
	DDX_Text(pDX, IDC_SCAN_STEPX, m_scanStepX);
	DDX_Text(pDX, IDC_SCAN_STEPY, m_scanStepY);

}


BEGIN_MESSAGE_MAP(CScanDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CScanDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CScanDlg 消息处理程序



void CScanDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_scanParameter.send.x = m_scanSendX;
	m_scanParameter.send.y = m_scanSendY;
	m_scanParameter.send.z = m_scanSendZ;
	m_scanParameter.angle.x = m_scanAngleX;
	m_scanParameter.angle.y = m_scanAngleY;
	m_scanParameter.angle.z = m_scanAngleZ;
	m_scanParameter.end.x = m_scanEndX;
	m_scanParameter.end.y = m_scanEndY;
	m_scanParameter.end.z = m_scanEndZ;
	m_scanParameter.stepX = m_scanStepX;
	m_scanParameter.stepY = m_scanStepY;

	CDialogEx::OnOK();
}

void CScanDlg::SetScanParameter(my::SCAN_PARAMETER scanParameter)
{
	m_scanParameter = scanParameter;
	m_scanSendX = m_scanParameter.send.x;
	m_scanSendY = m_scanParameter.send.y;
	m_scanSendZ = m_scanParameter.send.z;
	m_scanAngleX = m_scanParameter.angle.x;
	m_scanAngleY = m_scanParameter.angle.y;
	m_scanAngleZ = m_scanParameter.angle.z;
	m_scanEndX = m_scanParameter.end.x;
	m_scanEndY = m_scanParameter.end.y;
	m_scanEndZ = m_scanParameter.end.z;

	m_scanStepX = m_scanParameter.stepX;
	m_scanStepY = m_scanParameter.stepY;
}
