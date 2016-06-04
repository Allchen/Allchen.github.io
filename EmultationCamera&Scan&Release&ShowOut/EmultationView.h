
// EmultationView.h : CEmultationView ��Ľӿ�
//

#pragma once

#include "CTargaImage.h"
#include "HDL64E_Simulator.h"
#include "C3DSModel.h"
#include "EmultationDoc.h"
#include "ModelDlg.h"
#include <unordered_map>
#include "ScanDlg.h"
#include "CameraDlg.h"
#include "3DFont.h"


enum TIME_EVENT { EVENT_SCANLINE,EVENT_CAMERA };


class CEmultationView : public CView
{
protected: // �������л�����
	CEmultationView();
	DECLARE_DYNCREATE(CEmultationView)

// ����
public:
	CEmultationDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CEmultationView();
//#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
//#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

	//OpenGL
public:
	HGLRC m_hRC;          //Rendering Context��ɫ������
	CClientDC* m_pDC;     //Device Context�豸������
protected:
	BOOL SetupPixelFormat();
	BOOL InitOpenGL();
	void DrawScene();
public:
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	std::vector<C3DSModel> m_modelVec;
	//�������
public:
	void AddModel(int id, const C3DSModel &model);
	bool IsModelExisted(int id){ return m_modelMap.end() != m_modelMap.find(id); }
	void DeleteModel(int id){ m_modelMap.erase(id); }
	void SetCurModelID(int id){ m_curModelID = id; }
	int GetCurModelID(){ return m_curModelID; }
	void SavePointCloud(const std::string &fileName, std::vector<my::SPACEPOINT> m_pointData, std::vector<my::POINT_TYPE> m_pointDataType);
	void SaveRealObjPointCloud(const std::string &fileName, std::vector<my::SPACEPOINT> m_pointData, std::vector<my::POINT_TYPE> m_pointDataType);
	void SaveCameraData(const std::string &fileName, std::vector<my::SPACEPOINT> m_pointData, std::vector<my::POINT_TYPE> m_pointDataType);
	void SaveRealObjCameraData(const std::string &fileName, std::vector<my::SPACEPOINT> m_pointData, std::vector<my::POINT_TYPE> m_pointDataType);
private:
	void DrawAxis(int length);											//��ʾ������
	void DrawGround(int length, int width);								//��ʾ����
	void DrawModels();													//��ʾģ��
	void DrawPointCloud(std::vector<my::SPACEPOINT> m_pointData, std::vector<my::POINT_TYPE> m_pointDataType);													//��ʾģ��
	void DrawCameraPhoto(std::vector<my::SPACEPOINT> m_pointData, std::vector<my::POINT_TYPE> m_pointDataType);
	void DrawScanOut();
	void DrawCameraOut();
	void DrawScanLine();												//��ʾɨ����
	void DrawCameraLine();
	void DrawLight();
	bool RayHitModels(my::Ray scanRay, my::SPACEPOINT &scanPoint,int &modelID);
	bool RayHitModel(my::Ray scanRay, my::SPACEPOINT &scanPoint,int modelID);
	bool RayHitSphere(my::Ray scanRay, my::SPACEPOINT &scanPoint);
	void ScanEvent(my::SPACEPOINT &scanPoint, my::Ray &scanRay);
	void CameraEvent(my::SPACEPOINT &cameraPoint,my::Ray &cameraRay);
	my::Vector3  RoatMartixZ(float a, float b, float c, my::Vector3 RadarP, my::Vector3 TransP);
	my::Vector3  RoatMartixY(float a, float b, float c, my::Vector3 RadarP, my::Vector3 TransP);
	my::Vector3  RoatMartixX(float a, float b, float c, my::Vector3 RadarP, my::Vector3 TransP);
	void Move2Center(int id);
	void FindandGet(CString LineStr,float *point);
	unsigned int m_textureGroundObj;
	CTargaImage *m_textureGround;
	my::Vector3 m_ground;												//x��ʾ���泤��,y��,z���
	int m_mouseWheel;
	float m_angleX;
	float m_angleY;
	my::Vector3 m_eye;
	float m_scale;
	float m_scaleUnit;
	bool m_leftBtnDown;
	bool m_showScanLine;
	bool m_showCameraLine;
	bool m_invert;
	bool m_camera_invert;
	int m_curModelID;
	double CurVertAngle;
	double CurHoriAngle;
	double CameraCurVAngle;
	double CameraCurHAngle;
	std::vector<my::MakeOUT>m_cameraPhoto;
	std::vector<my::MakeOUT>m_pointVector;
	std::vector<my::SPACEPOINT> m_pointCloud;
	std::vector<my::POINT_TYPE> m_pointCloudType;
	std::vector<my::SPACEPOINT> m_cameraData;
	std::vector<my::POINT_TYPE> m_cameraDataType;
	CPoint m_oldPoint;
	CModelDlg m_modelDlg;
	GLFont font;
	std::unordered_map<int, C3DSModel> m_modelMap;
	std::unordered_map<int, my::POINT_TYPE> m_modelID2PointType;
	my::SCAN_PARAMETER m_scanParameter;
	my::CAMERA_PARAMETER m_cameraParamameter;

public:
	std::unordered_map<int, C3DSModel>& GetModelMap(){ return m_modelMap; }
public:
	afx_msg void OnModel();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMenuScan();
	afx_msg void OnMenuCamera();
	afx_msg void OnMenuScanout();
	afx_msg void OnMenuCameraout();
	afx_msg void OnMenuFlush();
};

#ifndef _DEBUG  // EmultationView.cpp �еĵ��԰汾
//inline CEmultationDoc* CEmultationView::GetDocument() const
//   { return reinterpret_cast<CEmultationDoc*>(m_pDocument); }
#endif

