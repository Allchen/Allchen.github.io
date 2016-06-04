
// EmultationView.cpp : CEmultationView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Emultation.h"
#endif

#include "EmultationDoc.h"
#include "EmultationView.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEmultationView

IMPLEMENT_DYNCREATE(CEmultationView, CView)

BEGIN_MESSAGE_MAP(CEmultationView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_MENU_MODEL, &CEmultationView::OnModel)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	ON_WM_TIMER()
	ON_COMMAND(ID_MENU_SCAN, &CEmultationView::OnMenuScan)
	ON_COMMAND(ID_MENU_CAMERA, &CEmultationView::OnMenuCamera)
	ON_COMMAND(ID_MENU_SCANOUT, &CEmultationView::OnMenuScanout)
	ON_COMMAND(ID_MENU_CAMERAOUT, &CEmultationView::OnMenuCameraout)
	ON_COMMAND(ID_MENU_FLUSH, &CEmultationView::OnMenuFlush)
END_MESSAGE_MAP()

// CEmultationView 构造/析构

CEmultationView::CEmultationView() :
m_ground(20, 0, 20),
m_mouseWheel(0),
m_angleX(0.0),
m_angleY(0.0),
m_eye(m_ground.x, m_ground.y + m_ground.x * 2, m_ground.z),
m_scale(1.0),
m_scaleUnit(2.0),
m_leftBtnDown(false),
m_showScanLine(false),
m_showCameraLine(false),
m_curModelID(-1),
m_oldPoint(CPoint()),
m_scanParameter(300,300,0 ,0,90,-45, 5, 0, 5, 0.2, 0.2),
//m_cameraParamameter(0, 10, 10, -135, 0, 0,10,10,10)
m_cameraParamameter(300, 300, 0, 0, 90, -45, 300, 300, 300)
{
	// TODO:  在此处添加构造代码
	CurHoriAngle = 30;
	CurVertAngle = -15 ;
	CameraCurHAngle = 30;
	CameraCurVAngle = -15;
	m_invert = true;
	m_camera_invert = true;
	//int size = m_ground.x * m_ground.z / min(m_scanParameter.stepX, m_scanParameter.stepY);
	int size = 512 * 512;
	m_pointVector.reserve(size);
	m_cameraPhoto.reserve(size);
	m_pointCloud.reserve(size);
	m_pointCloudType.reserve(size);
	int csize = 5000 * 5000;
	m_cameraData.reserve(csize);
	m_cameraDataType.reserve(csize);
}

CEmultationView::~CEmultationView()
{
}

BOOL CEmultationView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	// 在客户区绘制
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	return CView::PreCreateWindow(cs);
}

// CEmultationView 绘制

void CEmultationView::OnDraw(CDC* /*pDC*/)
{
	CEmultationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
	DrawScene();

}


// CEmultationView 打印

BOOL CEmultationView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CEmultationView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加额外的打印前进行的初始化过程
}

void CEmultationView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:  添加打印后进行的清理过程
}


// CEmultationView 诊断

//#ifdef _DEBUG
void CEmultationView::AssertValid() const
{
	CView::AssertValid();
}

void CEmultationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEmultationDoc* CEmultationView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEmultationDoc)));
	return (CEmultationDoc*)m_pDocument;
}

BOOL CEmultationView::SetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR), // size of this pfd
		1,                             // version number
		PFD_DRAW_TO_WINDOW |           // support window
		PFD_SUPPORT_OPENGL |           // support OpenGL
		PFD_DOUBLEBUFFER,              // double buffered
		PFD_TYPE_RGBA,                 // RGBA type
		24,                            // 24-bit color depth
		0, 0, 0, 0, 0, 0,              // color bits ignored
		0,                             // no alpha buffer
		0,                             // shift bit ignored
		0,                             // no accumulation buffer
		0, 0, 0, 0,                    // accum bits ignored
		32,                            // 32-bit z-buffer
		0,                             // no stencil buffer
		0,                             // no auxiliary buffer
		PFD_MAIN_PLANE,                // main layer
		0,                             // reserved
		0, 0, 0                        // layer masks ignored
	};

	int m_nPixelFormat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);
	if (m_nPixelFormat == 0)
	{
		MessageBox(_T("ChoosePixelFormat failed."));
		return FALSE;
	}
	if (::SetPixelFormat(m_pDC->GetSafeHdc(), m_nPixelFormat, &pfd) == FALSE)
	{
		MessageBox(_T("SetPixelFormat failed."));
		return FALSE;
	}
	return TRUE;
}

BOOL CEmultationView::InitOpenGL()
{
	//Get a DC for the Client Area
	m_pDC = new CClientDC(this);
	//Failure to Get DC
	if (m_pDC == NULL)
	{
		MessageBox(_T("Error Obtaining DC"));
		return FALSE;
	}
	//Failure to set the pixel format
	if (!SetupPixelFormat())
	{
		return FALSE;
	}
	//Create Rendering Context
	m_hRC = ::wglCreateContext(m_pDC->GetSafeHdc());
	//Failure to Create Rendering Context
	if (m_hRC == 0)
	{
		MessageBox(_T("Error Creating RC"));
		return FALSE;
	}

	//Make the RC Current
	if (::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC) == FALSE)
	{
		MessageBox(_T("Error making RC Current"));
		return FALSE;
	}

	if (!font.InitFont(wglGetCurrentDC(), "Times News Roman", 30))
		return FALSE;

	// 启用阴影平滑
	glShadeModel(GL_SMOOTH);
	//黑色背景
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//设置深度缓存
	glClearDepth(1.0f);
	//启用深度测试
	glEnable(GL_DEPTH_TEST);
	//所作深度测试的类型
	glDepthFunc(GL_LEQUAL);
	//告诉系统对透视进行修正
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);



	//纹理
	glEnable(GL_TEXTURE_2D);
	m_textureGround = new CTargaImage;
	if (!m_textureGround->Load("ground.tga"))
	{
		return false;
	}
	glGenTextures(1, &m_textureGroundObj);
	glBindTexture(GL_TEXTURE_2D, m_textureGroundObj);
	// minimum required to set the min and mag texture filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// now that the texture object is bound, specify a texture for it
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_textureGround->GetWidth(), m_textureGround->GetHeight(),
		0, GL_RGB, GL_UNSIGNED_BYTE, m_textureGround->GetImage());

	GLfloat LightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; 	/**< 环境光参数 */
	GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };		/**< 漫射光参数 */
	GLfloat LightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };	/**< 镜面光参数 */
	/** 设置光源的属性值 */
	float lightPosition[3] = { m_cameraParamameter.lightP.x, m_cameraParamameter.lightP.y, m_cameraParamameter.lightP.z };
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);				/**< 设置环境光 */
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);				/**< 设置漫射光 */
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);			/**< 设置漫射光 */
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);			/**< 设置光源位置 */
																/** 启用光照和光源 */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);

	return TRUE;
}

