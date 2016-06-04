#pragma  once
#include <vector>
#include <string>
#include <cmath>
//using namespace std;
//3DS Related Structure 
#define BYTE unsigned char
#define WORD unsigned short 
#define UINT unsigned int
#define	 MAX_TEXTURE 100

// HDL64E-S2常量定义

// GeneralDefiniton
const float PI = 3.14;
const UINT LASER_NUM = 512;						// 64路激光
const float HORI_FOV = 30;                    // 水平视场角360°
const float HORI_AZIMUTH_RES = 0.06;           // 水平扫描分辨率0.06°

const float VERT_FOV = 30;                   // 垂直视场角33.2°
const float VERT_SCAN_RANGE_LOWBOUND = -24.33; // 垂直扫描下界-24.33°
const float VERT_SCAN_RANGE_UPBOUND = 2.0;       // 垂直扫描上界2°
const float VERT_SCAN_VERT_RES = 0.06;			//垂直扫描分辨率0.06°
												 // UpperBlock
const UINT UPPERBLOCK_LASER_NUM = 32;          // 上部模块有32路激光
const float UPPERBLOCK_LOWBOUND = -8.33;       // 上部模块垂直扫描下界-8.33°
const float UPPERBLOCK_UPBOUND = 2.0;          // 上部模块垂直扫描上界2°
const float UPPERBLOCK_VERT_RES = 0.333;       // 上部模块垂直扫描分辨率1/3°

											   // LowerBlock
const UINT LOWERBLOCK_LASER_NUM = 32;           // 下部模块有32路激光
const float LOWERBLOCK_LOWBOUND = -24.33;       // 下部模块垂直扫描下界
const float LOWERBLOCK_UPBOUND = -8.33;           // 下部模块垂直扫描上界
const float LOWERBLOCK_VERT_RES = 0.500;       // 下部模块垂直扫描分辨率1/2°

const UINT CAMERA_NUM = 5000;
const float CAMERA_HORI = 30;
const float HORI_CAMERA_RES = 0.006;
const float CAMERA_VERT = 30;
const float VERT_CAMERA_RES = 0.006;
namespace my
{ 
	//#define  BYTE unsigned char
	//#define  WORD unsigned short 
	//#define  UINT unsigned int

	//#define MAX_TEXTURES 100

	const float inch2Meter = 39.37;// 0.0254;
	const float meter2Inch = 0.0254;

	struct STRING
	{
		char string[128];
	};

	// 2维向量
	struct Vector2
	{
		float x, y;
		Vector2() :x(0), y(0){};
		Vector2(float x1,float y1) :x(x1), y(y1){};
	};

	// 3维向量
	struct Vector3
	{
	public:
		// 向量初始化
		Vector3():x(0),y(0),z(0) {}
		Vector3(float X, float Y, float Z)	{ x = X; y = Y; z = Z; }
		Vector3(const Vector3& v) :x(v.x), y(v.y), z(v.z){}

		// 向量相加
		Vector3 operator+(Vector3 &vVector)	{ return Vector3(x + vVector.x, y + vVector.y, z + vVector.z); }
		// 向量相加
		Vector3 operator-(Vector3 &vVector)	{ return Vector3(x - vVector.x, y - vVector.y, z - vVector.z); }
		// 向量点乘
		Vector3 operator*(float num)		{ return Vector3(x * num, y * num, z * num); }
		Vector3 operator/(float num)		{ return Vector3(x / num, y / num, z / num); }
		Vector3 operator=(const Vector3& v)		{ x = v.x; y = v.y; z = v.z; return *this; }

		// 计算向量的叉乘
		Vector3 Cross(const Vector3 &rhs) const { return Vector3(y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x); }

		// 计算向量的点乘
		float Dot(const Vector3 &rhs) const { return x*rhs.x + y*rhs.y + z*rhs.z; }

		//计算向量长度
		float GetLength() const { return (float)sqrt((x*x) + (y*y) + (z*z)); }

		// 计算向量的长度的平方
		float GetSquaredLength() const { return (x*x) + (y*y) + (z*z); }

		void GetVector3(float X, float Y, float Z) { x = X; y = Y; z = Z; }

		Vector3 GetVector(float X, float Y, float Z) const { return Vector3(X, Y, Z); }

		//单位化向量
		void Normalize()
		{
			float length = GetLength();

			if (length == 1 || length == 0)
				return;

			float scalefactor = 1.0f / length;
			x *= scalefactor;
			y *= scalefactor;
			z *= scalefactor;
		}
		float x, y, z;
	};

	// 保存Chunk信息
	struct tChunk
	{
		WORD ID;										// Chunk的ID
		UINT length;									// Chunk的长度
/*		tChunk() :ID(0), length(0){};*/
	};

