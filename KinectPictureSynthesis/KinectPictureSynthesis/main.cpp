#include "GestureControl.h"
#include "PictureSynthesis.h"
#include "MouseControl.h"
#include <iostream>
#include "include.h"

using namespace std;

void main(void)
{
	IKinectSensor *kinect = NULL;
	HRESULT hr = S_OK;
	hr = GetDefaultKinectSensor(&kinect);  //  得到默认的设备  

	if (FAILED(hr) || kinect == NULL)
	{
		cout << "创建 sensor 失败\n";
		return;
	}
	if (kinect->Open() != S_OK) // 是否打开成功  
	{
		cout << "Kinect sensor 没准备好\n";
		return;
	}

	CGestureControl gC;
	CPictureSynthesis pS;
	CMouseControl mC;

	gC.Open(kinect);
	mC.Open(kinect);
	pS.Open(kinect);

	while (true)
	{
		//mC.KinectMouseControl();
		gC.GestureRecognition();
		pS.changeBg = gC.isChangeBg;
		pS.screenShot = gC.isScreenshot;

		Mat input = pS.PictureSynthesis();
		Mat show;
		int  cx = GetSystemMetrics(SM_CXSCREEN);
		int  cy = GetSystemMetrics(SM_CYSCREEN);
		resize(input, show, Size(cx, cy));
		imshow("input", show);
		waitKey(33);
	}
	

	kinect->Close();  // 优雅的关闭kinect
	cout << "kinect关闭\n";
}