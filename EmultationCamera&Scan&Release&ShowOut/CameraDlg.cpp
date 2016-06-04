// CameraDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Emultation.h"
#include "CameraDlg.h"
#include "afxdialogex.h"


// CCameraDlg 对话框

IMPLEMENT_DYNAMIC(CCameraDlg, CDialogEx)

CCameraDlg::CCameraDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CAMERA, pParent)
	, m_camera_angleX(0)
	, m_camera_angleY(0)
	, m_camera_angleZ(0)
	, m_camera_startPX(0)
	, m_camera_startPY(0)
	, m_camera_startPZ(0)
	, m_camera_lightX(0)
	, m_camera_lightY(0)
	, m_camera_lightZ(0)
{

}

CCameraDlg::~CCameraDlg()
{
}

void CCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CAMERA_ANGLEX, m_camera_angleX);
	DDX_Text(pDX, IDC_CAMERA_ANGLEY, m_camera_angleY);
	DDX_Text(pDX, IDC_CAMERA_ANGLEZ, m_camera_angleZ);
	DDX_Text(pDX, IDC_CAMERA_STARTPX, m_camera_startPX);
	DDX_Text(pDX, IDC_CAMERA_STARTPY, m_camera_startPY);
	DDX_Text(pDX, IDC_CAMERA_STARTPZ, m_camera_startPZ);
	DDX_Text(pDX, IDC_CAMERA_LightPX, m_camera_lightX);
	DDX_Text(pDX, IDC_CAMERA_LightPY, m_camera_lightY);
	DDX_Text(pDX, IDC_CAMERA_LightPZ, m_camera_lightZ);
}


BEGIN_MESSAGE_MAP(CCameraDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCameraDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCameraDlg 消息处理程序


void CCameraDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_cameraParameter.startP.x = m_camera_startPX;
	m_cameraParameter.startP.y = m_camera_startPY;
	m_cameraParameter.startP.z = m_camera_startPZ;
	m_cameraParameter.angle.x = m_camera_angleX;
	m_cameraParameter.angle.y = m_camera_angleY;
	m_cameraParameter.angle.z = m_camera_angleZ;
	m_cameraParameter.lightP.x = m_camera_lightX;
	m_cameraParameter.lightP.y = m_camera_lightY;
	m_cameraParameter.lightP.z = m_camera_lightZ;
	
	CDialogEx::OnOK();
}

void CCameraDlg::SetCameraParameter(my::CAMERA_PARAMETER cameraParameter)
{
	m_cameraParameter = cameraParameter;
	m_camera_startPX = m_cameraParameter.startP.x;
	m_camera_startPY = m_cameraParameter.startP.y;
	m_camera_startPZ = m_cameraParameter.startP.z;
	m_camera_angleX = m_cameraParameter.angle.x;
	m_camera_angleY = m_cameraParameter.angle.y;
	m_camera_angleZ = m_cameraParameter.angle.z;
	m_camera_lightX = m_cameraParameter.lightP.x;
	m_camera_lightY = m_cameraParameter.lightP.y;
	m_camera_lightZ = m_cameraParameter.lightP.z;

}


