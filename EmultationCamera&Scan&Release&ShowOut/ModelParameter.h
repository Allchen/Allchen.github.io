#pragma once

#include "Resource.h"
#include "DataStructs.h"


// ModelParameter 对话框

class ModelParameter : public CDialogEx
{
	DECLARE_DYNAMIC(ModelParameter)

public:
	ModelParameter(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ModelParameter();

// 对话框数据
	enum { IDD = IDD_MODEL_PARAMETER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	my::T3ds GetTransParameter();
	void SetTransParameter(my::T3ds parameter);
	void SetPosX(float x){ m_posX = x; }
	void SetPosY(float y){ m_posY = y; }
	void SetPosZ(float z){ m_posZ = z; }
	void SetRotateX(float x){ m_rotateX = x; }
	void SetRotateY(float y){ m_rotateY = y; }
	void SetRotateZ(float z){ m_rotateZ = z; }
	void SetScaleX(float x){ m_scaleX = x; }
	void SetScaleY(float y){ m_scaleY = y; }
	void SetScaleZ(float z){ m_scaleZ = z; }
	float GetPosX(){ return m_posX; }
	float GetPosY(){ return m_posY; }
	float GetPosZ(){ return m_posZ; }
	float GetRotateX(){ return m_rotateX; }
	float GetRotateY(){ return m_rotateY; }
	float GetRotateZ(){ return m_rotateZ; }
	float GetScaleX(){ return m_scaleX; }
	float GetScaleY(){ return m_scaleY; }
	float GetScaleZ(){ return m_scaleZ; }

private:
	float m_posX;
	float m_posY;
	float m_posZ;
	float m_rotateX;
	float m_rotateY;
	float m_rotateZ;
	float m_scaleX;
	float m_scaleY;
	float m_scaleZ;
};