//#endif //_DEBUG


// CEmultationView 消息处理程序


void CEmultationView::OnDestroy()
{
	CView::OnDestroy();
	//Delete the RC
	if (m_hRC)
	{
		//Make the RC non-current
		if (wglMakeCurrent(NULL, NULL) == FALSE)
		{
			::MessageBox(NULL, _T("释放DC或RC失败。"), _T("关闭错误"), MB_OK | MB_ICONINFORMATION);
		}
		//Delete the rendering context
		if (wglDeleteContext(m_hRC) == FALSE)
		{
			::MessageBox(NULL, _T("释放RC失败。"), _T("关闭错误"), MB_OK | MB_ICONINFORMATION);
		}
		m_hRC = NULL;
	}

	//Delete the DC
	if (m_pDC)
	{
		delete m_pDC;
	}
	//Set it to NULL
	m_pDC = NULL;
}


int CEmultationView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!InitOpenGL())
	{
		::MessageBox(NULL, _T("初始化OpenGL失败."), _T("错误"), MB_OK | MB_ICONEXCLAMATION);
		return -1;
	}
	return 0;
}


void CEmultationView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (cx <= 0 || cy <= 0)
	{
		return;
	}
	//选择投影矩阵
	glMatrixMode(GL_PROJECTION);
	//重置投影矩阵
	glLoadIdentity();
	//计算窗口的外观比例
	gluPerspective(45.0f, (GLfloat)cx / (GLfloat)cy, 0.1f, 3.0*10e+11f);
	//设置模型观察矩阵
	glMatrixMode(GL_MODELVIEW);
	//重置模型观察矩阵
	glLoadIdentity();
	//设置当前的视口
	glViewport(0, 0, cx, cy);
}

void CEmultationView::DrawScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	float length = m_ground.x;
	float width = m_ground.z;
	float height = m_ground.y + 500;
	m_scale = 1 - 7 * m_mouseWheel / 1200;
	length = max(m_scanParameter.send.GetLength(),m_cameraParamameter.startP.GetLength());
	gluLookAt(m_eye.x , m_eye.y , m_eye.z,//eye
				0.0, 0.0, 0.0,//center
				0.0, 1.0, 0.0);//up
	glRotatef(m_angleX, 1, 0, 0);
	glRotatef(m_angleY, 0, 1, 0);
	DrawLight();
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);	/**< 关闭光照 */
	DrawAxis(length/2);
	/*DrawGround(length, width);*/
	DrawScanLine();
	DrawCameraLine();
	//DrawModels();
	DrawPointCloud(m_pointCloud,m_pointCloudType);
	DrawCameraPhoto(m_cameraData,m_cameraDataType);
	DrawScanOut();
	DrawCameraOut();
	glEnable(GL_LIGHTING); /**< 启用光照 */
	glPopAttrib();
	DrawModels();
	SwapBuffers(wglGetCurrentDC());
}

void CEmultationView::DrawAxis(int length)
{
	glPushMatrix();
	
	GLfloat zero[] = { 0, 0, 0 };
	GLfloat x[] = { length, 0, 0 };
	GLfloat y[] = { 0, length, 0 };
	GLfloat z[] = { 0, 0, length };

	glLineWidth(20);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3fv(zero);
	glVertex3fv(x);
	glEnd();
	

	glBegin(GL_LINES);
	glColor3f(0, 1, 0);
	glVertex3fv(zero);
	glVertex3fv(y);
	glEnd();
	

	glBegin(GL_LINES);
	glColor3f(0, 0, 1);
	glVertex3fv(zero);
	glVertex3fv(z);
	glEnd();

	glPopMatrix();
	
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(x[0], x[1], x[2]);
	font.PrintText("X", x[0], x[1]);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(y[0], y[1], y[2]);
	font.PrintText("Y", x[0], x[1]);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(z[0], z[1], z[2]);
	font.PrintText("Z", z[0], z[1]);
	glPopMatrix();

	
}


BOOL CEmultationView::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

//	return CView::OnEraseBkgnd(pDC);
	return TRUE;
}
void CEmultationView::DrawGround(int length, int width)
{
	glPushMatrix();
	GLfloat groundColor[] = { 0.5, 0.5, 0.5 };
	glColor3fv(groundColor);
	glBindTexture(GL_TEXTURE_2D, m_textureGroundObj);
	glBegin(GL_QUADS);
	//地形1
	glTexCoord2f(length, 0.0);   glVertex3f(length / 2, 0, -width / 2);
	glTexCoord2f(0.0, 0.0);      glVertex3f(-length / 2, 0, -width / 2);
	glTexCoord2f(0.0, width);    glVertex3f(-length / 2, 0, width / 2);
	glTexCoord2f(length, width); glVertex3f(length / 2, 0, width / 2);

	//地形2
// 	glTexCoord2f(length, 0.0);   glVertex3f(length, 0, 0);
// 	glTexCoord2f(0.0, 0.0);      glVertex3f(0, 0, 0);
// 	glTexCoord2f(0.0, width);    glVertex3f(0, 0, width);
// 	glTexCoord2f(length, width); glVertex3f(length, 0, width);

	//没有纹理
// 	glVertex3f(length, 0, 0);
// 	glVertex3f(0, 0, 0);
// 	glVertex3f(0, 0, width);
// 	glVertex3f(length, 0, width);
	glEnd();
	glPopMatrix();
}


