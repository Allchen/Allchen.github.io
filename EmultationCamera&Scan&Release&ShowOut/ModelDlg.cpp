// Model.cpp : 实现文件
//

#include "stdafx.h"
#include "Emultation.h"
#include "ModelDlg.h"
#include "afxdialogex.h"

#include "MainFrm.h"
#include "EmultationView.h"
#include "EmultationDoc.h"

// CModel 对话框

IMPLEMENT_DYNAMIC(CModelDlg, CDialogEx)

CModelDlg::CModelDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CModelDlg::IDD, pParent)
	, m_addModelTime(0)
{

}

CModelDlg::~CModelDlg()
{
}

void CModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MODEL_LIST, m_cbModelList);
}


BEGIN_MESSAGE_MAP(CModelDlg, CDialogEx)

	ON_BN_CLICKED(IDC_BTN_OPEN_MODEL, &CModelDlg::OnBnClickedAddModel)
	ON_BN_CLICKED(IDC_BTN_MODEL_PARAMETER, &CModelDlg::OnBnClickedModelParameter)
	ON_BN_CLICKED(IDOK, &CModelDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_DELETE_MODEL, &CModelDlg::OnBnClickedBtnDeleteModel)
	ON_CBN_SELCHANGE(IDC_COMBO_MODEL_LIST, &CModelDlg::OnCbnSelchangeComboModelList)
END_MESSAGE_MAP()


// CModel 消息处理程序


void CModelDlg::OnBnClickedAddModel()
{
	//char szFilter[] = "3DS 文件(*.3DS)|*.3ds||";
	CFileDialog fDlg(TRUE, _T(".3ds"), NULL, OFN_HIDEREADONLY, _T("3DS 文件(*.3DS)|*.3ds||"));

	if (fDlg.DoModal() == IDOK)
	{
		m_fileName = fDlg.GetFileName();
		m_pathName = fDlg.GetPathName();
		C3DSModel model;
		model.Load3DS(fDlg.GetPathName());
		
		CString fileItem(_T("模型"));
		CString tmp;
		tmp.Format(_T("%d:"), m_addModelTime);
		fileItem = fileItem + tmp + fDlg.GetPathName();

		//m_cbIndex2ModelID.push_back(m_addModelTime);
		m_cbIndex2ModelID[m_cbModelList.GetCount()] = m_addModelTime;
		SetCurComBoxID(m_cbModelList.GetCount());

		m_cbModelList.InsertString(m_cbModelList.GetCount(), fileItem);
		m_cbModelList.SetCurSel(m_cbModelList.GetCount() - 1);
		CEmultationView *pView = (CEmultationView *)((CMainFrame *)AfxGetMainWnd())->GetActiveView();
		if (nullptr == pView)
		{
			AfxMessageBox(_T("获取视图类指针失败!"));
			return;
		}
		pView->AddModel(m_addModelTime, model);
		pView->SetCurModelID(m_cbIndex2ModelID[m_addModelTime]);

		++m_addModelTime;

	}
}


void CModelDlg::OnBnClickedModelParameter()
{
	// TODO:  在此添加控件通知处理程序代码

	CEmultationView *pView = (CEmultationView *)((CMainFrame *)AfxGetMainWnd())->GetActiveView();
	if (nullptr == pView)
	{
		AfxMessageBox(_T("获取视图类指针失败!"));
		return;
	}

	if (pView->GetModelMap().empty())
	{
		AfxMessageBox(_T("没有加载模型!"));
		return;
	}

	int modelID = m_cbIndex2ModelID[GetCurComBoxID()];
	ModelParameter modelParaDlg;
	modelParaDlg.SetTransParameter(pView->GetModelMap()[modelID].GetTransPara());
	if (modelParaDlg.DoModal() == IDOK)
	{
		pView->GetModelMap()[modelID].SetTransPara(modelParaDlg.GetTransParameter());
	}
}


void CModelDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CModelDlg::OnBnClickedBtnDeleteModel()
{
	// TODO:  在此添加控件通知处理程序代码
	CEmultationView *pView = (CEmultationView *)((CMainFrame *)AfxGetMainWnd())->GetActiveView();
	if (nullptr == pView)
	{
		AfxMessageBox(_T("获取视图类指针失败!"));
		return;
	}
	
	if (!pView->IsModelExisted(m_cbIndex2ModelID[GetCurComBoxID()]))
	{
		AfxMessageBox(_T("模型不存在!"));
		return;
	}
	pView->DeleteModel(m_cbIndex2ModelID[GetCurComBoxID()]);
	UpdataCBIndex2ModelID(GetCurComBoxID());
	m_cbModelList.DeleteString(GetCurComBoxID());
	m_cbModelList.SetCurSel(CB_ERR);

	SetCurComBoxID(CB_ERR);
}


void CModelDlg::OnCbnSelchangeComboModelList()
{
	// TODO:  在此添加控件通知处理程序代码
	SetCurComBoxID(m_cbModelList.GetCurSel());
	CEmultationView *pView = (CEmultationView *)((CMainFrame *)AfxGetMainWnd())->GetActiveView();
	if (nullptr == pView)
	{
		AfxMessageBox(_T("获取视图类指针失败!"));
		return;
	}
	pView->SetCurModelID(m_cbIndex2ModelID[m_cbModelList.GetCurSel()]);
}

void CModelDlg::UpdataCBIndex2ModelID(int index)
{
	if (index < 0 || index >= m_cbModelList.GetCount())
	{
		return;
	}
	std::unordered_map<int, int> tmp;
	for (auto iter = m_cbIndex2ModelID.begin(); iter != std::prev(m_cbIndex2ModelID.end());++iter)
	{
		if (iter->first < index)
		{
			tmp[iter->first] = iter->second;
		}
		else
		{
			tmp[iter->first] = (std::next(iter))->second;
		}
	}

	m_cbIndex2ModelID.swap(tmp);
}
