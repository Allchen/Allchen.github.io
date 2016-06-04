#pragma once

#include "DataStructs.h"
// CCameraDlg 对话框

class CCameraDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCameraDlg)

public:
	CCameraDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CCameraDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	my::CAMERA_PARAMETER m_cameraParameter;
public:
	float m_camera_angleX;
	float m_camera_angleY;
	float m_camera_angleZ;
	float m_camera_startPX;
	float m_camera_startPY;
	float m_camera_startPZ;
	my::CAMERA_PARAMETER GetCameraParameter() { return m_cameraParameter; }
	void SetCameraParameter(my::CAMERA_PARAMETER cameraParameter);
	afx_msg void OnBnClickedOk();
	float m_camera_lightX;
	float m_camera_lightY;
	float m_camera_lightZ;
};