void CEmultationView::OnModel()
{
	// TODO:  在此添加命令处理程序代码
	if (m_modelDlg.GetSafeHwnd() && m_modelDlg.IsWindowVisible())
	{
		m_modelDlg.ShowWindow(SW_HIDE);
	} 
	else
	{
		if (!m_modelDlg.GetSafeHwnd())
		{
			m_modelDlg.Create(IDD_MODEL, this);
		}
		m_modelDlg.ShowWindow(SW_SHOW);
	}
	Invalidate(1);
}

BOOL CEmultationView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_mouseWheel += zDelta;
	int step = zDelta / 60;
	m_eye.x -= step;
	m_eye.y -= step;
	m_eye.z -= step;
	Invalidate();
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CEmultationView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_leftBtnDown = true;
	m_oldPoint = point;
	CView::OnLButtonDown(nFlags, point);
}


void CEmultationView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_leftBtnDown = false;
	CView::OnLButtonUp(nFlags, point);
}


void CEmultationView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_leftBtnDown)
	{
		m_angleX += (point.y - m_oldPoint.y) / 20;
		m_angleY += (point.x - m_oldPoint.x) / 20;
		Invalidate();
	}
	CView::OnMouseMove(nFlags, point);
}

void CEmultationView::DrawModels()
{
	glPushMatrix();
	glColor3f(0.2, 0.2, 0.7);
	for (auto iter = m_modelMap.begin(); iter != m_modelMap.end(); ++iter)
	{
		/*Move2Center(iter->first);
		iter->second.m_3DModel.m_vCenter.x = 0;
		iter->second.m_3DModel.m_vCenter.y = 0;
		iter->second.m_3DModel.m_vCenter.z = 0;*/
		glPushMatrix();
		glTranslatef(iter->second.GetPosX(), iter->second.GetPosY() , iter->second.GetPosZ());
		glScalef(iter->second.GetScaleX(), iter->second.GetScaleY() , iter->second.GetScaleZ());
		glRotatef(iter->second.GetRotateX(), 1, 0, 0);
		glRotatef(iter->second.GetRotateY(), 0, 1, 0);
		glRotatef(iter->second.GetRotateZ(), 0, 0, 1);
		iter->second.Render();
		glPopMatrix();
	}
	glPopMatrix();
}


void CEmultationView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	//float scale = 1;
	//float startEye = 500 * my::inch2Meter;
	switch (nChar)
	{
	case '1':
		m_eye = { m_ground.x, 100, m_ground.z };
		m_scale = 1.0;
		m_scaleUnit = 50.0;
		m_angleX = 0.0;
		m_angleY = 0.0;
		break;
	case '2':
		m_eye = { m_ground.x / 2, 5, m_ground.z / 2 };
		m_scale = 1.0;
		m_scaleUnit = 2.0;
		m_angleX = 0.0;
		m_angleY = 0.0;
		break;
	case 'M': case 'm':
		if (!IsModelExisted(m_curModelID))
		{
			break;
		}
		m_eye = { m_modelMap[m_curModelID].GetPosX() + 10, m_modelMap[m_curModelID].GetPosY() + 10, m_modelMap[m_curModelID].GetPosZ() + 10 };
		m_scale = 1.0;
		m_scaleUnit = 2.0;
		m_angleX = 0.0;
		m_angleY = 0.0;
		break;
	case VK_UP : case 'W': case 'w':
		m_eye.y += 1;
		m_angleX = 0.0;
		m_angleY = 0.0;
		break;
	case VK_DOWN: case 'S': case 's':
		m_eye.y -= 1;
		m_angleX = 0.0;
		m_angleY = 0.0;
		break;
	case VK_RIGHT: case 'D': case 'd':
		m_eye.x += 1;
		m_angleX = 0.0;
		m_angleY = 0.0;
		break;
	case VK_LEFT: case 'A': case 'a':
		m_eye.x -= 1;
		m_angleX = 0.0;
		m_angleY = 0.0;
		break;
	default:
		break;
	}
	Invalidate();
	CView::OnChar(nChar, nRepCnt, nFlags);
}

void CEmultationView::DrawScanLine()
{
	my::Vector3 temp;
	float Expand = 0.0;
	glPushMatrix();
	glColor3f(1.0, 1.0, 0.0);
	glTranslatef(m_scanParameter.send.x, m_scanParameter.send.y, m_scanParameter.send.z);
	font.PrintText("LD", m_scanParameter.send.x, m_scanParameter.send.y);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0, 1.0, 0.0);
	glLineWidth(3);
	glBegin(GL_LINES);
	temp = m_scanParameter.cur - m_scanParameter.send;
	temp.Normalize();
	Expand = m_scanParameter.send.GetLength();
	temp.GetVector3(Expand*temp.x, Expand*temp.y, Expand*temp.z);
	temp = m_scanParameter.send + temp;
	glVertex3f(m_scanParameter.send.x, m_scanParameter.send.y, m_scanParameter.send.z);
	glVertex3f(temp.x, temp.y, temp.z);
	glEnd();
	glPopMatrix();
}

void CEmultationView::DrawCameraLine()
{
	my::Vector3 temp;
	float Expand = 0.0;
	glPushMatrix();
	glColor3f(1.0, 0.0, 1.0);
	glTranslatef(m_cameraParamameter.startP.x, m_cameraParamameter.startP.y, m_cameraParamameter.startP.z);
	font.PrintText("XJ", m_cameraParamameter.startP.x, m_cameraParamameter.startP.y);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0, 0.0, 1.0);
	glLineWidth(3);
	glBegin(GL_LINES);
	temp = m_cameraParamameter.cur - m_cameraParamameter.startP;
	temp.Normalize();
	Expand = m_cameraParamameter.startP.GetLength();
	temp.GetVector3(Expand*temp.x, Expand*temp.y, Expand*temp.z);
	temp = m_cameraParamameter.startP + temp;
	glVertex3f(m_cameraParamameter.startP.x, m_cameraParamameter.startP.y, m_cameraParamameter.startP.z);
	glVertex3f(temp.x, temp.y, temp.z);
	glEnd();
	glPopMatrix();
}

