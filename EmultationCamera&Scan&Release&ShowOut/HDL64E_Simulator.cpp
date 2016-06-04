#include "stdafx.h"
#include "HDL64E_Simulator.h"
using namespace std;

void CHDL64E_Simulator::setPos(my::Vector3 pos)
{
	m_RadarPos = pos;
}
my::Vector3 CHDL64E_Simulator::getPos(void)
{
	return m_RadarPos;
}



//**************************************************************************
//	函数名：ComputeValidRay
// 
//  输入参数：	_3dsModel:	    3ds文件	
//				SphereCenter:	模型外接球球心	
//				SphereR:	    模型外接球半径
//				ScanPoint:      扫描点
//
//  函数功能： 计算有效的光线，有效是指与模型的外接球有交点
//
//  返回值：   无
//
//   编写人：   lisben_cyan
//**************************************************************************

//**************************************************************************  
//   Z   Y     ==========
//   /\ /\	  /|激光雷达|（光线的起点位置（x0,y0,z0））
//   |  /   /  ==========
//   | /  /  DIR(光线的方向矢量(dx,dy,dz))   
//   |/ /        
//   O---------->X
//   模型中心（坐标系原点）
//
//              dx = sin(CurVertAngle)*cos(CurHoriAngle)
//	            dy = sin(CurVertAngle)*sin(CurHoriAngle)
//	            dz = cos(CurVertAngle)
//**************************************************************************

void CHDL64E_Simulator::ComputeValidRay(C3DSModel& _3dsModel, my::Vector3& Vec_SphereCenter, 
										double& dSphereR, std::vector<my::SPACEPOINT>& ScanPoint)
{
    double CurVertAngle = 0.0; // 光线与Z轴正方向夹角
	double CurHoriAngle = 0.0; //光线在XY平面上的投影线与X轴正方向夹角
	my::Ray Tmp_Ray;
	my::Vector3 Tmp_Dir, Tmp_Pos;
	Tmp_Pos = getPos();
	Tmp_Ray.origin.x = Tmp_Pos.x; Tmp_Ray.origin.y = Tmp_Pos.y; Tmp_Ray.origin.z = Tmp_Pos.z;
	my::SPACEPOINT spCrossPoint;
	// 水平方向360度
	while(CurHoriAngle < 360.0)
	{
	    cout<<"CurHoriAngle: "<<CurHoriAngle<<endl;
		CurVertAngle = 90.0 - UPPERBLOCK_UPBOUND;
		for (int i = 0; i < LASER_NUM; ++i)
		{
			cout<<"CurVertAngle: "<<CurVertAngle<<endl;
			// 计算光线的方向向量
			Tmp_Ray.direction.x = sin(CurVertAngle*PI/180.0)*cos(CurHoriAngle*PI/180.0);
			Tmp_Ray.direction.y = sin(CurVertAngle*PI/180.0)*sin(CurHoriAngle*PI/180.0);
			Tmp_Ray.direction.z = cos(CurVertAngle*PI/180.0);

			// 判断光线是否与模型的包围球相交，若相交，继续判断光线是否与模型相交，若相交，计算交点，存入ScanPoint
			if (OnRayHitModel(_3dsModel, Tmp_Ray, spCrossPoint, Vec_SphereCenter, dSphereR))
			{
				ScanPoint.push_back(spCrossPoint);                
			}
            // 上下两个部分的分辨率不同，分开处理
			if ( i < UPPERBLOCK_LASER_NUM)
				CurVertAngle += UPPERBLOCK_VERT_RES;			// 上部模块
			else
				CurVertAngle += LOWERBLOCK_VERT_RES;			// 下部模块
		}

		CurHoriAngle += HORI_AZIMUTH_RES;
	}
}





