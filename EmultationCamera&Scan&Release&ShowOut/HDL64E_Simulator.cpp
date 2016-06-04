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
//	��������ComputeValidRay
// 
//  ���������	_3dsModel:	    3ds�ļ�	
//				SphereCenter:	ģ�����������	
//				SphereR:	    ģ�������뾶
//				ScanPoint:      ɨ���
//
//  �������ܣ� ������Ч�Ĺ��ߣ���Ч��ָ��ģ�͵�������н���
//
//  ����ֵ��   ��
//
//   ��д�ˣ�   lisben_cyan
//**************************************************************************

//**************************************************************************  
//   Z   Y     ==========
//   /\ /\	  /|�����״�|�����ߵ����λ�ã�x0,y0,z0����
//   |  /   /  ==========
//   | /  /  DIR(���ߵķ���ʸ��(dx,dy,dz))   
//   |/ /        
//   O---------->X
//   ģ�����ģ�����ϵԭ�㣩
//
//              dx = sin(CurVertAngle)*cos(CurHoriAngle)
//	            dy = sin(CurVertAngle)*sin(CurHoriAngle)
//	            dz = cos(CurVertAngle)
//**************************************************************************

void CHDL64E_Simulator::ComputeValidRay(C3DSModel& _3dsModel, my::Vector3& Vec_SphereCenter, 
										double& dSphereR, std::vector<my::SPACEPOINT>& ScanPoint)
{
    double CurVertAngle = 0.0; // ������Z��������н�
	double CurHoriAngle = 0.0; //������XYƽ���ϵ�ͶӰ����X��������н�
	my::Ray Tmp_Ray;
	my::Vector3 Tmp_Dir, Tmp_Pos;
	Tmp_Pos = getPos();
	Tmp_Ray.origin.x = Tmp_Pos.x; Tmp_Ray.origin.y = Tmp_Pos.y; Tmp_Ray.origin.z = Tmp_Pos.z;
	my::SPACEPOINT spCrossPoint;
	// ˮƽ����360��
	while(CurHoriAngle < 360.0)
	{
	    cout<<"CurHoriAngle: "<<CurHoriAngle<<endl;
		CurVertAngle = 90.0 - UPPERBLOCK_UPBOUND;
		for (int i = 0; i < LASER_NUM; ++i)
		{
			cout<<"CurVertAngle: "<<CurVertAngle<<endl;
			// ������ߵķ�������
			Tmp_Ray.direction.x = sin(CurVertAngle*PI/180.0)*cos(CurHoriAngle*PI/180.0);
			Tmp_Ray.direction.y = sin(CurVertAngle*PI/180.0)*sin(CurHoriAngle*PI/180.0);
			Tmp_Ray.direction.z = cos(CurVertAngle*PI/180.0);

			// �жϹ����Ƿ���ģ�͵İ�Χ���ཻ�����ཻ�������жϹ����Ƿ���ģ���ཻ�����ཻ�����㽻�㣬����ScanPoint
			if (OnRayHitModel(_3dsModel, Tmp_Ray, spCrossPoint, Vec_SphereCenter, dSphereR))
			{
				ScanPoint.push_back(spCrossPoint);                
			}
            // �����������ֵķֱ��ʲ�ͬ���ֿ�����
			if ( i < UPPERBLOCK_LASER_NUM)
				CurVertAngle += UPPERBLOCK_VERT_RES;			// �ϲ�ģ��
			else
				CurVertAngle += LOWERBLOCK_VERT_RES;			// �²�ģ��
		}

		CurHoriAngle += HORI_AZIMUTH_RES;
	}
}





//*****************************************************************************
//
//	��������OnRayHitModels
// 
//  �������:	Ray ryScanRay��	�ռ��е�һ������		
//				spScanPoint��	������ģ�͵Ľ���
//              SphereCenter��  ģ������������
//              SphereR��       ģ�������İ뾶
//              _3dsModel��     3ds�ļ�����
//
//  ��������� BOOL: �����������е�һ��ģ���н��㣬�򷵻�TRUE��ͬʱ��������д��
//					 spScanPoint���У����򣬷���FALSE,ͬʱ����spScanPoint���Ķ�
//
//  ���ܣ� ��ɨ��������ģ�ͣ�����ģ���Ͽռ�����ꡣ
//
//******************************************************************************