void CEmultationView::DrawLight()
{
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);	/**< 关闭光照 */
		glTranslatef(m_cameraParamameter.lightP.x, m_cameraParamameter.lightP.y, m_cameraParamameter.lightP.z);
		glutWireSphere(1.0,15,15);
		glEnable(GL_LIGHTING); /**< 启用光照 */
		glPopAttrib();
	glPopMatrix();
}


void CEmultationView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	my::SPACEPOINT scanPoint;
	my::Ray scanRay(m_scanParameter.send, my::Vector3(m_scanParameter.cur - m_scanParameter.send));
	my::SPACEPOINT cameraPoint;
	my::Ray cameraRay(m_cameraParamameter.startP, my::Vector3(m_cameraParamameter.cur - m_cameraParamameter.startP));
	switch (nIDEvent)
	{
	case EVENT_SCANLINE:
		ScanEvent(scanPoint, scanRay);
		break;
	case EVENT_CAMERA:
		CameraEvent(cameraPoint, cameraRay);
	default:
		break;
	}

	CView::OnTimer(nIDEvent);
}

void CEmultationView::DrawScanOut()
{
	if (m_pointVector.empty())
	{
		return;
	}
	glPushMatrix();
	glPointSize(2);
	glColor3f(0, 1, 1);
	glBegin(GL_POINTS);
	for (auto iter = m_pointVector.begin(); iter != m_pointVector.end(); ++iter)
	{
		if (iter->show != -1)
			glVertex3f(iter->x * 1, iter->z + m_cameraParamameter.startP.GetLength() / 2, iter->y * 1);
			glVertex3f(iter->x * 1, iter->z, iter->y * 1);

	}
	glEnd();
	glPopMatrix();
}

void CEmultationView::DrawPointCloud(std::vector<my::SPACEPOINT> m_pointData,std::vector<my::POINT_TYPE> m_pointDataType)
{
	if (m_pointData.empty())
	{
		return;
	}
	glPushMatrix();
	glPointSize(2);
	glColor3f(0, 1, 1);
	glBegin(GL_POINTS);
	for (auto iter = m_pointData.begin(); iter != m_pointData.end();++iter)
	{
		if(m_pointDataType[iter - m_pointData.begin()] != -1)
			glVertex3f(iter->x * 1, iter->y+ m_cameraParamameter.startP.GetLength()/2, iter->z * 1);
			glVertex3f(iter->x * 1, iter->y , iter->z * 1 );

	}
	glEnd();
	glPopMatrix();
}

void CEmultationView::DrawCameraOut()
{
	if (m_cameraPhoto.empty())
	{
		return;
	}

	for (auto iter = m_cameraPhoto.begin(); iter != m_cameraPhoto.end(); ++iter)
	{
		if (iter->show!= -1)
		{
			glPushMatrix();
			glPointSize(2);
			glColor3f(abs(iter->r), abs(iter->g), abs(iter->b));
			glBegin(GL_POINTS);
			glVertex3f(iter->x * 1, /*iter->z*/m_cameraParamameter.startP.GetLength() / 2, iter->y);
			glEnd();
			glPopMatrix();
		}
	}
}

void CEmultationView::DrawCameraPhoto(std::vector<my::SPACEPOINT> m_pointData, std::vector<my::POINT_TYPE> m_pointDataType)
{
	if (m_pointData.empty())
	{
		return;
	}
	
	for (auto iter = m_pointData.begin(); iter != m_pointData.end(); ++iter)
	{
		if (m_pointDataType[iter - m_pointData.begin()] != -1)
		{
			glPushMatrix();
			glPointSize(2);
			glColor3f(abs(iter->Lux.x),abs(iter->Lux.y),abs(iter->Lux.z));
			glBegin(GL_POINTS);
			glVertex3f(iter->x * 1, /*iter->y*/m_cameraParamameter.startP.GetLength()/2, iter->z);
			glEnd();
			glPopMatrix();
		}
	}
	
}

void CEmultationView::OnMenuCamera()
{
	// TODO: 在此添加命令处理程序代码
	CCameraDlg cameraDlg;
	
	cameraDlg.SetCameraParameter(m_cameraParamameter);
	if (cameraDlg.DoModal() == IDOK)
	{
		m_showCameraLine = true;
		m_cameraParamameter = cameraDlg.GetCameraParameter();
		SetTimer(EVENT_CAMERA, 2, NULL);
	}
}

void CEmultationView::OnMenuScan()
{
	// TODO:  在此添加命令处理程序代码
// 	if (m_modelMap.empty())
// 	{
// 		AfxMessageBox(_T("没有模型！请添加模型再扫描！"));
// 		return;
// 	}
	CScanDlg scanDlg;
	scanDlg.SetScanParameter(m_scanParameter);
	if (scanDlg.DoModal() == IDOK)
	{
		m_showScanLine = true;
		m_scanParameter = scanDlg.GetScanParameter();
		SetTimer(EVENT_SCANLINE, 20, NULL);
	}

// 	if (!m_showScanLine)
// 	{
// 		m_showScanLine = true;
// 		m_scanLine.start.x = 5;
// 		m_scanLine.start.y = 5;
// 		m_scanLine.start.z = -1;
// 		m_scanLine.end.x = 0;
// 		m_scanLine.end.y = 0;
// 		m_scanLine.end.z = 0;
// 		m_scanLineStep.x = std::fabs(m_scanLineStep.x);
// 		m_pointCloud.clear();
// 		m_pointCloudType.clear();
// 
// 		SetTimer(EVENT_SCANLINE, 40, NULL);
// 	}
// 	else
// 	{
// 		m_showScanLine = false;
// 		KillTimer(EVENT_SCANLINE);
// 	}
}