//*****************************************************************************
//
//	函数名：OnRayHitModels
// 
//  输入参数:	Ray ryScanRay：	空间中的一条射线		
//				spScanPoint：	射线与模型的交点
//              SphereCenter：  模型外接球的球心
//              SphereR：       模型外接球的半径
//              _3dsModel：     3ds文件对象
//
//  输出参数： BOOL: 若射线与其中的一个模型有交点，则返回TRUE，同时，将交点写入
//					 spScanPoint当中；否则，返回FALSE,同时不对spScanPoint做改动
//
//  功能： 若扫描线遇到模型，则反推模型上空间点坐标。
//
//******************************************************************************

bool CHDL64E_Simulator::OnRayHitModel(C3DSModel& _3dsModel, my::Ray& ScanRay,my::SPACEPOINT& spSscanPoint, my::Vector3& SphereCenter, double& SphereR)
{
	float fdis;
	my::Vector3 vModel;
	
	// 光线是否与模型外接球有交点
	if (ComputeRayHitSphere(ScanRay, SphereCenter, SphereR))
	{
		if (RayhitCheck(_3dsModel.m_3DModel, ScanRay, vModel, fdis))
		{
			
			spSscanPoint.x = vModel.x ;  //米
			spSscanPoint.y = vModel.y ;  //米
			spSscanPoint.z = vModel.z ;  //米
			spSscanPoint.fDistance = fdis ;
			return true;
		}
	}

	return false;
}




//**************************************************************************
//	函数名：ComputeRayHitSphere											  
// 
//  输入参数：	_ray:	        入射光线									
//				_SphereCenter:	模型外接球球心	
//				_SphereR:	    模型外接球半径
//
//  函数功能： 判断一条直线是否与球相交
//
//  返回值：   为TRUE时相交，为FALSE时不相交
//
//  编写人：   lisben_cyan
//**************************************************************************

bool CHDL64E_Simulator::ComputeRayHitSphere(my::Ray& _ray, my::Vector3& _SphereCenter, double& _SphereR)
{
	my::Vector3 ray_dir = _ray.direction;
	my::Vector3 dist = _ray.origin - _SphereCenter;
	// 单位化方向向量
	ray_dir.Normalize();
	
	float a = ray_dir.Dot(ray_dir);
	float b = 2 * dist.Dot(ray_dir);
	float c = dist.Dot(dist) - _SphereR * _SphereR;
	float d = (b*b)-(4*c);

	if(d < 0) //B*B-4C<0 无解
	{
		return false;
	}
	else
	{
		float s0 = (-b - sqrt(d) ) / 2;
		float s1 = (-b + sqrt(d) ) / 2;
		
		//两个解都小于零，返回FALSE
		if( (s0 < 0) && (s1 < 0)) return false;	
		else return true;	
	}
}


//*************************************************************************************************************
//	函数名：RayhitCheck											  
// 
//  输入参数：	 pModel：  模型
//               ray：     光线
//               Vec_Hit： 光线与模型的相交点
//				 fdist：   交点到光线起点的距离    
//  函数功能：   检测光线是否与模型相交，若相交，则计算出交点
//              
//  编写人：     lisben_cyan
//*************************************************************************************************************

