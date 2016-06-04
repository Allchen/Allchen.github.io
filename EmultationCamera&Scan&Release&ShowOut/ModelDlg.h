#pragma once

#include <string>
#include <unordered_map>
#include "ModelParameter.h"
#include "DataStructs.h"
#include "C3DSModel.h"
#include "afxwin.h"
// CModel 对话框

class CModelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CModelDlg)

public:
	CModelDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModelDlg();

// 对话框数据
	enum { IDD = IDD_MODEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddModel();

private:
	int m_addModelTime;
	int m_curModelID;
	CString m_fileName;
	CString m_pathName;
	C3DSModel m_model;
	std::unordered_map<int, int> m_cbIndex2ModelID;
	//std::vector<int> m_cbIndex2ModelID;
	void UpdataCBIndex2ModelID(int index);

public:
	int GetAddModelTimes(){ return m_addModelTime; }
	int GetCurComBoxID(){ return m_curModelID; }
	void SetCurComBoxID(int i){ m_curModelID = i; }
	CString get3DSFileName(){ return m_fileName; }
	CString get3DSPathName(){ return m_pathName; }
	C3DSModel getMoldel(){ return m_model; }
	CComboBox m_cbModelList;
	afx_msg void OnBnClickedModelParameter();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnDeleteModel();
	afx_msg void OnCbnSelchangeComboModelList();
};