/**
*  @file  GLFont.cpp
*
*  ��Ŀ������ 3D������ʾ
*  �ļ�����:  ������  
*  ����ƽ̨�� Windows98/2000/NT/XP
*  
*  ���ߣ�     BrightXu
*  �����ʼ�:  huoxini@hotmail.com
*  �������ڣ� 2006-09-13	
*  �޸����ڣ� 2006-11-23
*
*/     
//========================================================

#include<windows.h>              /**< ����windowsͷ�ļ� */
#include "stdafx.h"				/**< ����glͷ�ļ� */
#include "3DFont.h"

GLFont::GLFont()
{
	
}
/** �������� */
GLFont::~GLFont()
{
	KillGLFont();/**< ����KillFont()ɾ������ */
}
/** ��ʼ������ */
bool GLFont::InitFont(HDC hDC, char *fontName,int Size )
{
   HFONT oldFont; /**< ����ɵ������� */
   HFONT hFont;  /**< ������ */
   /**< ����256����ʾ�б� */
   listBase = glGenLists(256);
   if(listBase == 0) /**< ����ʧ��,�򷵻� */
	   return false;
   WCHAR wszFontname[256];
   memset(wszFontname, 0, sizeof(wszFontname));
   MultiByteToWideChar(CP_ACP, 0, fontName, strlen(fontName) + 1, wszFontname, sizeof(wszFontname) / sizeof(wszFontname[0]));


    /**< �������� */
   hFont = CreateFont( Size,					/**< ����߶� */
						0,						/**< ������ */
						0,						/**< �������ת�Ƕ� Angle Of Escapement */
						0,						/**< ������ߵ���ת�Ƕ�Orientation Angle */
						FW_BOLD,				/**< ��������� */
						FALSE,					/**< �Ƿ�ʹ��б�� */
						FALSE,					/**< �Ƿ�ʹ���»��� */
						FALSE,					/**< �Ƿ�ʹ��ɾ���� */
						ANSI_CHARSET,			/**< �����ַ��� */
						OUT_TT_PRECIS,			/**< ������� */
						CLIP_DEFAULT_PRECIS,	/**< �ü����� */
						ANTIALIASED_QUALITY,	/**< ������� */
				FF_DONTCARE|DEFAULT_PITCH,		/**< Family And Pitch */
					 wszFontname);				/**< �������� */
	 if(!hFont)
		return false;  /**< ��������ʧ���򷵻� */
 
	oldFont = (HFONT)SelectObject(hDC, hFont); /**< ѡ��������Ҫ������ */
	
	wglUseFontOutlines(	hDC,					/**< ���õ�ǰ�����豸������ľ�� */
						0,						/**< ���ڴ�����ʾ�б�����ĵ�һ���ַ���ASCIIֵ */
						255,					/**< �ַ��� */
						listBase,				/**< ��һ����ʾ�б������ */
						0.0f,					/**< ����Ĺ⻬�ȣ�ԽСԽ�⻬��0.0Ϊ��⻬��״̬ */
						0.2f,					/**< ��z����ͻ���ľ��� */
						WGL_FONT_POLYGONS,		/**< ʹ�ö�����������ַ���ÿ��������ж����ķ��� */
						gmf);					/**< һ���������ζ������ݵ�����ĵ�ַ��ÿ������Ԫ��������Ӧ����ʾ�б��ַ���������� */

	SelectObject(hDC,oldFont);      /**< �ָ�ԭ�������� */
	DeleteObject(hFont);            /**< ɾ������ */
 
   return true;
}



/** ��ָ��λ������ַ��� */
void GLFont::PrintText(char *string, float x, float y)
{
   float  length=0;			/**< ���ڲ�ѯ�ַ����ĳ��� */
   
   ///������.���listBaseΪ0��stringΪNULL���û���ֱ�ӷ���
   if((listBase == 0 || string == NULL))
      return ;

   glPushMatrix();
	 glDisable(GL_DEPTH_TEST); /**< �ر���Ȳ��� */
	// glDisable(GL_LIGHTING); /**< �رչ��� */

	 glRasterPos2f(x, y); /**< ����դλ���ƶ���ָ���� */
	///���ַ����������Ļ��
	for (unsigned int loop = 0; loop < (strlen(string)); loop++)	/**< ���������ַ����ĳ��� */
	{
		length += gmf[string[loop]].gmfCellIncX;		
	}
	glTranslatef(-length/2,0.0f,0.0f);			/**< ���ַ������������ */
	glPushAttrib(GL_LIST_BIT);					/**< ����ʾ�б�����ѹ�����Զ�ջ */
	  glListBase(listBase);							/**< ������ʾ�б�Ļ���ֵΪ0 */
	  glCallLists(strlen(string), GL_UNSIGNED_BYTE, string);		/**< ������ʾ�б�����ַ��� */
	glPopAttrib();								/**< �������Զ�ջ */
	
    ///�ָ�ԭ��״̬
	//glEnable(GL_LIGHTING); /**< ���ù��� */
	glEnable(GL_DEPTH_TEST); /**< ������Ȳ��� */
   glPopMatrix();
 
}

/**< ɾ������ */
void GLFont::KillGLFont()
{
   /** ɾ��������ʾ�б� */
   if(listBase != 0)
      glDeleteLists(listBase, 256);
}

