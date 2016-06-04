#pragma once

#include "C3DSModel.h"
#include <iostream>
// HDL64E模拟器
class CHDL64E_Simulator
{
    friend class C3DSModel;
public:
	CHDL64E_Simulator(){}
	~CHDL64E_Simulator(){}

    void setPos(my::Vector3 pos);
	my::Vector3 getPos(void);
	//Vector3 CHDL64E_Simulator::VertTransformation(Vector3& Vec_Vert);

	void ComputeValidRay(C3DSModel& _3dsModel, my::Vector3& Vec_SphereCenter, double& dSphereR, std::vector<my::SPACEPOINT>& ScanPoint);
		bool OnRayHitModel(C3DSModel& _3dsModel, my::Ray& ScanRay,my::SPACEPOINT& spSscanPoint, my::Vector3& SphereCenter, double& SphereR);
			bool ComputeRayHitSphere(my::Ray& _ray, my::Vector3& _SphereCenter, double& _SphereR);// 判断光线是否与模型外接球相交	
				bool RayhitCheck(my::t3DModel& pModel, const my::Ray& ray, my::Vector3& Vec_Hit, float& fdist);
					bool RayHitTriangle( const my::Ray &ray,my::Vector3 vtri[],float &t, float &u, float &v);
// HDL64属性
private:
	my::Vector3 m_RadarPos;
	//vector<Ray> m_ScanRay;
};