bool CHDL64E_Simulator::OnRayHitModel(C3DSModel& _3dsModel, my::Ray& ScanRay,my::SPACEPOINT& spSscanPoint, my::Vector3& SphereCenter, double& SphereR)
{
	float fdis;
	my::Vector3 vModel;
	
	// �����Ƿ���ģ��������н���
	if (ComputeRayHitSphere(ScanRay, SphereCenter, SphereR))
	{
		if (RayhitCheck(_3dsModel.m_3DModel, ScanRay, vModel, fdis))
		{
			
			spSscanPoint.x = vModel.x ;  //��
			spSscanPoint.y = vModel.y ;  //��
			spSscanPoint.z = vModel.z ;  //��
			spSscanPoint.fDistance = fdis ;
			return true;
		}
	}

	return false;
}




//**************************************************************************
//	��������ComputeRayHitSphere											  
// 
//  ���������	_ray:	        �������									
//				_SphereCenter:	ģ�����������	
//				_SphereR:	    ģ�������뾶
//
//  �������ܣ� �ж�һ��ֱ���Ƿ������ཻ
//
//  ����ֵ��   ΪTRUEʱ�ཻ��ΪFALSEʱ���ཻ
//
//  ��д�ˣ�   lisben_cyan
//**************************************************************************

bool CHDL64E_Simulator::ComputeRayHitSphere(my::Ray& _ray, my::Vector3& _SphereCenter, double& _SphereR)
{
	my::Vector3 ray_dir = _ray.direction;
	my::Vector3 dist = _ray.origin - _SphereCenter;
	// ��λ����������
	ray_dir.Normalize();
	
	float a = ray_dir.Dot(ray_dir);
	float b = 2 * dist.Dot(ray_dir);
	float c = dist.Dot(dist) - _SphereR * _SphereR;
	float d = (b*b)-(4*c);

	if(d < 0) //B*B-4C<0 �޽�
	{
		return false;
	}
	else
	{
		float s0 = (-b - sqrt(d) ) / 2;
		float s1 = (-b + sqrt(d) ) / 2;
		
		//�����ⶼС���㣬����FALSE
		if( (s0 < 0) && (s1 < 0)) return false;	
		else return true;	
	}
}


//*************************************************************************************************************
//	��������RayhitCheck											  
// 
//  ���������	 pModel��  ģ��
//               ray��     ����
//               Vec_Hit�� ������ģ�͵��ཻ��
//				 fdist��   ���㵽�������ľ���    
//  �������ܣ�   �������Ƿ���ģ���ཻ�����ཻ������������
//              
//  ��д�ˣ�     lisben_cyan
//*************************************************************************************************************

