
// Emultation.h : Emultation Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������

// CEmultationApp:
// �йش����ʵ�֣������ Emultation.cpp
//

class CEmultationApp : public CWinAppEx
{
public:
	CEmultationApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

	//ģ��
public:

	
private:
};

extern CEmultationApp theApp;
