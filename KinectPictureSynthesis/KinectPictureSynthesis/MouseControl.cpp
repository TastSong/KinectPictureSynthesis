#include "MouseControl.h"
#include "include.h"

CMouseControl::CMouseControl()
{
	this->kinect = nullptr;
	this->bady = nullptr;
	this->pBodyReader = nullptr;
	this->pBodyFrame = nullptr;
	this->pBody[BODY_COUNT] = { 0 };
	this->pSingleBody = nullptr;
}

CMouseControl::~CMouseControl()
{
}
//检测右手是否握拳
bool CMouseControl::RightHandClose()
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
//检测右手臂是否向前推进
bool CMouseControl::IsRightArmSpread()
{
	const double rightArmSpread = 0.30;//手臂向前推进的距离
	Joint joints[JointType_Count];
	// 获取骨骼信息节点  
	pSingleBody->GetJoints(JointType::JointType_Count, joints);
	float rightHandZ = joints[JointType_HandRight].Position.Z;
	float spineBaseZ = joints[JointType::JointType_SpineBase].Position.Z;

	if (fabs(rightHandZ - spineBaseZ) > rightArmSpread)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool CMouseControl::Open(IKinectSensor* kinect)
{
	this->kinect = kinect;
	HRESULT hr = S_OK;
	kinect->get_BodyFrameSource(&bady);
	hr = bady->OpenReader(&pBodyReader); // 打开获取骨骼信息的  Reader  
	if (FAILED(hr))
	{
		//std::cerr << "Error : IBodyFrameSource::OpenReader()" << std::endl;
		return false;
	}
	return true;
}
void CMouseControl::KinectMouseControl()
{
	
	HRESULT hr = S_OK;	
	//cout << "检测开始" << endl;
	if(true)
	{
		
		hr = pBodyReader->AcquireLatestFrame(&pBodyFrame);

		if (SUCCEEDED(hr))
		{
			
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
				pSingleBody = pBody[i];
				if (pSingleBody)
				{
					Joint joints[JointType_Count];
					//获取骨骼信息节点  
					pSingleBody->GetJoints(JointType::JointType_Count, joints);

					// 得到各种骨骼位置
					CameraSpacePoint handLeft = joints[JointType_HandLeft].Position;
					CameraSpacePoint handRight = joints[JointType_HandRight].Position;
					CameraSpacePoint spineBase = joints[JointType_SpineBase].Position;
					// 如果右手向前推进
					if (IsRightArmSpread())
					{
						//用这个计算的手x。我们不使用右肩作为参考，
						//因为肩膀的右肩通常是在升力的后面，而这个位置是推断和不稳定的。
						//因为脊椎底部在右手的左边，我们加上0.05f，使它更接近右边。
						float x = handRight.X - spineBase.X + 0.05f;
						//用这个来计算。它的脊柱底部比右方要低，我们加上0.51f来让它成为higer，
						//值0.51是通过多次测试来计算的，你可以把它设置为你喜欢的另一个。
						float y = spineBase.Y - handRight.Y + 0.51f;
						//获取屏幕大小
						int  screenWidth = GetSystemMetrics(SM_CXSCREEN);
						int  screenHeight = GetSystemMetrics(SM_CYSCREEN);
						// 得到当前光标位置
						POINT p;
						GetCursorPos(&p);//获取鼠标坐标  
						//使用的平滑度应该是0-0.95f。我们的算法是:oldPos + (newPos - oldPos) * smoothValue
						float smoothing = 1 - 0.8f;
						// 设置光标位置
						SetCursorPos((int)(p.x + (x  * 1.8f * screenWidth - p.x) * smoothing), (int)(p.y + ((y + 0.25f) * 1.8f * screenHeight - p.y) * smoothing));

					}
					if (IsRightArmSpread() && RightHandClose())
					{
						mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
						Sleep(10);//要留给某些应用的反应时间   
						mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					}

				}

			}//for
			for (int count = 0; count < BODY_COUNT; count++)
			{
				SafeRelease(pBody[count]);
			}
			SafeRelease(pBodyFrame);  // 别忘了释放 
		}//if	

	}

	return;
}