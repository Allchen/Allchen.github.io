#pragma once
#include "DataStructs.h"
#include "ModelParameter.h"
#include <cmath>

class C3DSModel
{
public:
	C3DSModel();
	~C3DSModel();
	bool IsLoad();											//是否已加载模型
	bool Load3DS(CString filename);
	void Render(void);
	float GetMinHeight()const { return m_3DModel.m_minV.y; }

	void SetTransPara(my::T3ds transPara);
	void SetPosX(float x){ m_transPara.x = x; }
	void SetPosY(float y){ m_transPara.y = y; }
	void SetPosZ(float z){ m_transPara.z = z; }
	void SetRotateX(float x){ m_transPara.d_x = x; }
	void SetRotateY(float y){ m_transPara.d_y = y; }
	void SetRotateZ(float z){ m_transPara.d_z = z; }
	void SetScaleX(float x){ m_transPara.scaleX = x; }
	void SetScaleY(float y){ m_transPara.scaleY = y; }
	void SetScaleZ(float z){ m_transPara.scaleZ = z; }
	my::T3ds GetTransPara(){ return m_transPara; }
	float GetPosX(){ return m_transPara.x; }
	float GetPosY(){ return m_transPara.y; }
	float GetPosZ(){ return m_transPara.z; }
	float GetRotateX(){ return m_transPara.d_x; }
	float GetRotateY(){ return m_transPara.d_y; }
	float GetRotateZ(){ return m_transPara.d_z; }
	float GetScaleX(){ return m_transPara.scaleX; }
	float GetScaleY(){ return m_transPara.scaleY; }
	float GetScaleZ(){ return m_transPara.scaleZ; }
	bool RayHitSphere( my::Ray &ray);
	bool RayRealHitModel(my::Ray &ray, my::Vector3 &vecHit, float &dist,my::Vector3 &Lux,my::Vector3 LightP);
	float GetRais();
	my::Vector3 GetCenterPoint();
	bool GetModelFaceVert(int vertID, my::Vector3 &vert);
	void UpdataModelMatrix();
	void CalModelFaceVert();
	my::t3DModel m_3DModel;
	
private:
	FILE *m_FilePtr;										// 3ds文件指针
	int  DisplayList;										////模型的显示列表编号，用于opengl渲染绘图用
	my::T3ds m_transPara;
	bool m_isLoad;
	std::vector<my::Vector3> m_modelFaceVert;							//模型顶点在世界坐标系中的位置
	float m_modelChangeMatrix[16];
	float m_modelTransMatrix[16];
	float m_modelRotateXMatrix[16];
	float m_modelRotateYMatrix[16];
	float m_modelRotateZMatrix[16];
	float m_modelScaleMatrix[16];

	BYTE ReadByte(void);									// 从文件中读取1个字节
	WORD ReadWord(void);									// 从文件中读取2个字节
	UINT ReadUint(void);									// 从文件中读取4个字节
	UINT ReadString(my::STRING *pStr);						// 从文件中读取字符串（返回字符串长度）
	float ReadFloat(void);									// 从文件中读取浮点数
	UINT ReadMaterial(UINT n);								// 读取材质
	UINT ReadObject(UINT n);								// 读取3ds对象	
	bool ReadChunk(my::tChunk *pChunk);						// 读取3ds的一个Chunk信息
	UINT ReadPrimary(UINT n);								// 读取3ds文件主要Chunk
	UINT ReadEdit(UINT n);									// 读取3ds物体主编辑Chunk
	UINT ReadObjectInfo(my::t3DObject *pObj, UINT n);		// 读取3ds对象信息
	UINT ReadFacetInfo(my::t3DObject *pObj, UINT n);		// 读取面信息
	UINT ReadMatDif(my::tMaterial *pMat, UINT n);			// 读取材质的漫反射属性
	UINT ReadMatMap(my::tMaterial *pMat, UINT n);			// 读取材质的纹理
	my::Vector3 TransformVert(const my::Vector3& fPointSrc);
	void Release(void);										// 释放3ds模型资源
	

	my::Vector3 Cross(my::Vector3 v1, my::Vector3 v2);		// 计算两向量的叉积
	my::Vector3 Normalize(my::Vector3 vNormal);				// 向量单位化
	my::Vector3 GetLux(my::t3DObject *obj,int nFace,my::Vector3 Crossdot,my::Vector3 lightP);
	void ComputeNormals(void);								// 计算顶点法线量
	void MatrixMulti(float left[], float right[], int rowLeft, int colLeft, int colRight, float res[]);
	void ChangeCenterXYZ(float before[], float after[]);
	bool GetLineABCD(const my::Ray &ray, float &A, float &B, float &C, float &D);//Ax + By + Cz + D = 0;
	bool TriangleHitFace(double A, double B, double C, double D, my::Vector3 triVerts[]);
	bool RayHitTriangle(const my::Ray &ray, my::Vector3 vtri[], float &t, float &u, float &v);
	bool RayHitTriFace(const my::Ray &ray, my::Vector3 vtri[], my::Vector3& vecHit, float &dist);
};

