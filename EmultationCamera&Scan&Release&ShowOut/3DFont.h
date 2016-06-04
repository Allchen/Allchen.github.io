//========================================================
/**
*  @file  GLFont.h
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
#ifndef	__GLFONT_H__
#define	__GLFONT_H__

#pragma warning(disable: 4267)  /**< 禁止编译器出现类型转换的警告 */

/** 定义位图字体类 */
class GLFont															
{
public:
    /** 构造函数和析构函数 */
	GLFont();
	~GLFont();
    ///成员方法
	bool InitFont(HDC hDC, char *fontName,int Size );  /**< 初始化字体 */
	void PrintText(char *string, float x, float y);/**< 在位置(x,y)处绘制字符串string */
	void KillGLFont();  /**< 删除字体 */

protected:
	unsigned int listBase;  /**< 绘制字体的显示列表的开始位置 */
    GLYPHMETRICSFLOAT gmf[256];		/**< 记录256个字符的信息 */
	
};

#endif	// __GLFONT_H__