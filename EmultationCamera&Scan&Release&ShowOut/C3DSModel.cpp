#include "stdafx.h"
#include "C3DSModel.h"

bool BuildTexture(char *szPathName, GLuint &texid, int nAlpha, COLORREF clrTransparentKey);

C3DSModel::C3DSModel() :
m_FilePtr(nullptr),
DisplayList(0),
m_isLoad(false),
m_modelFaceVert(std::vector<my::Vector3>())
{
	// 初始化保存3DS模型的结构体
	m_3DModel.numOfMaterials = 0;
	m_3DModel.numOfObjects = 0;
	m_3DModel.m_maxV = my::Vector3(INT_MIN, INT_MIN, INT_MIN);
	m_3DModel.m_minV = my::Vector3(INT_MAX, INT_MAX, INT_MAX);
	UpdataModelMatrix();
}


C3DSModel::~C3DSModel()
{
	m_3DModel.pMaterials.swap(std::vector<my::tMaterial>());
	m_3DModel.pObject.swap(std::vector<my::t3DObject>());
}

bool C3DSModel::Load3DS(CString filename)
{
	int nLength = filename.GetLength();
	int nBytes = WideCharToMultiByte(CP_ACP, 0, filename, nLength, NULL, 0, NULL, NULL);
	char* cFileName = new char[nBytes + 1];
	memset(cFileName, 0, nLength + 1);//CP_OEMCP
	WideCharToMultiByte(CP_ACP, 0, filename, nLength, cFileName, nBytes, NULL, NULL);
	
	if (nullptr == (m_FilePtr = fopen(cFileName, "rb")))
	{
		CString str("3DS文件");
		str = str + filename + _T("不存在!");
		AfxMessageBox(str);
		return false;
	}

// 	std::auto_ptr<char> cFileName(new char[nBytes + 1]);
// 	memset(cFileName.get(), 0, nLength + 1);
// 	WideCharToMultiByte(CP_OEMCP, 0, filename, nLength, cFileName.get(), nBytes, NULL, NULL);
// 
// 	if (nullptr == (m_FilePtr = fopen(cFileName.get(), "rb")))
// 	{
// 		CString str("3DS文件");
// 		str = str + filename + _T("不存在!");
// 		AfxMessageBox(str);
// 		return false;
// 	}

	my::tChunk chunk = { 0 };
	if(!ReadChunk(&chunk))
		return false;

	if (chunk.ID != my::PRIMARY)
	{
		CString str("不是3DS文件");
		str = filename + str;
		AfxMessageBox(str);
		return false;
	}
	
	// 开始读取3ds文件
	ReadPrimary(chunk.length - 6);

	// 计算每个顶点的法线量
	ComputeNormals();
	// 关闭打开的文件
	fclose(m_FilePtr);
	m_FilePtr = nullptr;

	//LPWSTR szFullPath = new WCHAR(_MAX_PATH);
 	char fname[_MAX_PATH];
 	//char szFullPath[_MAX_PATH];
 	char szDir[_MAX_PATH];
 	char szDrive[_MAX_PATH];
 	//::GetModuleFileName(NULL, szFullPath, _MAX_PATH);
	_splitpath(cFileName, szDrive, szDir, NULL, NULL);
 	bool IsMaterialsLoad = true;
 	// 对有纹理的材质载入该纹理
 	// 遍历所有的材质
 	for (int i = 0; i < m_3DModel.numOfMaterials; i++)
 	{
 		if (m_3DModel.pMaterials[i].isTexMat)
 		{
 			strcpy(fname, szDrive);
 			strcat(fname, szDir);
 			strcat(fname, m_3DModel.pMaterials[i].mapName.string);
 
			if (!BuildTexture(cFileName, m_3DModel.pMaterials[i].texureId, 255, RGB(0, 0, 0)))
 			{
 				// 纹理载入失败
 				// 				sprintf(strMessage, "3DS纹理文件载入失败: %s ！", m_3DModel.pMaterials[i].mapName.string);
 				// 				MessageBox(NULL, strMessage, "Error", MB_OK);
 				// 				return false;
 				IsMaterialsLoad = FALSE;
 			}
 		}
 	}
// 	if (!IsMaterialsLoad)
// 		AfxMessageBox(_T("3DS模型纹理载入失败或仅载入部分纹理!"));

	//计算包围球的球心
	m_3DModel.m_vCenter.x = (m_3DModel.m_maxV.x + m_3DModel.m_minV.x) / 2;
	m_3DModel.m_vCenter.y = (m_3DModel.m_maxV.y + m_3DModel.m_minV.y) / 2;
	m_3DModel.m_vCenter.z = (m_3DModel.m_maxV.z + m_3DModel.m_minV.z) / 2;

	//计算包围球的半径
	int maxx = max(m_3DModel.m_maxV.x, fabs(m_3DModel.m_minV.x));
	int maxy = max(m_3DModel.m_maxV.y, fabs(m_3DModel.m_minV.y));
	int maxz = max(m_3DModel.m_maxV.z, fabs(m_3DModel.m_minV.z));
	m_3DModel.m_Rais = sqrt((maxx - m_3DModel.m_vCenter.x)*(maxx - m_3DModel.m_vCenter.x)
							+ (maxy - m_3DModel.m_vCenter.y)*(maxy - m_3DModel.m_vCenter.y)
							+ (maxz - m_3DModel.m_vCenter.z)*(maxz - m_3DModel.m_vCenter.z));

	//AfxMessageBox(_T("3DS加载成功!"));
	//CalModelFaceVert();
	m_isLoad = true;
	return true;
}

bool C3DSModel::ReadChunk(my::tChunk *pChunk)
{
	if (nullptr == pChunk || nullptr == m_FilePtr)
		return false;

	fread(&pChunk->ID, 1, 2, m_FilePtr);
	fread(&pChunk->length, 1, 4, m_FilePtr);

	return true;
}

