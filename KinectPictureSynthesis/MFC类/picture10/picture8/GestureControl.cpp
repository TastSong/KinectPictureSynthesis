#include "stdafx.h"
#include "GestureControl.h"
#include "include.h"
#include <Kinect.h>

CGestureControl::CGestureControl()
{
	this->kinect = nullptr;
	this->bady = nullptr;  
	this->pBodyReader = nullptr;  
	this->pBodyFrame = nullptr; 
//	this->pBody[BODY_COUNT] = { 0 }; 
	this->pSingleBody = nullptr;  
	//Joint joints[JointType_Count];
	this->isChangeBg = false;
	this->isScreenshot = false;
	this->isStop = false;
}

CGestureControl::~CGestureControl()
{
}
bool CGestureControl::LeftHandClose()
{
	HandState leftHandState = HandState_Unknown;
	//获取手部状态
	pSingleBody->get_HandLeftState(&leftHandState);
	if (HandState_Closed == leftHandState)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//检测右手是否握拳
bool CGestureControl::RightHandClose()
{
	HandState rightHandState = HandState_Unknown;
	//获取手部状态
	pSingleBody->get_HandRightState(&rightHandState);
	if (HandState_Closed == rightHandState)
	{
		//cout << "右手握拳\n";
		return true;
	}
	else
	{
		//cout << "右手张开\n";
		return false;
	}
}
bool CGestureControl::RightHandLasso()
{
	HandState rightHandState = HandState_Unknown;
	//获取手部状态
	pSingleBody->get_HandRightState(&rightHandState);
	if (HandState_Lasso == rightHandState)
	{
		//cout << "右手套索\n";
		return true;
	}
	else
	{
		return false;
	}
}
//检测手臂是否水平展开
bool CGestureControl::IsArmSpread(int hand, int shoulder)
{
	//手臂伸展开的距离
	const double ArmStretched = 0.45;
	// 获取骨骼信息节点  
	float handX = joints[hand].Position.X;
	float shoulderX = joints[shoulder].Position.X;

	if (fabs(handX - shoulderX) > ArmStretched)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//检测手部在头部之上
bool CGestureControl::IsHandAboveTheHead(int hand, int head)
{
	const double handAboveTheHead = 0.04;//手部在头部之上
	// 获取骨骼信息节点  
	float handY = joints[hand].Position.Y;
	float headY = joints[head].Position.Y;
	
	if (handY - headY > handAboveTheHead)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//打开骨骼摄像头
bool CGestureControl::Open(IKinectSensor* kinect)
{
	this->kinect = kinect;
	HRESULT hr = S_OK;
	kinect->get_BodyFrameSource(&this->bady);
	hr = bady->OpenReader(&this->pBodyReader); // 打开获取骨骼信息的  Reader  
	if (FAILED(hr))
	{
		//std::cerr << "Error : IBodyFrameSource::OpenReader()" << std::endl;
		return false;
	}
	else
	{
		return true;
	}
}
//动作检测
void CGestureControl::GestureRecognition()
{
	this->isChangeBg = false;
	this->isScreenshot = false;
	this->isStop = false;
	HRESULT hr = S_OK;	
	hr = pBodyReader->AcquireLatestFrame(&this->pBodyFrame);
	if (SUCCEEDED(hr))
	{		
		IBody* pBody[BODY_COUNT] = { 0 };
		//更新骨骼数据    
		hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, pBody); // 刷新骨骼信息（6个）  
		if (SUCCEEDED(hr))
		{
			BOOLEAN bTracked = false;

			for (int i = 0; i < BODY_COUNT; i++)
			{
				hr = pBody[i]->get_IsTracked(&bTracked); // 检查是否被追踪  
			}
		}
		for (int i = 0; i < BODY_COUNT; i++)
		{
			this->pSingleBody = pBody[i];
			if (this->pSingleBody)
			{
					
				//获取骨骼信息节点  
				this->pSingleBody->GetJoints(JointType::JointType_Count, this->joints);

				bool isRightHandClose = this->RightHandClose();
				bool isLeftHandClose = this->LeftHandClose();
				bool isRightHandLasso = this->RightHandLasso();
				bool isRightArmSpread = IsArmSpread(JointType_HandRight, JointType_ShoulderRight);
				bool isRightHandAboveTheHead = this->IsHandAboveTheHead(JointType_HandRight, JointType_Head);
				bool isLeftHandAboveTheHead = this->IsHandAboveTheHead(JointType_HandLeft, JointType_Head);
				//右握拳伸展为右转
				if (isRightArmSpread && isRightHandClose)
				{						
					//cout << "背景切换\n";
					this->isChangeBg = true;
				}
				if (isRightHandAboveTheHead && isRightHandLasso)
				{
					//cout << "截屏\n";
					this->isScreenshot = true;
				}
				if (isLeftHandAboveTheHead && isLeftHandClose)
				{
					//cout << "停止\n";
					this->isStop = true;
				}
			}

		}//for
		for (int count = 0; count < BODY_COUNT; count++)
		{
			SafeRelease(pBody[count]);
		}
		SafeRelease(this->pBodyFrame);  // 别忘了释放 
	}//if	

	return;
}

