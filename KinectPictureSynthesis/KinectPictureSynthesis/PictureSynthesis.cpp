#include "PictureSynthesis.h"
#include "include.h"

#include <sstream>
#include <string>
#include <iostream>
using namespace std;

CPictureSynthesis::CPictureSynthesis()
{
	this->kinect = nullptr;

	this->colorHeight = 0;
	this->colorWidth = 0;
	this->myColorSource = nullptr;
	this->myColorReader = nullptr;
	this->myDescription = nullptr;
	this->colorDataSize = 0;
	this->output = nullptr;
	this->myColorFrame = nullptr;

	this->depthHeight = 0;
	this->depthWidth = 0;
	this->myDepthSource = nullptr;
	this->myDepthReader = nullptr;
	this->depthDataSize = 0;
	this->depthData = nullptr;
	this->myDepthframe = nullptr;

	this->bodyHeight = 0;
	this->bodyWidth = 0;
	this->myBodyIndexSource = nullptr;
	this->myBodyIndexReader = nullptr;
	this->bodyDataSize = 0;
	this->bodyData = nullptr;
	this->myBodyIndexFrame = nullptr;

	this->myMaper = nullptr;

	this->photoNumber = 3;
	this->count_B = 1; //加载的位置不一样
	this->count_F = 0;

	this->changeBg = false;
	this->screenShot = false;
	
	this->temp = imread("../Background/test0.jpg");
}

