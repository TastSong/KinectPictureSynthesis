#pragma once
#include "CvvImage.h"
#include "showpicture.h" //打印模式下三级窗口的头文件
//#include "picture8Dlg.h"
#include "Choose.h"  //非打印模式下的三级窗口的头文件

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <Kinect.h>
#include "GestureControl.h"
#include "PictureSynthesis.h"
#include "MouseControl.h"
#include "include.h"
using namespace cv;


// Dlg2 对话框

class Dlg2 : public CDialogEx
{
	DECLARE_DYNAMIC(Dlg2)

public:
	Dlg2(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Dlg2();

// 对话框数据
	enum { IDD = IDD_SECOND_DEGREE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
//	virtual void PreInitDialog();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	CString strFilter; //打印路径
	
	BOOL wetherChoose;
	BOOL doShutScreen;//判断是否需要截屏
	
	BOOL wetherPrint;   //二级窗口用于标记自己是否为打印模式，其值由一级窗口指定
	int count;   //在fuyuanbin()函数里，用于读入jpg图片以测试Mat接口，可用fuyuanbin()函数可用kinect的函数取代
	void drawPicture(void);  
	void ShowImage(IplImage* TheImage, UINT myId);   //二级窗口显示图片的函数
//	void ResizeImage(IplImage* img);
	Mat fuyuanbin(int order);
	int dialogWidth;
	int dialogHeight;
	CRect m_rect;



	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
	Dlg2(BOOL firstIsPrint);
	void ShutScreen(void);
	void showChoose(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void ChangeSize(UINT nID, int xlabel, int ylabel);

	void ShowInSecondDialog(Mat total, Mat navi1, Mat navi2, Mat navi3);
	void getOut(void);
	void allScreen(void);

public:
	IKinectSensor *kinect = NULL;
	CGestureControl gC;
	CPictureSynthesis pS;
	int numberScenarios;
};
