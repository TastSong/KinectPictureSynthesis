#pragma once

#include "include.h"

class CMouseControl
{
public:
	CMouseControl();
	~CMouseControl();
private:
	//kinect设备
	IKinectSensor* kinect;
	// 获取源 
	IBodyFrameSource* bady;   
	IBodyFrameReader* pBodyReader;
	IBodyFrame* pBodyFrame;
	// 默认的是 6 个骨骼 ，初始化所有的骨骼信息
	IBody* pBody[BODY_COUNT];  
	IBody* pSingleBody;

private:
	//判断右手是否握拳
	bool RightHandClose();
	//判断右臂是否向前推进
	bool IsRightArmSpread();
	
public:
	//打开骨骼摄像头
	bool Open(IKinectSensor* kinect);
	//进行鼠标控制
	void KinectMouseControl();
};