	// 定义3ds的一些有使用到的ChunkID
	// 根Chunk,在每个文件的开始位置                                                                                                                                                                                                                                                                                                                                                                                                                                     
	const WORD PRIMARY = 0x4D4D;
	const WORD PRIM_EDIT = 0x3D3D;						// ChunkID：3ds模型
	const WORD EDIT_MAT = 0xAFFF;						// ChunkID：材质
	const WORD MAT_NAME = 0xA000;						// ChunkID：材质名称
	const WORD MAT_AMB = 0xA010;						// ChunkID：材质环境光属性（没使用到）
	const WORD MAT_DIF = 0xA020;						// ChunkID：材质漫反射属性
	const WORD MAT_SPE = 0xA030;						// ChunkID：材质镜面反射属性（没使用到）
	const WORD MAT_MAP = 0xA200;						// ChunkID：材质的纹理
	const WORD MAP_NAME = 0xA300;						// ChunkID：纹理的名称
	const WORD EDIT_OBJECT = 0x4000;					// ChunkID：3ds对象的面、点等信息
	const WORD OBJECT_INFO = 0x4100;					// ChunkID：对象的主要信息
	const WORD OBJECT_VERTEX = 0x4110;					// ChunkID：物体的顶点信息
	const WORD OBJECT_FACET = 0x4120;					// ChunkID：物体的面信息
	const WORD FACET_MAT = 0x4130;						// ChunkID：物体具有的材质
	const WORD FACET_SMOOTH = 0x4150;					// ChunkID：面光滑信息（没使用到）
	const WORD OBJECT_UV = 0x4140;						// ChunkID：纹理坐标信息
	const WORD OBJECT_LOCAL = 0x4160;
	const WORD PRIM_KEY = 0xB000;						// ChunkID：所有的关键帧信息（没使用到）
	const WORD COLOR_BYTE = 0x0011;						// ChunkID：颜色


	// 保存面信息：顶点与纹理坐标的索引值。
	struct tFace
	{//vertIndex要改成vector?????//need to do
		int vertIndex[3];								// 3个顶点的索引值
		int	matID;										// 该面对应的材质ID
/*		tFace() :vertIndex(3, 0), matID(0){}*/
	};

	// 保存单个3ds对象
	struct t3DObject
	{
		int  numOfVerts;								// 该对象顶点的个数
		int  numOfFaces;								// 该对象面的个数
		int  numTexVertex;								// 该对象纹理坐标的个数
		my::STRING	 objName;							// 保存对象的名称
		my::Vector3  *pVerts;							// 保存顶点坐标
		my::Vector3  *pNormals;							// 保存点的法线量
		my::Vector2  *pTexVerts;						// 保存纹理坐标
		my::tFace	 *pFaces;							// 保存面信息（顶点索引及面对应的材质）
// 		t3DObject() :numOfVerts(0), numOfFaces(0), numTexVertex(0), objName(my::STRING()),
// 			pVerts(nullptr), pNormals(nullptr), pTexVerts(nullptr), pFaces(nullptr){}
	} ;

	// 保存材质信息。
	struct tMaterial
	{
		my::STRING  matName;							// 材质的名称
		my::STRING  mapName;							// 纹理的名称（bmp，jpg等的文件名）
		BYTE	color[3];								// 材质颜色
		UINT	texureId;								// 纹理的ID（指向载入的纹理）
		bool	isTexMat;								// 该材质是不是包含有纹理
// 		tMaterial() :matName(my::STRING()), mapName(my::STRING()),
// 			color(3, 0), texureId(0), isTexMat(false){}
	};

	// 保存整个3ds模型
	struct t3DModel
	{
		int numOfObjects;								// 3ds对象的个数
		int numOfMaterials;								// 3ds材质的个数
		std::vector<my::tMaterial> pMaterials;			// 保存3ds材质
		std::vector<my::t3DObject> pObject;				// 保存3ds对象

		my::Vector3			m_maxV;						// 模型最大X,Y,Z值
		my::Vector3			m_minV;						// 模型最小X,Y,Z值
		my::Vector3			m_vCenter;					// 包围盒中心
		float				m_Rais;						// 包围盒半径
// 		t3DModel() :numOfObjects(0), numOfMaterials(0), pMaterials(std::vector<my::tMaterial>()),
// 			pObject()
	};

	struct ScanLine
	{
		my::Vector3 start;
		my::Vector3 end;
		ScanLine(my::Vector3 s, my::Vector3 e) :start(s), end(e){}
	};