bool CEmultationView::RayHitModels(my::Ray scanRay, my::SPACEPOINT &scanPoint, int &modelID)
{
	bool hitModel = false;
	my::SPACEPOINT scanPointTmp;
	scanPointTmp.fDistance = INT_MAX;
	for (auto iter = m_modelMap.begin(); iter != m_modelMap.end();++iter)
	{
		/*Move2Center(iter->first);
		iter->second.m_3DModel.m_vCenter.x = 0;
		iter->second.m_3DModel.m_vCenter.y = 0;
		iter->second.m_3DModel.m_vCenter.z = 0;*/
		if (RayHitModel(scanRay, scanPoint, iter->first))
		{
			if (scanPoint.fDistance < scanPointTmp.fDistance)
			{
				scanPointTmp = scanPoint;
				modelID = iter->first;
			}
			hitModel = true;
		}
	}
	if (!hitModel)
	{
		return false;
	}
	else
	{
		scanPoint = scanPointTmp;
		return true;
	}
}

bool CEmultationView::RayHitModel(my::Ray scanRay, my::SPACEPOINT &scanPoint, int modelID)
{
	if (m_modelMap[modelID].RayHitSphere(scanRay))
	{
		my::Vector3 vModel;
		my::Vector3 Lux1;
		float dist = INT_MAX;
		if (m_modelMap[modelID].RayRealHitModel(scanRay, vModel, dist,Lux1,m_cameraParamameter.lightP))
		{
			scanPoint.x = vModel.x;
			scanPoint.y = vModel.y;
			scanPoint.z = vModel.z;
			scanPoint.fDistance = dist;
			scanPoint.Lux = Lux1;
			return true;
		}
	}

	return false;
}

void CEmultationView::CameraEvent(my::SPACEPOINT &cameraPoint, my::Ray &cameraRay)
{
	my::Vector3 RadarP;
	if (!m_showCameraLine)
	{
		return;
	}
	int modelID;
	if (RayHitModels(cameraRay, cameraPoint, modelID))
	{
		m_cameraData.push_back(cameraPoint);
		m_cameraDataType.push_back(m_modelID2PointType[modelID]);
	}
	else
	{
		m_cameraData.push_back(my::SPACEPOINT(m_cameraParamameter.cur.x, m_cameraParamameter.cur.y, m_cameraParamameter.cur.z, cameraRay.direction.GetLength()));
		m_cameraDataType.push_back(my::POINT_GROUND);
	}
	//if (m_camera_invert)
	//	CameraCurVAngle += VERT_CAMERA_RES;
	//else
	//	CameraCurVAngle -= VERT_CAMERA_RES;
	//RadarP.GetVector3(sin(CameraCurVAngle*PI / 180.0)*cos(CameraCurHAngle*PI / 180.0), sin(CameraCurVAngle*PI / 180.0)*sin(CameraCurHAngle*PI / 180.0), cos(CameraCurVAngle*PI / 180.0));
	//m_cameraParamameter.cur = RoatMartixZ(m_cameraParamameter.angle.x, m_cameraParamameter.angle.y, m_cameraParamameter.angle.z, RadarP, m_cameraParamameter.startP);

	//if ((CameraCurVAngle < -15) || (CameraCurVAngle>15))
	//{
	//	m_camera_invert = m_camera_invert == true ? false : true;
	//	CameraCurHAngle += HORI_CAMERA_RES;
	//	if (CameraCurHAngle > 60)
	//	{
	//		CameraCurHAngle = 60;
	//		//MessageBox(_T("Camera run out"));
	//		m_showCameraLine = false;
	//		std::string filename("CameraRealObj.txt");
	//		SaveRealObjCameraData(filename, m_cameraData, m_cameraDataType);
	//		SaveCameraData("CameraData.txt", m_cameraData, m_cameraDataType);
	//	}
	//}

	/*CameraCurVAngle += VERT_CAMERA_RES;*/
	CameraCurVAngle += 0.6; //调试用
	RadarP.GetVector3(sin(CameraCurVAngle*PI / 180.0)*cos(CameraCurHAngle*PI / 180.0), sin(CameraCurVAngle*PI / 180.0)*sin(CameraCurHAngle*PI / 180.0), cos(CameraCurVAngle*PI / 180.0));
	m_cameraParamameter.cur = RoatMartixY(m_cameraParamameter.angle.x, m_cameraParamameter.angle.y, m_cameraParamameter.angle.z, RadarP, m_cameraParamameter.startP);
	if (CameraCurVAngle > 15)
	{
		/*CameraCurHAngle += HORI_CAMERA_RES;*/
		CameraCurHAngle += 0.6;
		CameraCurVAngle = -15;
	}
	if (CameraCurHAngle > 60)
	{
		CameraCurHAngle = 60;
		//MessageBox(_T("Camera run out"));
		m_showCameraLine = false;
		std::string filename("CameraRealObj.txt");
		SaveRealObjCameraData(filename, m_cameraData, m_cameraDataType);
		SaveCameraData("CameraData.txt", m_cameraData, m_cameraDataType);
	}

	Invalidate();
}
void CEmultationView::ScanEvent(my::SPACEPOINT &scanPoint, my::Ray &scanRay)
{
	my::Vector3 RadarP;
	if (!m_showScanLine)
	{
		return;
	}
	int modelID;
	if (RayHitModels(scanRay, scanPoint,modelID))
	{
		m_pointCloud.push_back(scanPoint);
		m_pointCloudType.push_back(m_modelID2PointType[modelID]);
	}
	else
	{
		m_pointCloud.push_back(my::SPACEPOINT(m_scanParameter.cur.x, m_scanParameter.cur.y, m_scanParameter.cur.z, scanRay.direction.GetLength()));
		m_pointCloudType.push_back(my::POINT_GROUND);
	}
	//if (m_invert)
	//	CurVertAngle += VERT_SCAN_VERT_RES;
	//else
	//	CurVertAngle -= VERT_SCAN_VERT_RES;
	//RadarP.GetVector3(sin(CurVertAngle*PI / 180.0)*cos(CurHoriAngle*PI / 180.0), sin(CurVertAngle*PI / 180.0)*sin(CurHoriAngle*PI / 180.0), cos(CurVertAngle*PI / 180.0));
	//m_scanParameter.cur = RoatMartixY(m_scanParameter.angle.x, m_scanParameter.angle.y, m_scanParameter.angle.z, RadarP, m_scanParameter.send);

	//if ((CurVertAngle < -15) || (CurVertAngle>15))
	//{
	//	m_invert = m_invert == true ? false : true;
	//	CurHoriAngle += HORI_AZIMUTH_RES;
	//	if (CurHoriAngle > 60)
	//	{
	//		CurHoriAngle = 60;
	//		//MessageBox(_T("Scan run out"));
	//		m_showScanLine = false;
	//		std::string filename("RealObj.txt");
	//		SaveRealObjPointCloud(filename, m_pointCloud, m_pointCloudType);
	//		SavePointCloud("all.txt", m_pointCloud, m_pointCloudType);
	//	}
	//}
	
	/*CurVertAngle += VERT_SCAN_VERT_RES;*/
	CurVertAngle += 0.6;//调试用
	RadarP.GetVector3(sin(CurVertAngle*PI / 180.0)*cos(CurHoriAngle*PI / 180.0), sin(CurVertAngle*PI / 180.0)*sin(CurHoriAngle*PI / 180.0), cos(CurVertAngle*PI / 180.0));
	m_scanParameter.cur = RoatMartixY(m_scanParameter.angle.x, m_scanParameter.angle.y, m_scanParameter.angle.z, RadarP, m_scanParameter.send);
	if (CurVertAngle > 15)
	{
		/*CurHoriAngle += HORI_AZIMUTH_RES;*/
		CurHoriAngle += 0.6;//调试用
		CurVertAngle = -15;
	}
	if (CurHoriAngle > 60)
	{
		CurHoriAngle = 60;
		//MessageBox(_T("Scan run out"));
		m_showScanLine = false;
		std::string filename("RealObj.txt");
		SaveRealObjPointCloud(filename, m_pointCloud, m_pointCloudType);
		SavePointCloud("all.txt", m_pointCloud, m_pointCloudType);
	}

	Invalidate();
}

