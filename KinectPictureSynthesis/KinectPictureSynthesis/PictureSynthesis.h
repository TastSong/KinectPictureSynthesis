#pragma once

#include "include.h"

class CPictureSynthesis
{
public:
	CPictureSynthesis();
	~CPictureSynthesis();
private:
	//kinect设备
	IKinectSensor* kinect;
	//彩色数据
	int colorHeight, colorWidth;
	IColorFrameSource* myColorSource;
	IColorFrameReader* myColorReader;
	IFrameDescription* myDescription;
	UINT    colorDataSize;
	DepthSpacePoint* output;
	IColorFrame* myColorFrame;
	//深度数据
	int depthHeight, depthWidth;
	IDepthFrameSource* myDepthSource;
	IDepthFrameReader* myDepthReader;
	UINT    depthDataSize;
	UINT16* depthData;
	IDepthFrame* myDepthframe;
	//身体索引框架数据
	int bodyHeight, bodyWidth;
	IBodyIndexFrameSource* myBodyIndexSource;
	IBodyIndexFrameReader* myBodyIndexReader;
	UINT    bodyDataSize = 0;
	BYTE* bodyData;
	IBodyIndexFrame* myBodyIndexFrame;
	//开启mapper
	ICoordinateMapper* myMaper;  
	//背景图片计数
	int count_B;  
	//前景图片计数
	int count_F;  

	Mat colorData;
	//读入背景图像
	Mat temp;
	//修改大小后的背景图像
	Mat background;
	//拷贝一份背景用作图像处理
	Mat copy;
	//最终要输出的图像
	Mat input;
	//截屏图像
	Mat screen;

public:
	//照片张数
	int photoNumber;
	//判断是否切换到下一个场景
	bool changeBg;
	//判断是否截屏
	bool screenShot;

private:
	//透明前景添加
	void mapToMat(const cv::Mat &srcAlpha, cv::Mat &dest, int x, int y);
	//开启彩色帧
	bool OpenColor();
    //开启深度帧
	bool OpenDepth();
	//开启身体索引框架帧
	bool OpenBodyIndex();
	//切换到下一个场景
	void NextScene();
	//全屏化
	void FullScreen();
	//添加前景
	void AddFrontPhotos();
	//截屏保存
	void ScreenShot();

public:
	//打开三种镜头并开启相机
	bool Open(IKinectSensor* kinect);
	Mat PictureSynthesis();
	Mat NextPictureSynthesize();
	Mat LastPictureSynthesize();
};

