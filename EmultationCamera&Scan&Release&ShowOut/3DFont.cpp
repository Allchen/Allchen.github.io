/**
*  @file  GLFont.cpp
*
*  项目描述： 3D字体演示
*  文件描述:  字体类  
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     BrightXu
*  电子邮件:  huoxini@hotmail.com
*  创建日期： 2006-09-13	
*  修改日期： 2006-11-23
*
*/     
//========================================================

#include<windows.h>              /**< 包含windows头文件 */
#include "stdafx.h"				/**< 包含gl头文件 */
#include "3DFont.h"

GLFont::GLFont()
{
	
}
/** 析构函数 */
GLFont::~GLFont()
{
	KillGLFont();/**< 调用KillFont()删除字体 */
}
/** 初始化字体 */
bool GLFont::InitFont(HDC hDC, char *fontName,int Size )
{
   HFONT oldFont; /**< 保存旧的字体句柄 */
   HFONT hFont;  /**< 字体句柄 */
   /**< 创建256个显示列表 */
   listBase = glGenLists(256);
   if(listBase == 0) /**< 分配失败,则返回 */
	   return false;
   WCHAR wszFontname[256];
   memset(wszFontname, 0, sizeof(wszFontname));
   MultiByteToWideChar(CP_ACP, 0, fontName, strlen(fontName) + 1, wszFontname, sizeof(wszFontname) / sizeof(wszFontname[0]));


    /**< 创建字体 */
   hFont = CreateFont( Size,					/**< 字体高度 */
						0,						/**< 字体宽度 */
						0,						/**< 字体的旋转角度 Angle Of Escapement */
						0,						/**< 字体底线的旋转角度Orientation Angle */
						FW_BOLD,				/**< 字体的重量 */
						FALSE,					/**< 是否使用斜体 */
						FALSE,					/**< 是否使用下划线 */
						FALSE,					/**< 是否使用删除线 */
						ANSI_CHARSET,			/**< 设置字符集 */
						OUT_TT_PRECIS,			/**< 输出精度 */
						CLIP_DEFAULT_PRECIS,	/**< 裁剪精度 */
						ANTIALIASED_QUALITY,	/**< 输出质量 */
				FF_DONTCARE|DEFAULT_PITCH,		/**< Family And Pitch */
					 wszFontname);				/**< 字体名称 */
	 if(!hFont)
		return false;  /**< 创建字体失败则返回 */
 
	oldFont = (HFONT)SelectObject(hDC, hFont); /**< 选择我们需要的字体 */
	
	wglUseFontOutlines(	hDC,					/**< 设置当前窗口设备描述表的句柄 */
						0,						/**< 用于创建显示列表字体的第一个字符的ASCII值 */
						255,					/**< 字符数 */
						listBase,				/**< 第一个显示列表的名称 */
						0.0f,					/**< 字体的光滑度，越小越光滑，0.0为最光滑的状态 */
						0.2f,					/**< 在z方向突出的距离 */
						WGL_FONT_POLYGONS,		/**< 使用多边形来生成字符，每个顶点具有独立的法线 */
						gmf);					/**< 一个接收字形度量数据的数组的地址，每个数组元素用它对应的显示列表字符的数据填充 */

	SelectObject(hDC,oldFont);      /**< 恢复原来的字体 */
	DeleteObject(hFont);            /**< 删除字体 */
 
   return true;
}



/** 在指定位置输出字符串 */
void GLFont::PrintText(char *string, float x, float y)
{
   float  length=0;			/**< 用于查询字符串的长度 */
   
   ///错误检查.如果listBase为0或string为NULL则不用绘制直接返回
   if((listBase == 0 || string == NULL))
      return ;

   glPushMatrix();
	 glDisable(GL_DEPTH_TEST); /**< 关闭深度测试 */
	// glDisable(GL_LIGHTING); /**< 关闭光照 */

	 glRasterPos2f(x, y); /**< 将光栅位置移动到指定点 */
	///将字符串输出到屏幕上
	for (unsigned int loop = 0; loop < (strlen(string)); loop++)	/**< 查找整个字符串的长度 */
	{
		length += gmf[string[loop]].gmfCellIncX;		
	}
	glTranslatef(-length/2,0.0f,0.0f);			/**< 把字符串置于最左边 */
	glPushAttrib(GL_LIST_BIT);					/**< 把显示列表属性压入属性堆栈 */
	  glListBase(listBase);							/**< 设置显示列表的基础值为0 */
	  glCallLists(strlen(string), GL_UNSIGNED_BYTE, string);		/**< 调用显示列表绘制字符串 */
	glPopAttrib();								/**< 弹出属性堆栈 */
	
    ///恢复原来状态
	//glEnable(GL_LIGHTING); /**< 启用光照 */
	glEnable(GL_DEPTH_TEST); /**< 启用深度测试 */
   glPopMatrix();
 
}

/**< 删除字体 */
void GLFont::KillGLFont()
{
   /** 删除字体显示列表 */
   if(listBase != 0)
      glDeleteLists(listBase, 256);
}