CPictureSynthesis::~CPictureSynthesis()
{
	//记得各种释放
	delete[] depthData;        
	delete[] bodyData;
	delete[] output;

	this->myMaper->Release();
	this->myColorReader->Release();
	this->myDepthReader->Release();
	this->myBodyIndexReader->Release();
}
//透明前景添加
void CPictureSynthesis::mapToMat(const cv::Mat &srcAlpha, cv::Mat &dest, int x, int y)
{
	int nc = 3;
	int alpha = 0;

	for (int j = 0; j < srcAlpha.rows; j++)
	{
		for (int i = 0; i < srcAlpha.cols * 3; i += 3)
		{
			// 目的图片为三通道，所以是三通道的遍历、四通道的源图
			// i/3*4表示第i/3个像素的位置 
			// i/3*4 + 3表示本像素的alpha通道的值
			alpha = srcAlpha.ptr<uchar>(j)[i / 3 * 4 + 3];
			//alpha = 255-alpha;
			if (alpha != 0) //4通道图像的alpha判断
			{
				for (int k = 0; k < 3; k++)
				{
					// if (src1.ptr<uchar>(j)[i / nc*nc + k] != 0)
					if ((j + y < dest.rows) && (j + y >= 0) &&
						((i + x * 3) / 3 * 3 + k < dest.cols * 3) && ((i + x * 3) / 3 * 3 + k >= 0) &&
						(i / nc * 4 + k < srcAlpha.cols * 4) && (i / nc * 4 + k >= 0))
					{
						dest.ptr<uchar>(j + y)[(i + x*nc) / nc*nc + k] = srcAlpha.ptr<uchar>(j)[(i) / nc * 4 + k];
					}
				}
			}
		}
	}
}
//获取彩色数据
bool CPictureSynthesis::OpenColor()
{
	//准备好彩色图像的Reader并获取尺寸
	this->kinect->get_ColorFrameSource(&this->myColorSource);
	HRESULT hr = S_OK;
	hr = this->myColorSource->OpenReader(&this->myColorReader);
	if (FAILED(hr))
	{
		return false;
	}
	this->myColorSource->get_FrameDescription(&this->myDescription);
	this->myDescription->get_Height(&this->colorHeight);
	this->myDescription->get_Width(&this->colorWidth);

	this->myDescription->Release();
	this->myColorSource->Release();
	//为图像准备buffer
	this->colorDataSize = this->colorHeight * this->colorWidth;
	this->output = new DepthSpacePoint[this->colorDataSize];
	//准备buffer
	Mat colorData(colorHeight, colorWidth, CV_8UC4);        
	this->colorData = colorData;

	return true;
}
//获取深度数据
bool CPictureSynthesis::OpenDepth()
{
	//准备好深度图像的Reader并获取尺寸
	this->kinect->get_DepthFrameSource(&this->myDepthSource);
	HRESULT hr = S_OK;
	hr = this->myDepthSource->OpenReader(&this->myDepthReader);
	if (FAILED(hr))
	{
		return false;
	}
	this->myDepthSource->get_FrameDescription(&this->myDescription);
	this->myDescription->get_Height(&this->depthHeight);
	this->myDescription->get_Width(&this->depthWidth);

	this->myDescription->Release();
	this->myDepthSource->Release();
	//为图像准备buffer
	this->depthDataSize = this->depthHeight * this->depthWidth;
	this->depthData = new UINT16[this->depthDataSize];

	return true;
}
//获取身体框架数据
bool CPictureSynthesis::OpenBodyIndex()
{
	//准备好人体索引图像的Reader并获取尺寸
	this->kinect->get_BodyIndexFrameSource(&this->myBodyIndexSource);
	HRESULT hr = S_OK;
	hr = this->myBodyIndexSource->OpenReader(&this->myBodyIndexReader);
	if (FAILED(hr))
	{
		return false;
	}
	this->myDepthSource->get_FrameDescription(&this->myDescription);
	this->myDescription->get_Height(&this->bodyHeight);
	this->myDescription->get_Width(&this->bodyWidth);

	this->myDescription->Release();
	this->myBodyIndexSource->Release();
	//为图像准备buffer
	this->bodyDataSize = this->bodyHeight * this->bodyWidth;
	this->bodyData = new BYTE[this->bodyDataSize];

	return true;
}
bool CPictureSynthesis::Open(IKinectSensor* kinect)
{
	this->kinect = kinect;
	if (!this->OpenColor())
	{
		return false;
	}
	if (!this->OpenDepth())
	{
		return false;
	}
	if (!this->OpenBodyIndex())
	{
		return false;
	}
	//并且开启Mappe
	this->kinect->get_CoordinateMapper(&this->myMaper);	

	return true;
}
//切换到下一个场景
void CPictureSynthesis::NextScene()
{
	string str1 = "../Background/test";
	stringstream stream;
	stream << count_B;
	string str2 = stream.str();
	string str3 = ".jpg";
	string backgroundPath = str1 + str2 + str3;

	this->temp = imread(backgroundPath);
	count_B = (count_B + 1) % this->photoNumber;
	count_F = (count_F + 1) % this->photoNumber;
	
	changeBg = false;	
}
void CPictureSynthesis::FullScreen()
{
	this->input = this->copy;
	namedWindow("input", CV_WINDOW_AUTOSIZE);
	moveWindow("input", 0, 0);
	setWindowProperty("input", CV_WINDOW_FULLSCREEN, CV_WINDOW_FULLSCREEN);;
	HWND win_handle = FindWindow(0, L"input");
	if (!win_handle)
	{
		cout << "Failed FindWindow" << endl;
	}

	// Resize  
	unsigned int flags = (SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);
	flags &= ~SWP_NOSIZE;
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int w = input.cols;
	unsigned int h = input.rows;
	SetWindowPos(win_handle, HWND_NOTOPMOST, x, y, w, h, flags);

	SetWindowLong(win_handle, GWL_STYLE, GetWindowLong(win_handle, GWL_EXSTYLE) | WS_EX_TOPMOST);
	ShowWindow(win_handle, SW_SHOW);
}
//添加前景
void CPictureSynthesis::AddFrontPhotos()
{
	string str1 = "../Outlook/test";
	stringstream stream;
	stream << count_F;
	string str2 = stream.str();
	string str3 = ".png";
	string frontPhotosPath = str1 + str2 + str3;

	Mat srcAlpha;
	// imread后一个参数为-1表示读取带有alpha通道的图片
	srcAlpha = imread(frontPhotosPath, -1);
	// 表示从dest的第(50,100)设置对srcAlpha的叠加
	mapToMat(srcAlpha, this->copy, 40, 40);		
}
//截屏
void CPictureSynthesis::ScreenShot()
{
	if (this->screenShot)
	{
		//cout << "接哦in给我" << endl;
		//等待2秒截屏
		Sleep(2000);
		IplImage qImg;
		qImg = IplImage(screen); // cv::Mat -> IplImage
		cvSaveImage("../Screenshot/screenshot.jpg", &qImg);
		this->screenShot = false;
	}
}
Mat CPictureSynthesis::PictureSynthesis()
{
	if (this->changeBg)
	{
		CPictureSynthesis::NextScene();
	}

	resize(this->temp, this->background, Size(colorWidth, colorHeight));   //调整至彩色图像的大小
	
	while (this->myColorReader->AcquireLatestFrame(&this->myColorFrame) != S_OK);   //读取color图
	this->myColorFrame->CopyConvertedFrameDataToArray(this->colorDataSize * 4, this->colorData.data, ColorImageFormat_Bgra);
	this->myColorFrame->Release();

		
	while (this->myDepthReader->AcquireLatestFrame(&this->myDepthframe) != S_OK);   //读取depth图
	this->myDepthframe->CopyFrameDataToArray(this->depthDataSize, this->depthData);
	this->myDepthframe->Release();

		
	while (this->myBodyIndexReader->AcquireLatestFrame(&this->myBodyIndexFrame) != S_OK);
	this->myBodyIndexFrame->CopyFrameDataToArray(this->bodyDataSize, this->bodyData);
	this->myBodyIndexFrame->Release();

	this->copy = this->background.clone();                  //复制一份背景图来做处理
	if (this->myMaper->MapColorFrameToDepthSpace(this->depthDataSize, this->depthData, this->colorDataSize, this->output) == S_OK)
	{
		for (int i = 0; i < this->colorHeight; ++i)
		for (int j = 0; j < this->colorWidth; ++j)
		{
			DepthSpacePoint tPoint = this->output[i * colorWidth + j];    //取得彩色图像上的一点，此点包含了它对应到深度图上的坐标
			if (tPoint.X >= 0 && tPoint.X < this->depthWidth && tPoint.Y >= 0 && tPoint.Y < this->depthHeight)  //判断是否合法
			{
				int index = (int)tPoint.Y * this->depthWidth + (int)tPoint.X; //取得彩色图上那点对应在BodyIndex里的值(注意要强转)
				if (this->bodyData[index] <= 5)                   //如果判断出彩色图上某点是人体，就用它来替换背景图上对应的点
				{
					Vec4b   color = colorData.at<Vec4b>(i, j);
					this->copy.at<Vec3b>(i, j) = Vec3b(color[0], color[1], color[2]);
				}
			}
		}
		//前景添加
		//CPictureSynthesis::AddFrontPhotos();
		//全屏
	//	CPictureSynthesis::FullScreen();	
		this->input = this->copy;
		screen = input;
		//截屏
		CPictureSynthesis::ScreenShot();
	}

	return  this->input;
}
Mat CPictureSynthesis::NextPictureSynthesize()
{
	string strB1 = "../Background/test";
	stringstream streamB;
	streamB << (count_B) % this->photoNumber;
	string strB2 = streamB.str();
	string strB3 = ".jpg";
	string nextBackgroundPath = strB1 + strB2 + strB3;
	Mat nextBackground = imread(nextBackgroundPath);

	string strF1 = "../Outlook/test";
	stringstream streamF;
	streamF << (count_F+1) % this->photoNumber;
	string strF2 = streamF.str();
	string strF3 = ".png";
	string nextFrontPhotosPath = strF1 + strF2 + strF3;
	Mat srcAlpha;
	srcAlpha = imread(nextFrontPhotosPath, -1);
	mapToMat(srcAlpha, nextBackground, 0, 0);

	return nextBackground;
}
Mat CPictureSynthesis::LastPictureSynthesize()
{
	string strB1 = "../Background/test";
	stringstream streamB;
	streamB << (count_B + this->photoNumber - 2) % this->photoNumber;
	string strB2 = streamB.str();
	string strB3 = ".jpg";
	string lastBackgroundPath = strB1 + strB2 + strB3;
	Mat lastBackground = imread(lastBackgroundPath);

	string strF1 = "../Outlook/test";
	stringstream streamF;
	streamF << (count_F + this->photoNumber - 1) % this->photoNumber;
	string strF2 = streamF.str();
	string strF3 = ".png";
	string lastFrontPhotosPath = strF1 + strF2 + strF3;
	Mat srcAlpha;
	srcAlpha = imread(lastFrontPhotosPath, -1);
	mapToMat(srcAlpha, lastBackground, 0, 0);

	return lastBackground;
}