bool CHDL64E_Simulator::RayhitCheck(my::t3DModel& pModel, const my::Ray& ray, my::Vector3& Vec_Hit, float& fdist)
{
	my::t3DObject* obj = NULL;
	fdist = -1.0f;             // �����ֵ
	float ftmp_dist = -1.0f;   // һ�����߿����������ཻ����ÿ�����ཻʱ���������¼�ڴ���ʱ������
	float u = 0.0, v = 0.0, ftmp_u = 0.0, ftmp_v = 0.0; 
	bool  bhit = false;
	my::Vector3 Vec_Tmp_Hit, Vert_Triangle[3],Vert_Triangle_New[3];//��������任��Ķ�������;
    
	int iface;		  // �ཻ��������������
	int iObj;		  // �ཻ��ģ�Ͷ�������
	int *index;       // �ཻ�������ε���������������

	for(int i = 0; i < pModel.numOfObjects; ++i)
	{// begin objects loop

		obj = &pModel.pObject[i];
		for(int j = 0; j < obj->numOfFaces; ++j)
		{// begin faces loop

			index = obj->pFaces[j].vertIndex;
			// ��ȡ�����ε�3������
			for(int k = 0; k < 3; ++k)
			{				
				Vert_Triangle[k] = obj->pVerts[index[k]];
				//Vert_Triangle_New[k] = VertTransformation(Vert_Triangle[k]);
			}

			// �����Ƿ����������ཻ
			if(RayHitTriangle(ray, Vert_Triangle, ftmp_dist, ftmp_u, ftmp_v))
			{// begin computing the hit point

				if(fdist)
				{
					if((fdist == -1.0)||(ftmp_dist < fdist))
					{
						if(fdist)
						{
							fdist  = ftmp_dist; // ȷ��������ཻ����Ϊ�������
						}
						iface = j; // �ཻ��������������
						iObj = i;  // �ཻ��ģ�Ͷ�������
						u = ftmp_u;
						v = ftmp_v;
						
						// �ཻ֮���ཻ�����ΪTRUE
						if(!bhit)
						{
							bhit = true; 
						}
					}
				}

			}// end computing the hit point

		}// end faces loop

	}// end objects loop

	// ����û�����������ཻʱ������FALSE
	if( !bhit )
		return false;

	// ͨ������������������������������u��v�����������ģ�͵����ս���
	obj = &pModel.pObject[iObj];
	index = obj->pFaces[iface].vertIndex;
	Vec_Hit = obj->pVerts[index[0]] + (obj->pVerts[index[1]] - obj->pVerts[index[0]])*u + (obj->pVerts[index[2]] - obj->pVerts[index[0]])*v;
	return true;
}




//*************************************************************************************************************
//	��������RayHitTriangle											  
// 
//  ���������	 ray��                ģ��
//               Vert_Triangle[]��    ������������
//				 t��                  ���㵽�������ľ���
//				 u��v                 �������������ڲ�����
//
//  �������ܣ�   ���������������Ľ���
//              
//  ��д�ˣ�     lisben_cyan
//*************************************************************************************************************
bool CHDL64E_Simulator::RayHitTriangle( const my::Ray &ray, my::Vector3 Vert_Triangle[], float &t, float &u, float &v)
{
	float fInv_det = 0.0;
	my::Vector3 Vec_Edge1, Vec_Edge2;
	my::Vector3 Vec_P, Vec_T, Vec_Q;

	// ����(1/(Ray_dir X Edge2)*Edge1) 
	Vec_Edge1 = Vert_Triangle[1] - Vert_Triangle[0]; 
	Vec_Edge2 = Vert_Triangle[2] - Vert_Triangle[0];
	Vec_P = ray.direction.Cross(Vec_Edge2);
	float fdet = Vec_Edge1.Dot(Vec_P);

	// fdet�ӽ�0ʱ����ʾ������������ƽ�У����ཻ
    if (fabs(fdet) < 0.0001f)
	return false;

	fInv_det = 1.0/fdet;

	// ����Ray_pos - V0
	Vec_T.x = ray.origin.x - Vert_Triangle[0].x;
	Vec_T.y = ray.origin.y - Vert_Triangle[0].y;
	Vec_T.z = ray.origin.z - Vert_Triangle[0].z;

	// ����u = (1/(Ray_dir X Edge2)*Edge1) * ��(Ray_dir X Edge2��* (Ray_pos - V0)��
	u = Vec_T.Dot(Vec_P) * fInv_det;
	if (u < 0.0f || u > 1.0f)
    return false;

	// ����v = (1/(Ray_dir X Edge2)*Edge1) * ��((Ray_pos - V0) X Edge1��* Ray_dir��
	Vec_Q = Vec_T.Cross(Vec_Edge1);
	v = ray.direction.Dot(Vec_Q) * fInv_det;

	if( v < 0.0f || u + v > 1.0f )
	return false;
	
	// ����t = (1/(Ray_dir X Edge2)*Edge1) * ��((Ray_pos - V0) X Edge1��* Edge2��
	t = Vec_Edge2.Dot(Vec_Q) * fInv_det;	
	return true;
}

