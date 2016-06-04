//========================================================
/**
*  @file  GLFont.h
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
#ifndef	__GLFONT_H__
#define	__GLFONT_H__

#pragma warning(disable: 4267)  /**< ��ֹ��������������ת���ľ��� */

/** ����λͼ������ */
class GLFont															
{
public:
    /** ���캯������������ */
	GLFont();
	~GLFont();
    ///��Ա����
	bool InitFont(HDC hDC, char *fontName,int Size );  /**< ��ʼ������ */
	void PrintText(char *string, float x, float y);/**< ��λ��(x,y)�������ַ���string */
	void KillGLFont();  /**< ɾ������ */

protected:
	unsigned int listBase;  /**< �����������ʾ�б�Ŀ�ʼλ�� */
    GLYPHMETRICSFLOAT gmf[256];		/**< ��¼256���ַ�����Ϣ */
	
};

#endif	// __GLFONT_H__