	struct T3ds
	{
		T3ds(){
			x = 0;//x坐标（单位：米）
			y = 0;//y坐标（单位：米）
			z = 0;//z坐标（单位：米）
			scaleX = 1.0; //缩放倍数	
			scaleY = 1.0; //缩放倍数	
			scaleZ = 1.0; //缩放倍数	
			d_x = 0;
			d_y = 0;
			d_z = 0;
		}
		float x, y, z;
		float scaleX, scaleY, scaleZ;
		float d_x;      //  沿x旋转角度 (单位：度) Range: 0 -- 360
		float d_y;      //  沿y旋转角度 (单位：度) Range: 0 -- 360	
		float d_z;      //  沿z旋转角度 (单位：度) Range: 0 -- 360
	};

	struct Ray
	{
		my::Vector3 origin;
		my::Vector3 direction;
		Ray():origin(),direction(){}
		Ray(const my::Vector3 &start, const my::Vector3 &d) :origin(start), direction(d){}
	};

	struct POINT3D
	{
		float x;			//空间点x坐标	
		float y;			//空间点y坐标
		float z;			//空间点z坐标

		POINT3D(){}
		POINT3D(float x1, float y1, float z1) :x(x1), y(y1), z(z1){}
	};

	struct MakeOUT
	{
		float x;
		float y;
		float z;
		int show;
		float r;
		float g;
		float b;
		MakeOUT() :x(0),y(0),z(0), show(-1),r(0),g(0),b(0) {}
		MakeOUT(float X, float Y, float Z, int bshow) :x(X), y(Y), z(Z), show(bshow),r(0),g(0),b(0) {}
		void GetShowScan(float X, float Y, float Z, int bshow) { x = X; y = Y; z = Z; show = bshow; }
		void GetShowCamera(float X, float Y, float Z, float R, float G, float B,int bshow) { x = X; y = Y; z = Z; r = R; g = G; b = B; show = bshow; }
	};

	struct SPACEPOINT
	{
		float x;			//空间点x坐标	
		float y;			//空间点y坐标
		float z;			//空间点z坐标
		float fDistance;	//空间该点到发射点的距离
		my::Vector3 Lux;

		operator POINT3D(){ return POINT3D(x, y, z); }
		bool operator==(float val)
		{ 
			return (std::fabs(x - val) < 10e-6 
					&& std::fabs(y - val) < 10e-6 
					&& std::fabs(z - val) < 10e-6 
					&& std::fabs(fDistance - val) < 10e-6); 
		}
		SPACEPOINT() :x(0), y(0), z(0), fDistance(0),Lux(0,0,0){}
		SPACEPOINT(float x1, float y1, float z1, float d) :x(x1), y(y1), z(z1), fDistance(d){}
	};

	const float PI = 3.14159265;

	enum POINT_TYPE{ POINT_GROUND = -1, POINT_OBJ_DOWN = 1, POINT_OBJ_UP = 2 };

	struct CAMERA_PARAMETER
	{
		my::Vector3 startP;
		my::Vector3 angle;
		my::Vector3 cur;
		my::Vector3 lightP;
		CAMERA_PARAMETER() :startP(0, 0, 0), angle(startP), cur(startP),lightP(startP){}
		CAMERA_PARAMETER(float startPX, float startPY, float startPZ, float angleX, float angleY, float angleZ,float lightPX,float lightPY,float lightPZ) :
			startP(startPX, startPY, startPZ), angle(angleX, angleY, angleZ), cur(0, 0, 0),lightP(lightPX,lightPY,lightPZ) {}

		CAMERA_PARAMETER operator=(const CAMERA_PARAMETER& cameraParameter)
		{
			startP = cameraParameter.startP;
			angle = cameraParameter.angle;
			cur = cameraParameter.cur;
			lightP = cameraParameter.lightP;

			return *this;
		}
	};

	struct SCAN_PARAMETER 
	{
		my::Vector3 send;
		my::Vector3 angle;
		my::Vector3 end;
		my::Vector3 cur;
		float stepX;
		float stepY;
// 		float length;
// 		float width;
		SCAN_PARAMETER() :send(0, 0, 0), angle(0, 0, 0), end(angle), cur(angle), stepX(0), stepY(0){}
		SCAN_PARAMETER(float sendX, float sendY, float sendZ, float angleX, float angleY, float angleZ, float endX, float endY, float endZ, float stepX1, float stepY1) :
			send(sendX,sendY,sendZ), angle(angleX, angleY, angleZ), end(endX, endY, endZ), cur(0,0,0), stepX(stepX1), stepY(stepY1){}

		SCAN_PARAMETER operator=(const SCAN_PARAMETER& scanParameter)
		{
			send = scanParameter.send;
			angle = scanParameter.angle;
			end = scanParameter.end;
			cur = scanParameter.cur;
			stepX = scanParameter.stepX;
			stepY = scanParameter.stepY;

			return *this;
		}
	};
}