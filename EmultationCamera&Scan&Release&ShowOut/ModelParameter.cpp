// ModelParameter.cpp : 实现文件
//

#include "stdafx.h"
#include "Emultation.h"
#include "ModelParameter.h"
#include "afxdialogex.h"


// ModelParameter 对话框

IMPLEMENT_DYNAMIC(ModelParameter, CDialogEx)

ModelParameter::ModelParameter(CWnd* pParent /*=NULL*/)
	: CDialogEx(ModelParameter::IDD, pParent)
	, m_posX(0)
	, m_posY(0)
	, m_posZ(0)
	, m_rotateX(0)
	, m_rotateY(0)
	, m_rotateZ(0)
	, m_scaleX(1)
	, m_scaleY(1)
	, m_scaleZ(1)
{

}

ModelParameter::~ModelParameter()
{
}

void ModelParameter::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_POSX, m_posX);
	DDX_Text(pDX, IDC_EDIT_POSY, m_posY);
	DDX_Text(pDX, IDC_EDIT_POSZ, m_posZ);
	DDX_Text(pDX, IDC_EDIT_ROTATEX, m_rotateX);
	DDX_Text(pDX, IDC_EDIT_ROTATEY, m_rotateY);
	DDX_Text(pDX, IDC_EDIT_ROTATEZ, m_rotateZ);
	DDX_Text(pDX, IDC_EDIT_SCALEX, m_scaleX);
	DDX_Text(pDX, IDC_EDIT_SCALEY, m_scaleY);
	DDX_Text(pDX, IDC_EDIT_SCALEZ, m_scaleZ);
}


BEGIN_MESSAGE_MAP(ModelParameter, CDialogEx)
	ON_BN_CLICKED(IDOK, &ModelParameter::OnBnClickedOk)
END_MESSAGE_MAP()


// ModelParameter 消息处理程序


void ModelParameter::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);

	CDialogEx::OnOK();
}

my::T3ds ModelParameter::GetTransParameter()
{
	my::T3ds parameter;
	parameter.x = m_posX;
	parameter.y = m_posY;
	parameter.z = m_posZ;
	parameter.d_x = m_rotateX;
	parameter.d_y = m_rotateY;
	parameter.d_z = m_rotateZ;
	parameter.scaleX = m_scaleX;
	parameter.scaleY = m_scaleY;
	parameter.scaleZ = m_scaleZ;

	return parameter;
}

void ModelParameter::SetTransParameter(my::T3ds parameter)
{
	SetPosX(parameter.x);
	SetPosY(parameter.y);
	SetPosZ(parameter.z);
	SetRotateX(parameter.d_x);
	SetRotateY(parameter.d_y);
	SetRotateZ(parameter.d_z);
	SetScaleX(parameter.scaleX);
	SetScaleY(parameter.scaleY);
	SetScaleZ(parameter.scaleZ);
}