bool CHDL64E_Simulator::RayhitCheck(my::t3DModel& pModel, const my::Ray& ray, my::Vector3& Vec_Hit, float& fdist)
{
	my::t3DObject* obj = NULL;
	fdist = -1.0f;             // 赋予初值
	float ftmp_dist = -1.0f;   // 一条光线可能与多个面相交，与每个面相交时，将距离记录在此临时变量中
	float u = 0.0, v = 0.0, ftmp_u = 0.0, ftmp_v = 0.0; 
	bool  bhit = false;
	my::Vector3 Vec_Tmp_Hit, Vert_Triangle[3],Vert_Triangle_New[3];//经过坐标变换后的顶点坐标;
    
	int iface;		  // 相交的三角形面索引
	int iObj;		  // 相交的模型对象索引
	int *index;       // 相交的三角形的三顶点索引数组

	for(int i = 0; i < pModel.numOfObjects; ++i)
	{// begin objects loop

		obj = &pModel.pObject[i];
		for(int j = 0; j < obj->numOfFaces; ++j)
		{// begin faces loop

			index = obj->pFaces[j].vertIndex;
			// 获取三角形的3个顶点
			for(int k = 0; k < 3; ++k)
			{				
				Vert_Triangle[k] = obj->pVerts[index[k]];
				//Vert_Triangle_New[k] = VertTransformation(Vert_Triangle[k]);
			}

			// 光线是否与三角形相交
			if(RayHitTriangle(ray, Vert_Triangle, ftmp_dist, ftmp_u, ftmp_v))
			{// begin computing the hit point

				if(fdist)
				{
					if((fdist == -1.0)||(ftmp_dist < fdist))
					{
						if(fdist)
						{
							fdist  = ftmp_dist; // 确保与光线相交的面为最近的面
						}
						iface = j; // 相交的三角形面索引
						iObj = i;  // 相交的模型对象索引
						u = ftmp_u;
						v = ftmp_v;
						
						// 相交之后将相交标记置为TRUE
						if(!bhit)
						{
							bhit = true; 
						}
					}
				}

			}// end computing the hit point

		}// end faces loop

	}// end objects loop

	// 光线没有与三角面相交时，返回FALSE
	if( !bhit )
		return false;

	// 通过物体索引，面索引，顶点索引，u，v，计算光线与模型的最终交点
	obj = &pModel.pObject[iObj];
	index = obj->pFaces[iface].vertIndex;
	Vec_Hit = obj->pVerts[index[0]] + (obj->pVerts[index[1]] - obj->pVerts[index[0]])*u + (obj->pVerts[index[2]] - obj->pVerts[index[0]])*v;
	return true;
}




//*************************************************************************************************************
//	函数名：RayHitTriangle											  
// 
//  输入参数：	 ray：                模型
//               Vert_Triangle[]：    三角形三顶点
//				 t：                  交点到光线起点的距离
//				 u，v                 交点在三角形内部坐标
//
//  函数功能：   计算光线与三角面的交点
//              
//  编写人：     lisben_cyan
//*************************************************************************************************************
bool CHDL64E_Simulator::RayHitTriangle( const my::Ray &ray, my::Vector3 Vert_Triangle[], float &t, float &u, float &v)
{
	float fInv_det = 0.0;
	my::Vector3 Vec_Edge1, Vec_Edge2;
	my::Vector3 Vec_P, Vec_T, Vec_Q;

	// 计算(1/(Ray_dir X Edge2)*Edge1) 
	Vec_Edge1 = Vert_Triangle[1] - Vert_Triangle[0]; 
	Vec_Edge2 = Vert_Triangle[2] - Vert_Triangle[0];
	Vec_P = ray.direction.Cross(Vec_Edge2);
	float fdet = Vec_Edge1.Dot(Vec_P);

	// fdet接近0时，表示光线与三角面平行，不相交
    if (fabs(fdet) < 0.0001f)
	return false;

	fInv_det = 1.0/fdet;

	// 计算Ray_pos - V0
	Vec_T.x = ray.origin.x - Vert_Triangle[0].x;
	Vec_T.y = ray.origin.y - Vert_Triangle[0].y;
	Vec_T.z = ray.origin.z - Vert_Triangle[0].z;

	// 计算u = (1/(Ray_dir X Edge2)*Edge1) * （(Ray_dir X Edge2）* (Ray_pos - V0)）
	u = Vec_T.Dot(Vec_P) * fInv_det;
	if (u < 0.0f || u > 1.0f)
    return false;

	// 计算v = (1/(Ray_dir X Edge2)*Edge1) * （((Ray_pos - V0) X Edge1）* Ray_dir）
	Vec_Q = Vec_T.Cross(Vec_Edge1);
	v = ray.direction.Dot(Vec_Q) * fInv_det;

	if( v < 0.0f || u + v > 1.0f )
	return false;
	
	// 计算t = (1/(Ray_dir X Edge2)*Edge1) * （((Ray_pos - V0) X Edge1）* Edge2）
	t = Vec_Edge2.Dot(Vec_Q) * fInv_det;	
	return true;
}