void CEmultationView::SavePointCloud(const std::string &fileName, std::vector<my::SPACEPOINT> m_pointData, std::vector<my::POINT_TYPE> m_pointDataType)
{
	std::ofstream outFile;
	outFile.open(fileName, std::ios::out);
	if (!outFile.good())
	{
		AfxMessageBox(_T("打开文件失败!"));
		outFile.close();
		return;
	}

	outFile << m_pointData.size() << "\t" << 4 << std::endl;

	for (auto iter = m_pointData.begin(); iter != m_pointData.end();++iter)
	{
		outFile << iter->x << "\t" << iter->z << "\t" << iter->y <<"\t"<<iter->fDistance;
		outFile << "\t" << m_pointDataType[iter - m_pointData.begin()] << std::endl;
	}


	outFile.close();
}

void CEmultationView::SaveCameraData(const std::string &fileName, std::vector<my::SPACEPOINT> m_pointData, std::vector<my::POINT_TYPE> m_pointDataType)
{
	std::ofstream outFile;
	outFile.open(fileName, std::ios::out);
	if (!outFile.good())
	{
		AfxMessageBox(_T("打开文件失败!"));
		outFile.close();
		return;
	}

	outFile << m_pointData.size() << "\t" << 4 << std::endl;

	for (auto iter = m_pointData.begin(); iter != m_pointData.end(); ++iter)
	{
		outFile << iter->x << "\t" << iter->z << "\t" << iter->y << "\t" << abs(iter->Lux.x) << "\t" << abs(iter->Lux.y) << "\t" << abs(iter->Lux.z);
		outFile << "\t" << m_pointDataType[iter - m_pointData.begin()] << std::endl;
	}


	outFile.close();
}

void CEmultationView::AddModel(int id, const C3DSModel &model)
{
	m_modelMap[id] = model;
	m_modelID2PointType[id] = my::POINT_OBJ_DOWN;
	if (m_modelMap.size() == 2)
	{
		int firstModelID = m_modelMap.begin()->first;
		int secondModelID = std::next(m_modelMap.begin())->first;
		if (m_modelMap[firstModelID].GetMinHeight() > m_modelMap[secondModelID].GetMinHeight())
		{
			m_modelID2PointType[firstModelID] = my::POINT_OBJ_UP;
		}
		else
		{
			m_modelID2PointType[secondModelID] = my::POINT_OBJ_UP;
		}
	}
}

void CEmultationView::SaveRealObjPointCloud(const std::string &fileName, std::vector<my::SPACEPOINT> m_pointData, std::vector<my::POINT_TYPE> m_pointDataType)
{
	std::ofstream outFile;
	outFile.open(fileName, std::ios::out);
	if (!outFile.good())
	{
		AfxMessageBox(_T("打开文件失败!"));
		outFile.close();
		return;
	}

	int sum = 0;
	for (auto iter = m_pointDataType.begin(); iter != m_pointDataType.end(); ++iter)
	{
		if (my::POINT_GROUND != *iter)
		{
			++sum;
		}
	}
	outFile << sum << " " << 4 << std::endl;

	for (auto iter = m_pointData.begin(); iter != m_pointData.end(); ++iter)
	{
		if (my::POINT_GROUND == m_pointDataType[iter - m_pointData.begin()])
			continue;
		outFile << iter->x << " " << iter->z << " " << iter->y;
		outFile << " " << m_pointDataType[iter - m_pointData.begin()] << std::endl;
	}


	outFile.close();
}
void CEmultationView::SaveRealObjCameraData(const std::string &fileName, std::vector<my::SPACEPOINT> m_pointData, std::vector<my::POINT_TYPE> m_pointDataType)
{
	std::ofstream outFile;
	outFile.open(fileName, std::ios::out);
	if (!outFile.good())
	{
		AfxMessageBox(_T("打开文件失败!"));
		outFile.close();
		return;
	}

	int sum = 0;
	for (auto iter = m_pointDataType.begin(); iter != m_pointDataType.end(); ++iter)
	{
		if (my::POINT_GROUND != *iter)
		{
			++sum;
		}
	}
	outFile << sum << " " << 4 << std::endl;

	for (auto iter = m_pointData.begin(); iter != m_pointData.end(); ++iter)
	{
		if (my::POINT_GROUND == m_pointDataType[iter - m_pointData.begin()])
			continue;
		outFile << iter->x << " " << iter->z << " " << iter->y;
		outFile << " " << m_pointDataType[iter - m_pointData.begin()] << std::endl;
	}


	outFile.close();
}

