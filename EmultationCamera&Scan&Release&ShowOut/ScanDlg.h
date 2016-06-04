#pragma once

#include "DataStructs.h"

// CScanDlg �Ի���

class CScanDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScanDlg)

public:
	CScanDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CScanDlg();

// �Ի�������
	enum { IDD = IDD_SCAN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	float m_scanSendX;
	float m_scanSendY;
	float m_scanSendZ;
	float m_scanAngleY;
	float m_scanAngleZ;
	float m_scanAngleX;
	float m_scanEndX;
	float m_scanEndY;
	float m_scanEndZ;
	float m_scanStepX;
	float m_scanStepY;
	afx_msg void OnBnClickedOk();

private:
	my::SCAN_PARAMETER m_scanParameter;
public:
	my::SCAN_PARAMETER GetScanParameter(){ return m_scanParameter; }
	void SetScanParameter(my::SCAN_PARAMETER scanParameter);

};