UINT C3DSModel::ReadPrimary(UINT n)
{
	UINT count = 0;				// 该Chunk内容已读取的字节计数
	my::tChunk chunk = { 0 };			// 用以保存子Chunk的内容
	while (count < n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case my::PRIM_EDIT:
			ReadEdit(chunk.length - 6);
			break;
			//case PRIM_KEY:
			//	ReadKeyframe(chunk.length-6);
			//	break;
		default:
			fseek(m_FilePtr, chunk.length - 6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	return count;
}

UINT C3DSModel::ReadEdit(UINT n)
{
	UINT count = 0;
	my::tChunk chunk = { 0 };
	while (count < n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case my::EDIT_MAT:
			ReadMaterial(chunk.length - 6);
			break;
		case my::EDIT_OBJECT:
			ReadObject(chunk.length - 6);
			break;
		default:
			fseek(m_FilePtr, chunk.length - 6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	return count;
}

// 读取材质
UINT C3DSModel::ReadMaterial(UINT n)
{
	UINT count = 0;
	my::tChunk chunk = { 0 };
	// 新的材质
	my::tMaterial newMaterial = { 0 };
	m_3DModel.numOfMaterials++;
	while (count < n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case my::MAT_NAME:
			ReadString(&newMaterial.matName);
			break;
		case my::MAT_DIF:
			ReadMatDif(&newMaterial, chunk.length - 6);
			break;
		case my::MAT_MAP:
			ReadMatMap(&newMaterial, chunk.length - 6);
			break;
		default:
			fseek(m_FilePtr, chunk.length - 6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	// 保存新的材质
	m_3DModel.pMaterials.push_back(newMaterial);
	return count;
}

// 读取3ds对象
UINT C3DSModel::ReadObject(UINT n)
{
	UINT count = 0;
	my::tChunk chunk = { 0 };
	// 新的3ds对象
	my::t3DObject newObject = { 0 };
	count += ReadString(&newObject.objName);
	m_3DModel.numOfObjects++;

	while (count < n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case my::OBJECT_INFO:
			ReadObjectInfo(&newObject, n - count - 6);
			break;
		default:
			fseek(m_FilePtr, chunk.length - 6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	// 保存3ds对象
	m_3DModel.pObject.push_back(newObject);
	return count;
}

// 从文件中读取1个字节
BYTE C3DSModel::ReadByte(void)
{
	BYTE result = 0;
	fread(&result, 1, 1, m_FilePtr);
	return result;
}

WORD C3DSModel::ReadWord(void)
{
	return ReadByte() + (ReadByte() << 8);
}

UINT C3DSModel::ReadUint(void)
{
	return ReadWord() + (ReadWord() << 16);
}

float C3DSModel::ReadFloat(void)
{
	float result;
	fread(&result, sizeof(float), 1, m_FilePtr);
	return result;
}

// 从文件中读取字符串（返回字符串长度）
UINT C3DSModel::ReadString(my::STRING *pStr)
{
	int n = 0;
	while ((pStr->string[n++] = ReadByte()) != 0)
		;
	return n;
}

UINT C3DSModel::ReadObjectInfo(my::t3DObject *pObj, UINT n)
{//ccs to do
	UINT count = 0;
	my::tChunk chunk = { 0 };
	int i;

	while (count < n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case my::OBJECT_VERTEX:
			pObj->numOfVerts = ReadWord();
			pObj->pVerts = new my::Vector3[pObj->numOfVerts];
			memset(pObj->pVerts, 0, sizeof(my::Vector3) * pObj->numOfVerts);

			// 按块读取顶点坐标值
			fread(pObj->pVerts, 1, chunk.length - 8, m_FilePtr);

			// 调换y、z坐标值(由于3dMAX坐标系方向与OpenGL不同，3DMax的z轴是指向上的。)
			float fTempY;
			for (i = 0; i < pObj->numOfVerts; i++)
			{
				pObj->pVerts[i].x *= my::meter2Inch;
				pObj->pVerts[i].y *= my::meter2Inch;
				pObj->pVerts[i].z *= my::meter2Inch;

				fTempY = pObj->pVerts[i].y;
				pObj->pVerts[i].y = pObj->pVerts[i].z;
				pObj->pVerts[i].z = -fTempY;

				//保存最大最小值
				if (pObj->pVerts[i].x > m_3DModel.m_maxV.x)
				{
					m_3DModel.m_maxV.x = pObj->pVerts[i].x;
				}
				else if (pObj->pVerts[i].x < m_3DModel.m_minV.x)
				{
					m_3DModel.m_minV.x = pObj->pVerts[i].x;
				}

				if (pObj->pVerts[i].y > m_3DModel.m_maxV.y)
				{
					m_3DModel.m_maxV.y = pObj->pVerts[i].y;
				}
				else if (pObj->pVerts[i].y < m_3DModel.m_minV.y)
				{
					m_3DModel.m_minV.y = pObj->pVerts[i].y;
				}

				if (pObj->pVerts[i].z > m_3DModel.m_maxV.z)
				{
					m_3DModel.m_maxV.z = pObj->pVerts[i].z;
				}
				else if (pObj->pVerts[i].z < m_3DModel.m_minV.z)
				{
					m_3DModel.m_minV.z = pObj->pVerts[i].z;
				}
			}

			break;
		case my::OBJECT_FACET:
			ReadFacetInfo(pObj, chunk.length - 6);
			break;
		case my::OBJECT_UV:
			pObj->numTexVertex = ReadWord();
			pObj->pTexVerts = new my::Vector2[pObj->numTexVertex];
			memset(pObj->pTexVerts, 0, sizeof(my::Vector2) * pObj->numTexVertex);
			// 按块读取纹理坐标值
			fread(pObj->pTexVerts, 1, chunk.length - 8, m_FilePtr);
			break;
		default:
			fseek(m_FilePtr, chunk.length - 6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	return count;
}

UINT C3DSModel::ReadFacetInfo(my::t3DObject *pObj, UINT n)
{
	int i;
	UINT count = 0;
	my::tChunk chunk = { 0 };
	pObj->numOfFaces = ReadWord();
	pObj->pFaces = new my::tFace[pObj->numOfFaces];
	memset(pObj->pFaces, 0, sizeof(my::tFace) * pObj->numOfFaces);
	// 读取面索引值(第4个值为3dMAX使用的参数，舍弃)
	for (i = 0; i < pObj->numOfFaces; i++)
	{
		pObj->pFaces[i].vertIndex[0] = ReadWord();
		pObj->pFaces[i].vertIndex[1] = ReadWord();
		pObj->pFaces[i].vertIndex[2] = ReadWord();
		ReadWord();
	}
	count += 2 + pObj->numOfFaces * 8;

	my::STRING matName;
	int t;
	int matID = 0;
	while (count < n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case my::FACET_MAT:
			ReadString(&matName);			// 材质名称
			t = ReadWord();					// 材质对应的面个数
			// 查找对应的材质
			for (i = 0; i < m_3DModel.numOfMaterials; i++)//????????
			{
				if (strcmp(matName.string, m_3DModel.pMaterials[i].matName.string) == 0)
				{
					matID = i;
					break;
				}
			}
			// 依据面索引给每个面绑定材质ID
			while (t > 0)
			{
				pObj->pFaces[ReadWord()].matID = matID;
				t--;
			}
			break;
		default:
			fseek(m_FilePtr, chunk.length - 6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	return count;
}

UINT C3DSModel::ReadMatDif(my::tMaterial *pMat, UINT n)
{
	UINT count = 0;
	my::tChunk chunk = { 0 };
	while (count < n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case my::COLOR_BYTE:
			pMat->color[0] = ReadByte();
			pMat->color[1] = ReadByte();
			pMat->color[2] = ReadByte();
			break;
		default:
			fseek(m_FilePtr, chunk.length - 6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	return count;
}

UINT C3DSModel::ReadMatMap(my::tMaterial *pMat, UINT n)
{
	UINT count = 0;
	my::tChunk chunk = { 0 };
	while (count < n)
	{
		ReadChunk(&chunk);
		switch (chunk.ID)
		{
		case my::MAP_NAME:
			ReadString(&pMat->mapName);
			pMat->isTexMat = true;
			break;
		default:
			fseek(m_FilePtr, chunk.length - 6, SEEK_CUR);
			break;
		}
		count += chunk.length;
	}
	return count;
}

void C3DSModel::Release(void)
{
	while (m_3DModel.pMaterials.size() != 0)
		m_3DModel.pMaterials.pop_back();

	for (int nOfObj = 0; nOfObj < m_3DModel.numOfObjects; nOfObj++)
	{
		delete[] m_3DModel.pObject[nOfObj].pFaces;
		delete[] m_3DModel.pObject[nOfObj].pVerts;
		delete[] m_3DModel.pObject[nOfObj].pTexVerts;
		delete[] m_3DModel.pObject[nOfObj].pNormals;
	}
	m_3DModel.pObject.clear();

	m_3DModel.numOfMaterials = 0;
	m_3DModel.numOfObjects = 0;
	glDeleteLists(DisplayList, 1); //删除显示列表，释放资源
}

bool C3DSModel::IsLoad()
{
// 	if (m_3DModel.pObject.empty() || m_3DModel.pMaterials.empty())
// 	{
// 		return true;
// 	}
	return m_isLoad;
}

my::Vector3 C3DSModel::Cross(my::Vector3 v1, my::Vector3 v2)
{
	my::Vector3 vCross;

	vCross.x = ((v1.y * v2.z) - (v1.z * v2.y));
	vCross.y = ((v1.z * v2.x) - (v1.x * v2.z));
	vCross.z = ((v1.x * v2.y) - (v1.y * v2.x));

	return vCross;
}

my::Vector3 C3DSModel::Normalize(my::Vector3 vNormal)
{
	double Magnitude;

	Magnitude = sqrt(vNormal.x*vNormal.x + vNormal.y*vNormal.y + vNormal.z*vNormal.z);
	vNormal = vNormal / (float)Magnitude;

	return vNormal;
}

void C3DSModel::ComputeNormals(void)
{
	my::Vector3 v1, v2, vNormal, vPoly[3];

	// 如果没有3ds对象则直接返回
	if (m_3DModel.numOfObjects <= 0)
		return;

	my::t3DObject *obj;
	int		  *index;

	for (int nOfObj = 0; nOfObj < m_3DModel.numOfObjects; nOfObj++)
	{
		obj = &m_3DModel.pObject[nOfObj];
		my::Vector3 *pNormals = new my::Vector3[obj->numOfFaces];
		my::Vector3 *pTempNormals = new my::Vector3[obj->numOfFaces];
		obj->pNormals = new my::Vector3[obj->numOfVerts];

		for (int nOfFace = 0; nOfFace < obj->numOfFaces; nOfFace++)
		{
			index = obj->pFaces[nOfFace].vertIndex;
			// 三角形的3个顶点
			vPoly[0] = obj->pVerts[index[0]];
			vPoly[1] = obj->pVerts[index[1]];
			vPoly[2] = obj->pVerts[index[2]];
			// 计算这个三角形的法线量
			v1 = vPoly[0] - vPoly[1];
			v2 = vPoly[2] - vPoly[1];
			vNormal = Cross(v1, v2);

			pTempNormals[nOfFace] = vNormal;					// 保存未单位化的法向量
			vNormal = Normalize(vNormal);						// 单位化法向量
			pNormals[nOfFace] = vNormal;						// 增加到法向量数组列表
		}
		my::Vector3 vSum(0.0, 0.0, 0.0);
		my::Vector3 vZero(0.0, 0.0, 0.0);
		int shared = 0;

		for (int nOfVert = 0; nOfVert < obj->numOfVerts; nOfVert++)			// 遍历所有顶点
		{
			for (int nOfFace = 0; nOfFace < obj->numOfFaces; nOfFace++)		// 遍历包含该顶点的面
			{
				if (obj->pFaces[nOfFace].vertIndex[0] == nOfVert ||
					obj->pFaces[nOfFace].vertIndex[1] == nOfVert ||
					obj->pFaces[nOfFace].vertIndex[2] == nOfVert)
				{
					vSum = vSum + pTempNormals[nOfFace];
					shared++;
				}
			}

			obj->pNormals[nOfVert] = vSum / float(-shared);//注意：这里加了负号，相当于v1 = vPoly[0]-vPoly[1]; v2 = vPoly[1]-vPoly[2]。

			obj->pNormals[nOfVert] = Normalize(obj->pNormals[nOfVert]);

			vSum = vZero;
			shared = 0;
		}

		delete[] pTempNormals;
		delete[] pNormals;
	}
}

void C3DSModel::Render(void)
{
	glPushMatrix();
	if (0 != DisplayList)
	{
		glCallList(DisplayList);//直接调用显示列表
		return;
	}
	
	DisplayList = glGenLists(1);
	glNewList(DisplayList, GL_COMPILE);//创建一个显示列表，mygllist为显示列表编号

	my::tMaterial *mat = nullptr;
	my::t3DObject *obj = nullptr;
	int		  *index = nullptr;
	for (int nOfObj = 0; nOfObj < m_3DModel.numOfObjects; nOfObj++)
	{
		obj = &m_3DModel.pObject[nOfObj];
		for (int nOfFace = 0; nOfFace < obj->numOfFaces; nOfFace++)
		{
			index = obj->pFaces[nOfFace].vertIndex;
			if (!m_3DModel.pMaterials.empty())
				mat = &m_3DModel.pMaterials[obj->pFaces[nOfFace].matID];

			if (nullptr != mat && mat->isTexMat)				// 如果面对应的材质具有纹理
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, mat->texureId);		// 选择该材质的纹理

				glColor3ub(mat->color[0], mat->color[1], mat->color[2]);
				// 绘制三角形面
				glBegin(GL_TRIANGLES);
				glTexCoord2f(obj->pTexVerts[index[0]].x, obj->pTexVerts[index[0]].y);
				glNormal3f(obj->pNormals[index[0]].x, obj->pNormals[index[0]].y, obj->pNormals[index[0]].z);
				glVertex3f(obj->pVerts[index[0]].x, obj->pVerts[index[0]].y, obj->pVerts[index[0]].z);

				glTexCoord2f(obj->pTexVerts[index[1]].x, obj->pTexVerts[index[1]].y);
				glNormal3f(obj->pNormals[index[1]].x, obj->pNormals[index[1]].y, obj->pNormals[index[1]].z);
				glVertex3f(obj->pVerts[index[1]].x, obj->pVerts[index[1]].y, obj->pVerts[index[1]].z);

				glTexCoord2f(obj->pTexVerts[index[2]].x, obj->pTexVerts[index[2]].y);
				glNormal3f(obj->pNormals[index[2]].x, obj->pNormals[index[2]].y, obj->pNormals[index[2]].z);
				glVertex3f(obj->pVerts[index[2]].x, obj->pVerts[index[2]].y, obj->pVerts[index[2]].z);
				glEnd();
			}
			else							// 如果面对应的材质没有纹理
			{
				//glDisable(GL_TEXTURE_2D);
				//glColor3ub(mat->color[0], mat->color[1], mat->color[2]);
				// 绘制三角形面
				glBegin(GL_TRIANGLES);
				glNormal3f(obj->pNormals[index[0]].x, obj->pNormals[index[0]].y, obj->pNormals[index[0]].z);
				glVertex3f(obj->pVerts[index[0]].x, obj->pVerts[index[0]].y, obj->pVerts[index[0]].z);

				glNormal3f(obj->pNormals[index[1]].x, obj->pNormals[index[1]].y, obj->pNormals[index[1]].z);
				glVertex3f(obj->pVerts[index[1]].x, obj->pVerts[index[1]].y, obj->pVerts[index[1]].z);

				glNormal3f(obj->pNormals[index[2]].x, obj->pNormals[index[2]].y, obj->pNormals[index[2]].z);
				glVertex3f(obj->pVerts[index[2]].x, obj->pVerts[index[2]].y, obj->pVerts[index[2]].z);
				glEnd();
			}
		}
	}
	glEndList();
	glPopMatrix();
}

bool C3DSModel::RayHitSphere( my::Ray &ray)
{
	//获取球心坐标以及包围球半径
	my::Vector3 center = GetCenterPoint();//SphereCenter;
	float radius = GetRais();// SphereR;

	my::Vector3 end;
	end.x = ray.origin.x + ray.direction.x;
	end.y = ray.origin.y + ray.direction.y;
	end.z = ray.origin.z + ray.direction.z;
// 	if (std::fabs(end.x - center.x) < radius && std::fabs(end.y - center.y) < radius &&std::fabs(end.z - center.z) < radius)
// 	{
// 		return true;
// 	}

	float a, b, c, d;
	if (!GetLineABCD(ray, a, b, c, d))
		return false;

	float left = a * center.x + b * center.y + c * center.z + d;
	float right = (a * a + b * b + c * c) * radius * radius;
	return left * left < right ? true : false;
	

	////获取球心坐标以及包围球半径
	//my::Vector3 spCenter = SphereCenter;
	//float radius = SphereR;
	//my::Vector3 rayorig;

	//rayorig.x = ray.origin.x - spCenter.x;
	//rayorig.y = ray.origin.y - spCenter.y;
	//rayorig.z = ray.origin.z - spCenter.z;
	//my::Vector3 raydir = ray.direction;
	////单位化射线方向向量
	//raydir.Normalize();

	//float a = raydir.Dot(raydir);
	//float b = 2 * rayorig.Dot(raydir);
	//float c = rayorig.Dot(rayorig) - radius * radius;
	//float d = (b*b) - (4 * c);

	//if (d < 0) //B*B-4C<0 无解
	//{
	//	return false;
	//}
	//else
	//{
	//	float t0 = (-b - std::sqrt(d)) / 2;
	//	float t1 = (-b + std::sqrt(d)) / 2;

	//	if ((t1 < 0) && (t0 < 0))//两个解都小于零，则出错
	//	{
	//		return false;
	//	}
	//	else
	//	{
	//		return true;
	//	}
	//}

	//return true;
}

void C3DSModel::MatrixMulti(float left[], float right[], int rowLeft, int colLeft, int colRight, float res[])
{
	int index = 0;
	for (int row = 0; row < rowLeft; ++row)
	{
		for (int col = 0; col < colRight;++col)
		{
			index = row * colRight + col;
			res[index] = 0;
			for (int i = 0; i < colLeft;++i)
			{
				res[index] = res[index] + left[row * colLeft + i] * right[i * colRight + col];
			}
		}
	}
}

float C3DSModel::GetRais()
{
	float maxScale;
	maxScale = m_transPara.scaleX > m_transPara.scaleY ? m_transPara.scaleX : m_transPara.scaleY;
	maxScale = maxScale > m_transPara.scaleZ ? maxScale : m_transPara.scaleZ;

	return m_3DModel.m_Rais * maxScale;
}

my::Vector3 C3DSModel::GetCenterPoint()
{
	my::Vector3 center;

	float before[4] = { m_3DModel.m_vCenter.x, m_3DModel.m_vCenter.y, m_3DModel.m_vCenter.z, 1 };
	float after[4];
	//对每个顶点进行变换，使用按行存储的变换矩阵
	ChangeCenterXYZ(before, after);

	center.x = after[0];//光线求交过程中所有的单位均转为米
	center.y = after[1];
	center.z = after[2];

	return center;
}

void C3DSModel::ChangeCenterXYZ(float before[], float after[])
{
	//不需要缩放、旋转，只平移
// 	//旋转
// 	float tmpZ[4];
// 	MatrixMulti(before, m_modelRotateZMatrix, 1, 4, 4, tmpZ);
// 
// 	float tmpY[4];
// 	MatrixMulti(tmpZ, m_modelRotateYMatrix, 1, 4, 4, tmpY);
// 
// 	float tmpX[4];
// 	MatrixMulti(tmpY, m_modelRotateXMatrix, 1, 4, 4, tmpX);

	//平移
	MatrixMulti(before, m_modelTransMatrix, 1, 4, 4, after);
}

bool C3DSModel::GetLineABCD(const my::Ray &ray, float &A, float &B, float &C, float &D)
{
	//(x - a) / m = (y - b) / n = (z - c) / k
	float a = ray.origin.x;
	float b = ray.origin.y;
	float c = ray.origin.z;
	float m = ray.direction.x;
	float n = ray.direction.y;
	float k = ray.direction.z;

	if (std::fabs(m) < 1e-3)
	{
		A = 0;
		B = k;
		C = -n;
		D = n * c - k * b;
	}
	else if (std::fabs(n) < 1e-3)
	{
		A = k;
		B = 0;
		C = -m;
		D = m * c - k * a;
	}
	else if (std::fabs(k) < 1e-3)
	{
		A = n;
		B = -m;
		C = 0;
		D = m * b - n * a;
	}
	else if (k != m)
	{
		A = n;
		B = k - m;
		C = -n;
		D = (m - k) * b + n * (c - a);
	}
	else
	{
		A = n;
		B = 2 * k - m;
		C = -2 * n;
		D = (m - 2 * k) * b + n * (2 * c - a);
	}
	return true;
}

my::Vector3  C3DSModel::GetLux(my::t3DObject *obj,int nFace, my::Vector3 Crossdot, my::Vector3 lightP)
{
	
	BYTE *color;
	int *index;
	my::Vector3 Material,v1,v2,vPoly[3],vNormal,Direcion,Lux,lightcolor(1.0,1.0,1.0),ambient(0,0,0),temp;
	color = m_3DModel.pMaterials[obj->pFaces[nFace].matID].color;
	Material.GetVector3(color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f);
	/*Material.GetVector3(60 / 255.0f, 80 / 255.0f, 90 / 255.0f);*/
	index = obj->pFaces[nFace].vertIndex;
	// 三角形的3个顶点
	vPoly[0] = obj->pVerts[index[0]];
	vPoly[1] = obj->pVerts[index[1]];
	vPoly[2] = obj->pVerts[index[2]];
	// 计算这个三角形的法线量
	v1 = vPoly[0] - vPoly[1];
	v2 = vPoly[2] - vPoly[1];
	vNormal = Cross(v1, v2);
	vNormal.Normalize();						// 单位化法向量

	Direcion = lightP - Crossdot;
	Direcion.Normalize();
	temp.GetVector3(Material.x*ambient.x, Material.y*ambient.y, Material.z*ambient.z);
	Lux.GetVector3(Material.x*lightcolor.x*vNormal.Dot(Direcion), Material.y*lightcolor.y*vNormal.Dot(Direcion), Material.z*lightcolor.z*vNormal.Dot(Direcion));
	Lux = Lux + temp;
	return Lux;
}

bool C3DSModel::RayRealHitModel(my::Ray &ray, my::Vector3 &vecHit, float &dist,my::Vector3 &Lux, my::Vector3 LightP)
{
	float a, b, c, d;
	if (!GetLineABCD(ray, a, b, c, d))
		return false;

	bool hitModel = false;
	float distTmp;
	my::Vector3 vertTmp;
	int *pfaceVert = nullptr;
	my::t3DObject* obj = nullptr;
	my::Vector3 triFaceVerts[3];
	for (int i = 0; i < m_3DModel.numOfObjects; i++)
	{
		obj = &m_3DModel.pObject[i];
		for (int j = 0; j < obj->numOfFaces; j++)
		{
			pfaceVert = obj->pFaces[j].vertIndex;
			for (int k = 0; k < 3; k++)
			{
				triFaceVerts[k] = obj->pVerts[pfaceVert[k]];
			}
			if (RayHitTriFace(ray, triFaceVerts, vertTmp, distTmp))
			{
				hitModel = true;
				distTmp = (ray.origin - vertTmp).GetLength();
				
				if (distTmp < dist)
				{
					vecHit = vertTmp;
					dist = distTmp;
					Lux = GetLux(obj, j, vertTmp,LightP);
				}
			}
		}
	}


	return hitModel;
}

/*
bool C3DSModel::RayRealHitModel(my::Ray &ray, my::Vector3* vecHit, float* dist)
{
	//计算射线与其在地面上投影两条直线所构成的平面的法向量，该法向量的z=0。
	//平面方程Ax+By+Cz+D=0;其中(A,B,C)为平面法向量
	float A1, B1, D1;
	float C1 = 0.0f;
	float E1 = sqrt(ray.direction.x * ray.direction.x + ray.direction.y * ray.direction.y);
	A1 = -1.0 * ray.direction.y / E1;
	B1 = ray.direction.x / E1;
	D1 = -1.0 * (A1 * ray.origin.x + B1 * ray.origin.y + C1 * ray.origin.z);

	double B2, C2, D2;
	B2 = -1.0 * A1 / B1;
	C2 = -1 * (ray.direction.x + B2 * ray.direction.y) / ray.direction.z;
	D2 = -1 * (ray.origin.x + B2 * ray.origin.y + C2 * ray.origin.z);

	my::t3DObject* obj = nullptr;
	float temdist = *dist = -1.0f;
	float u, v, tu, tv;
	bool  bhit = false;
	my::Vector3   temvhit;
	my::Vector3   triVertsNew[3];//经过坐标变换后的顶点坐标

	int num = 0;
	int findvert = 0;
	for (int i = 0; i < m_3DModel.numOfObjects; i++)
	{
		obj = &m_3DModel.pObject[i];
		for (int j = 0; j < obj->numOfFaces; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				if (!GetModelFaceVert(num, triVertsNew[k]))
				{
					AfxMessageBox(_T("获取三角面点失败！"));
					return false;
				}
				num++;
			}
			if (TriangleHitFace(A1, B1, C1, D1, triVertsNew) && TriangleHitFace(1, B2, C2, D2, triVertsNew))
			{
				// 检测拾取线是否与三角行相交
				if (RayHitTriangle(ray, triVertsNew, temdist, tu, tv))
				{
					if (dist)
					{
						if ((*dist == -1.0) || (temdist < *dist))
						{
							// 确保与该射线相交的面为最近的面
							if (dist)
							{
								*dist = temdist;
							}
							// 							iface = j;
							// 							iObj = i;
							findvert = num - 1;

							u = tu;
							v = tv;

							if (!bhit)
							{
								bhit = true;
							}
						}
					}
					else // 如果只检测是否碰撞则返回
						return true;
				}

			}
		}
	}

	if (!bhit)
		return false;

	// 求交点，注意坐标转换
	if (vecHit)// 如果需要求交点  
	{
		my::Vector3 vi0, vi1, vi2;
		if (!GetModelFaceVert(findvert - 2, vi0) || !GetModelFaceVert(findvert - 2, vi0) || !GetModelFaceVert(findvert - 2, vi0))
		{
			return false;
		}
		*vecHit = vi0 + (vi1 - vi0) * u + (vi2 - vi0) * v;
	}

	return true;
}
*/
void C3DSModel::CalModelFaceVert()
{
	int VertNum = 0;
	int num = 0;
	my::t3DObject* obj = nullptr;
	int *index;
	for (int i = 0; i < m_3DModel.numOfObjects; i++)
	{
		//	obj = &m_pActModel->m_3dsModel.m_3DModel.pObject[i];
		obj = &m_3DModel.pObject[i];
		for (int j = 0; j < obj->numOfFaces; j++)
		{
			VertNum++;
		}
	}
	VertNum = VertNum * 3;
	m_modelFaceVert.resize(VertNum);

	for (int i1 = 0; i1 < m_3DModel.numOfObjects; i1++)
	{
		obj = &m_3DModel.pObject[i1];
		for (int j1 = 0; j1 < obj->numOfFaces; j1++)
		{
			index = obj->pFaces[j1].vertIndex;
			// 获取三角形的3个顶点
			for (int k = 0; k < 3; k++)
			{
				m_modelFaceVert[num] = obj->pVerts[index[k]];// TransformVert(obj->pVerts[index[k]]);
				num++;
			}
		}
	}
}

my::Vector3 C3DSModel::TransformVert(const my::Vector3& fPointSrc)
{
	my::Vector3 fPointDst;

	//float TempVector[4];
	//float m[4] = { fPointSrc.x, fPointSrc.y, fPointSrc.z, 1 };
	//MatrixMulti((float*)m, (float*)m_modelChangeMatrix, 1, 4, 4, (float*)TempVector);


 //	fPointDst.x = TempVector[0];  //光线求交过程中所有的单位均为米
 //	fPointDst.y = TempVector[1];
 //	fPointDst.z = TempVector[2];


	my::Vector3 center = GetCenterPoint();
	float m[4] = { fPointSrc.x - center.x, fPointSrc.y - center.y, fPointSrc.z - center.z, 1 };
	float rotateZ[4], rotateY[4], rotateX[4], scale[4];
	MatrixMulti(m, m_modelRotateZMatrix, 1, 4, 4, rotateZ);
	MatrixMulti(rotateZ, m_modelRotateYMatrix, 1, 4, 4, rotateY);
	MatrixMulti(rotateY, m_modelRotateZMatrix, 1, 4, 4, rotateX);
	MatrixMulti(rotateX, m_modelScaleMatrix, 1, 4, 4, scale);


	fPointDst.x = scale[0] + center.x;  //光线求交过程中所有的单位均为米
	fPointDst.y = scale[1] + center.y;
	fPointDst.z = scale[2] + center.z;

	return fPointDst;
}

void C3DSModel::UpdataModelMatrix()
{
	//(x, y, z, 1) = (x0, y0, z0, 1) * Matrix;
	//和师兄(x, y, z, 1) = Matrix * (x0, y0, z0, 1);不一样
	//缩放
	float scale[16] =
	{
		m_transPara.scaleX, 0, 0, 0,
		0, m_transPara.scaleY, 0, 0,
		0, 0, m_transPara.scaleZ, 0,
		0, 0, 0, 1
	};
	memcpy(&m_modelScaleMatrix, &scale, sizeof(scale));
	//旋转
	float cosTheta, sinTheta;
	cosTheta = std::cos(my::PI * m_transPara.d_z / 180);
	sinTheta = std::sin(my::PI * m_transPara.d_z / 180);
	float rotateZ[16] = 
	{ 
		cosTheta, sinTheta, 0, 0,
		-sinTheta, cosTheta, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	memcpy(&m_modelRotateZMatrix, &rotateZ, sizeof(rotateZ));

	cosTheta = std::cos(my::PI * m_transPara.d_y / 180);
	sinTheta = std::sin(my::PI * m_transPara.d_y / 180);
	float rotateY[16] = 
	{ 
		cosTheta, 0, sinTheta, 0,
		0, 1, 0, 0,
		-sinTheta, 0, cosTheta, 0,
		0, 0, 0, 1 
	};
	memcpy(&m_modelRotateYMatrix, &rotateY, sizeof(rotateY));

	cosTheta = std::cos(my::PI * m_transPara.d_x / 180);
	sinTheta = std::sin(my::PI * m_transPara.d_x / 180);
	float rotateX[16] = 
	{ 
		1, 0, 0, 0,
		0, cosTheta, sinTheta, 0,
		0, -sinTheta, cosTheta, 0,
		0, 0, 0, 1 
	};
	memcpy(&m_modelRotateXMatrix, &rotateX, sizeof(rotateX));

	//平移
	float trans[16] = 
	{ 
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		m_transPara.x, m_transPara.y, m_transPara.z, 1 
	};
	memcpy(&m_modelTransMatrix, &trans, sizeof(trans));

	//整体模型变换矩阵
	//(x, y, z, 1) = (x0, y0, z0, 1) * Matrix;
	//m_M = T * Rx * Ry * Rz * S
	//m_modelMatrix = S * Rz * Ry * Rx * T
	float TempVector0[16] = { 1, 2, 3, 4, 5, 6 };
	float TempVector1[16];
	float TempVector2[16];
	MatrixMulti((float*)m_modelScaleMatrix, (float*)m_modelRotateZMatrix, 4, 4, 4, (float*)TempVector0);
	MatrixMulti((float*)TempVector0, (float*)m_modelRotateYMatrix, 4, 4, 4, (float*)TempVector1);
	MatrixMulti((float*)TempVector1, (float*)m_modelRotateXMatrix, 4, 4, 4, (float*)TempVector2);
	MatrixMulti((float*)TempVector2, (float*)m_modelTransMatrix, 4, 4, 4, (float*)m_modelChangeMatrix);
}

void C3DSModel::SetTransPara(my::T3ds transPara)
{
	m_transPara = transPara;
	UpdataModelMatrix();
	//CalModelFaceVert();
}

bool C3DSModel::GetModelFaceVert(int vertID, my::Vector3 &vert)
{
	if (vertID < -1 || vertID >= (int)m_modelFaceVert.size())
		return false;

	vert = m_modelFaceVert[vertID];
	return true;
}

bool C3DSModel::TriangleHitFace(double A, double B, double C, double D, my::Vector3 triVerts[])
{
	//判断一个面是否与这个过射线的平面相交
	//Ax + By + Cz + D = 0
	float s[3] = { 0.0f };
	s[0] = A * triVerts[0].x + B * triVerts[0].y + C * triVerts[0].z + D;
	s[1] = A * triVerts[1].x + B * triVerts[1].y + C * triVerts[1].z + D;
	s[2] = A * triVerts[2].x + B * triVerts[2].y + C * triVerts[2].z + D;
	if (s[0] > 0 && s[1] > 0 && s[2] > 0 || s[0] < 0 && s[1] < 0 && s[2] < 0)
	{
		return false;
	}
	else
		return true;
}

bool C3DSModel::RayHitTriangle(const my::Ray &ray, my::Vector3 vtri[], float &dist, float &u, float &v)
{
	float inv_det;
	my::Vector3 vec1; //两条边的向量
	my::Vector3 vec2;
	my::Vector3 pvec;
	my::Vector3 tvec;
	my::Vector3 qvec;

	//算出两个边的向量
	vec1 = vtri[1] - vtri[0];
	vec2 = vtri[2] - vtri[0];

	pvec = ray.direction.Cross(vec2);

	// 如果det为0，或接近于零则射线与三角面共面或平行，不相交
	float det = vec1.Dot(pvec);

	if (std::fabs(det) < 0.0001f)  //注意注意，abs()参数为int，应该用fabs()
	{
		return false;
	}

	tvec.x = ray.origin.x - vtri[0].x;
	tvec.y = ray.origin.y - vtri[0].y;
	tvec.z = ray.origin.z - vtri[0].z;

	inv_det = 1.0 / det;

	// 计算u并测试是否合法（在三角形内）
	u = tvec.Dot(pvec) * inv_det;

	if (u < 0.0f || u > 1.0)
	{
		return false;
	}

	qvec = tvec.Cross(vec1);

	//计算v并测试是否合法（在三角形内）
	v = ray.direction.Dot(qvec) * inv_det;

	if (v < 0.0f || u + v > 1.0)
	{
		return false;
	}

	dist = vec2.Dot(qvec) * inv_det;

	return true;
}

bool C3DSModel::RayHitTriFace(const my::Ray &ray, my::Vector3 vtri[], my::Vector3& vecHit, float &dist)
{
	float U, V;
	if (!RayHitTriangle(ray, vtri, dist, U, V))
		return false;
	my::Vector3 vertTmp = vtri[0] * 1 + (vtri[1] - vtri[0]) * U + (vtri[2] - vtri[0]) * V;
	vecHit.x = vertTmp.x;
	vecHit.y = vertTmp.y;
	vecHit.z = vertTmp.z;
	return true;
}

bool BuildTexture(char *szPathName, GLuint &texid, int nAlpha, COLORREF clrTransparentKey) // / Load Image And Convert To A Texture
{
	HDC			hdcTemp;												// The DC To Hold Our Bitmap
	HBITMAP		hbmpTemp;												// Holds The Bitmap Temporarily
	IPicture	*pPicture;												// IPicture Interface
	OLECHAR		wszPath[MAX_PATH + 1];									// Full Path To Picture (WCHAR)
	char		szPath[MAX_PATH + 1];										// Full Path To Picture
	long		lWidth;													// Width In Logical Units
	long		lHeight;												// Height In Logical Units
	long		lWidthPixels;											// Width In Pixels
	long		lHeightPixels;											// Height In Pixels
	GLint		glMaxTexDim;											// Holds Maximum Texture Size

	/*
	if (strstr(szPathName, "http://"))									// If PathName Contains http:// Then...
	{
	strcpy(szPath, szPathName);										// Append The PathName To szPath
	}
	else																// Otherwise... We Are Loading From A File
	{
	GetCurrentDirectory(MAX_PATH, szPath);							// Get Our Working Directory
	strcat(szPath, "\\");											// Append "\" After The Working Directory
	strcat(szPath, szPathName);										// Append The PathName
	}
	*/
	strcpy(szPath, szPathName);										// Append The PathName

	MultiByteToWideChar(CP_ACP, 0, szPath, -1, wszPath, MAX_PATH);		// Convert From ASCII To Unicode
	HRESULT hr = OleLoadPicturePath(wszPath, 0, 0, 0, IID_IPicture, (void**)&pPicture);

	if (FAILED(hr))														// If Loading Failed
		return FALSE;													// Return False

	hdcTemp = CreateCompatibleDC(GetDC(0));								// Create The Windows Compatible Device Context
	if (!hdcTemp)														// Did Creation Fail?
	{
		pPicture->Release();											// Decrements IPicture Reference Count
		return FALSE;													// Return False (Failure)
	}

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glMaxTexDim);					// Get Maximum Texture Size Supported

	pPicture->get_Width(&lWidth);										// Get IPicture Width (Convert To Pixels)
	lWidthPixels = MulDiv(lWidth, GetDeviceCaps(hdcTemp, LOGPIXELSX), 2540);
	pPicture->get_Height(&lHeight);										// Get IPicture Height (Convert To Pixels)
	lHeightPixels = MulDiv(lHeight, GetDeviceCaps(hdcTemp, LOGPIXELSY), 2540);

	// Resize Image To Closest Power Of Two
	if (lWidthPixels <= glMaxTexDim) // Is Image Width Less Than Or Equal To Cards Limit
		lWidthPixels = 1 << (int)floor((log((double)lWidthPixels) / log(2.0f)) + 0.5f);
	else  // Otherwise  Set Width To "Max Power Of Two" That The Card Can Handle
		lWidthPixels = glMaxTexDim;

	if (lHeightPixels <= glMaxTexDim) // Is Image Height Greater Than Cards Limit
		lHeightPixels = 1 << (int)floor((log((double)lHeightPixels) / log(2.0f)) + 0.5f);
	else  // Otherwise  Set Height To "Max Power Of Two" That The Card Can Handle
		lHeightPixels = glMaxTexDim;

	//	Create A Temporary Bitmap
	BITMAPINFO	bi = { 0 };												// The Type Of Bitmap We Request
	DWORD		*pBits = 0;												// Pointer To The Bitmap Bits

	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);				// Set Structure Size
	bi.bmiHeader.biBitCount = 32;									// 32 Bit
	bi.bmiHeader.biWidth = lWidthPixels;							// Power Of Two Width
	bi.bmiHeader.biHeight = lHeightPixels;						// Make Image Top Up (Positive Y-Axis)
	bi.bmiHeader.biCompression = BI_RGB;								// RGB Encoding
	bi.bmiHeader.biPlanes = 1;									// 1 Bitplane

	//	Creating A Bitmap This Way Allows Us To Specify Color Depth And Gives Us Imediate Access To The Bits
	hbmpTemp = CreateDIBSection(hdcTemp, &bi, DIB_RGB_COLORS, (void**)&pBits, 0, 0);

	if (!hbmpTemp)														// Did Creation Fail?
	{
		DeleteDC(hdcTemp);												// Delete The Device Context
		pPicture->Release();											// Decrements IPicture Reference Count
		return FALSE;													// Return False (Failure)
	}

	SelectObject(hdcTemp, hbmpTemp);									// Select Handle To Our Temp DC And Our Temp Bitmap Object

	// Render The IPicture On To The Bitmap
	pPicture->Render(hdcTemp, 0, 0, lWidthPixels, lHeightPixels, 0, lHeight, lWidth, -lHeight, 0);

	// Convert From BGR To RGB Format And Add An Alpha Value Of 255
	for (long i = 0; i < lWidthPixels * lHeightPixels; i++)				// Loop Through All Of The Pixels
	{
		BYTE* pPixel = (BYTE*)(&pBits[i]);							// Grab The Current Pixel
		BYTE  temp = pPixel[0];									// Store 1st Color In Temp Variable (Blue)
		pPixel[0] = pPixel[2];									// Move Red Value To Correct Position (1st)
		pPixel[2] = temp;											// Move Temp Value To Correct Blue Position (3rd)

		// This Will Make Any Black Pixels, Completely Transparent		(You Can Hardcode The Value If You Wish)
		if ((pPixel[0] == GetRValue(clrTransparentKey)) &&
			(pPixel[1] == GetGValue(clrTransparentKey)) &&
			(pPixel[2] == GetBValue(clrTransparentKey)))			// Is Pixel Completely Black
			pPixel[3] = 0;											// Set The Alpha Value To 0
		else															// Otherwise
			pPixel[3] = nAlpha;											// Set The Alpha Value To 255
	}

	glGenTextures(1, &texid);											// Create The Texture

	// Typical Texture Generation Using Data From The Bitmap
	glBindTexture(GL_TEXTURE_2D, texid);								// Bind To The Texture ID
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, lWidthPixels, lHeightPixels, 0, GL_RGBA, GL_UNSIGNED_BYTE, pBits);	// (Modify This If You Want Mipmaps)

	DeleteObject(hbmpTemp);												// Delete The Object
	DeleteDC(hdcTemp);													// Delete The Device Context

	pPicture->Release();												// Decrements IPicture Reference Count

	return TRUE;														// Return True (All Good)
}