my::Vector3  CEmultationView::RoatMartixZ(float a, float b, float c, my::Vector3 RadarP, my::Vector3 TransP)
{
	float Alpha = PI*a / 180;
	float Beta = PI *b / 180;
	float Gama = PI*c / 180;
	my::Vector3 XRoate[3], YRoate[3], ZRoate[3], temp[3], RoateM[3],ResultP;
	XRoate[0].GetVector3(1, 0, 0);
	XRoate[1].GetVector3(0, cos(Alpha), sin(Alpha));
	XRoate[2].GetVector3(0, -sin(Alpha), cos(Alpha));
	
	YRoate[0].GetVector3(cos(Beta), 0, -sin(Beta));
	YRoate[1].GetVector3(0, 1, 0);
	YRoate[2].GetVector3(sin(Beta), 0, cos(Beta));
	
	ZRoate[0].GetVector3(cos(Gama), -sin(Gama), 0);
	ZRoate[1].GetVector3(sin(Gama), cos(Gama), 0);
	ZRoate[2].GetVector3(0, 0, 1);
	for (int i = 0; i<3; i++)
	{
		temp[i].GetVector3(ZRoate[i].Dot(YRoate[0]), ZRoate[i].Dot(YRoate[1]), ZRoate[i].Dot(YRoate[2]));
	}
	for (int i = 0; i<3; i++)
	{
		RoateM[i].GetVector3(temp[i].Dot(XRoate[0]), temp[i].Dot(XRoate[1]), temp[i].Dot(XRoate[2]));
	}
	temp[0].GetVector3(RoateM[0].x, RoateM[1].x, RoateM[2].x);
	temp[1].GetVector3(RoateM[0].y, RoateM[1].y, RoateM[2].y);
	temp[2].GetVector3(RoateM[0].z, RoateM[1].z, RoateM[2].z);
	RoateM[0] = temp[0];
	RoateM[1] = temp[1];
	RoateM[2] = temp[2];
	/*RoateM[0].GetVector3(0, -0.7072, -0.7072);
	RoateM[1].GetVector3(0, 0.7072, -0.7072);
	RoateM[2].GetVector3(1,0,0);*/
	ResultP.GetVector3(RadarP.Dot(RoateM[0]), RadarP.Dot(RoateM[1]), RadarP.Dot(RoateM[2]));
	ResultP = ResultP + TransP;
	return ResultP;

}

my::Vector3  CEmultationView::RoatMartixY(float a, float b, float c, my::Vector3 RadarP, my::Vector3 TransP)
{
	float Alpha = PI*a / 180;
	float Beta = PI *b / 180;
	float Gama = PI*c / 180;
	my::Vector3 XRoate[3], YRoate[3], ZRoate[3], temp[3], RoateM[3], ResultP;
	XRoate[0].GetVector3(1, 0, 0);
	XRoate[1].GetVector3(0, cos(Alpha), sin(Alpha));
	XRoate[2].GetVector3(0, -sin(Alpha), cos(Alpha));

	YRoate[0].GetVector3(cos(Beta), 0, sin(Beta));
	YRoate[1].GetVector3(0, 1, 0);
	YRoate[2].GetVector3(-sin(Beta), 0, cos(Beta));

	ZRoate[0].GetVector3(cos(Gama), sin(Gama), 0);
	ZRoate[1].GetVector3(-sin(Gama), cos(Gama), 0);
	ZRoate[2].GetVector3(0, 0, 1);
	for (int i = 0; i<3; i++)
	{
		temp[i].GetVector3(YRoate[i].Dot(ZRoate[0]), YRoate[i].Dot(ZRoate[1]), YRoate[i].Dot(ZRoate[2]));
	}
	for (int i = 0; i<3; i++)
	{
		RoateM[i].GetVector3(temp[i].Dot(XRoate[0]), temp[i].Dot(XRoate[1]), temp[i].Dot(XRoate[2]));
	}
	temp[0].GetVector3(RoateM[0].x, RoateM[1].x, RoateM[2].x);
	temp[1].GetVector3(RoateM[0].y, RoateM[1].y, RoateM[2].y);
	temp[2].GetVector3(RoateM[0].z, RoateM[1].z, RoateM[2].z);
	RoateM[0] = temp[0];
	RoateM[1] = temp[1];
	RoateM[2] = temp[2];
	/*RoateM[0].GetVector3(0, -0.7072, -0.7072);
	RoateM[1].GetVector3(0, 0.7072, -0.7072);
	RoateM[2].GetVector3(1,0,0);*/
	ResultP.GetVector3(RadarP.Dot(RoateM[0]), RadarP.Dot(RoateM[1]), RadarP.Dot(RoateM[2]));
	ResultP = ResultP + TransP;
	return ResultP;

}

