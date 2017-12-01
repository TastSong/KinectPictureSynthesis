#pragma once

#include "include.h"

class CGestureControl
{
public:
	CGestureControl();
	~CGestureControl();

private:
	//kinect设备
	IKinectSensor* kinect;
	// 获取源  
	IBodyFrameSource* bady;  
	//身体框架读取
	IBodyFrameReader* pBodyReader;   
	//身体框架
	IBodyFrame* pBodyFrame; 
	// 默认的是 6 个骨骼 ，初始化所有的骨骼信息 
	IBody* pBody[BODY_COUNT];
	//单个人物
	IBody* pSingleBody;  
	Joint joints[JointType_Count];

private:
	//检测右手是否握拳
	bool RightHandClose();
	//检测手臂是否水平展开
	bool IsArmSpread(int hand, int shoulder);
	//检测手部在头部之上
	bool IsHandAboveTheHead(int hand, int head);

public:
	//判断是否切换到下一个场景
	bool isChangeBg;
	//判断是否截屏
	bool isScreenshot;
	//判断是否停止二级界面
	bool isStop;

public:
	//打开骨骼帧
	bool Open(IKinectSensor* kinect);
	//动作识别
	void GestureRecognition();
};