void CEmultationView::Move2Center(int id)
{
	/*float scale = m_modelMap[id].GetTransPara().scaleX;*/
	float scale = 1.0;
	//double dSphereRadius = 0.0;
	my::Vector3 vec_SphereCenter;
	vec_SphereCenter = m_modelMap[id].m_3DModel.m_vCenter;
	// 初始时刻载入的3ds文件的外接球及外接球半径
	//dSphereRadius = static_cast<double>(m_modelMap[id].m_3DModel.m_Rais * scale);
	m_modelMap[id].m_3DModel.m_Rais =  m_modelMap[id].m_3DModel.m_Rais*scale;
	for (auto it1 = m_modelMap[id].m_3DModel.pObject.begin(); it1 != m_modelMap[id].m_3DModel.pObject.end(); it1++)
	{
		// 读取每个物体的顶点信息
		for (int j = 0; j < it1->numOfVerts; j++)
		{
			my::Vector3 TEMP = it1->pVerts[j];
			it1->pVerts[j] = (it1->pVerts[j] - vec_SphereCenter)*scale;
			//fprintf(fp, "%f %f %f\n", it1->pVerts[j].x, it1->pVerts[j].y, it1->pVerts[j].z);
		}
	}
}

void CEmultationView::FindandGet(CString LineStr,float *point)
{
	CString Strtemp;
	CString temp;
	int idx = 0;
	int len = 0;
	int i = 0;
	while(idx != -1)
	{
		len = LineStr.GetLength();
		idx = LineStr.Find(_T("\t"));
		temp = LineStr.Mid(0,idx);
		point[i] = _ttof(temp);
		Strtemp = LineStr.Right(len - (idx + 1));
		LineStr = Strtemp;
		i++;
	}
}

void CEmultationView::OnMenuScanout()
{
	// TODO: 在此添加命令处理程序代码
	CString   FilePathName;//文件名参数定义
	//FILE *m_FilePtr;
	CStdioFile cFile;
	float *pointcloud = new float [10];
	CString strText = _T("");
	my::MakeOUT pointtemp;
	std::vector<CString> strVector;//用于保存每行读取出来的内容
	/*std::vector<my::Vector3>pointVector;*/
	CFileDialog  Dlg(TRUE,_T(".txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("TXT Files(*.txt)|*.txt|All Files(*.*)|*.*"));
	//打开文件
	if (Dlg.DoModal() == IDOK)//是否打开成功
	{
		FilePathName = Dlg.GetPathName();//取得文件路径及文件名
		//SetDlgItemText(IDC_OutPutFile, FilePathName);//在一个地址为IDC_OutPutFile的editbox中显示文件名
	}
	else//打开失败处理
	{
		//打开失败处理
		MessageBox(_T("打开失败"), NULL, MB_OK);
	}
	int nLength = FilePathName.GetLength();
	int nBytes = WideCharToMultiByte(CP_ACP, 0, FilePathName, nLength, NULL, 0, NULL, NULL);
	char* cFileName = new char[nBytes + 1];
	memset(cFileName, 0, nLength + 1);//CP_OEMCP
	WideCharToMultiByte(CP_ACP, 0, FilePathName, nLength, cFileName, nBytes, NULL, NULL);

	if (!cFile.Open(FilePathName, CFile::modeRead))
	{
		MessageBox(_T("文件打开失败。"));
		return;
	}
	while (cFile.ReadString(strText))
	{
		strVector.push_back(strText);
		//MessageBox(strText);
	}
	for (auto iter = strVector.begin()+1; iter != strVector.end(); ++iter)
	{
		FindandGet(*iter, pointcloud);
		pointtemp.GetShowScan(pointcloud[0], pointcloud[1], pointcloud[2],pointcloud[4]);
		m_pointVector.push_back(pointtemp);
	}
	//关闭文件
	cFile.Close();
}


void CEmultationView::OnMenuCameraout()
{
	// TODO: 在此添加命令处理程序代码
	CString   FilePathName;//文件名参数定义
						   //FILE *m_FilePtr;
	CStdioFile cFile;
	float *pointcloud = new float[10];
	CString strText = _T("");
	my::MakeOUT pointtemp;
	std::vector<CString> strVector;//用于保存每行读取出来的内容
								   /*std::vector<my::Vector3>pointVector;*/
	CFileDialog  Dlg(TRUE, _T(".txt"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("TXT Files(*.txt)|*.txt|All Files(*.*)|*.*"));
	//打开文件
	if (Dlg.DoModal() == IDOK)//是否打开成功
	{
		FilePathName = Dlg.GetPathName();//取得文件路径及文件名
										 //SetDlgItemText(IDC_OutPutFile, FilePathName);//在一个地址为IDC_OutPutFile的editbox中显示文件名
	}
	else//打开失败处理
	{
		//打开失败处理
		MessageBox(_T("打开失败"), NULL, MB_OK);
	}
	int nLength = FilePathName.GetLength();
	int nBytes = WideCharToMultiByte(CP_ACP, 0, FilePathName, nLength, NULL, 0, NULL, NULL);
	char* cFileName = new char[nBytes + 1];
	memset(cFileName, 0, nLength + 1);//CP_OEMCP
	WideCharToMultiByte(CP_ACP, 0, FilePathName, nLength, cFileName, nBytes, NULL, NULL);

	if (!cFile.Open(FilePathName, CFile::modeRead))
	{
		MessageBox(_T("文件打开失败。"));
		return;
	}
	while (cFile.ReadString(strText))
	{
		strVector.push_back(strText);
		//MessageBox(strText);
	}
	for (auto iter = strVector.begin() + 1; iter != strVector.end(); ++iter)
	{
		FindandGet(*iter, pointcloud);
		pointtemp.GetShowCamera(pointcloud[0], pointcloud[1], pointcloud[2], pointcloud[3],pointcloud[4],pointcloud[5],pointcloud[6]);
		m_cameraPhoto.push_back(pointtemp);
	}
	//关闭文件
	cFile.Close();
}


void CEmultationView::OnMenuFlush()
{
	// TODO: 在此添加命令处理程序代码
	m_pointVector.clear();
	m_cameraPhoto.clear();
}
