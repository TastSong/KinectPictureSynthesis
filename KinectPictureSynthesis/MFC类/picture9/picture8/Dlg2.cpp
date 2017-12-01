// Dlg2.cpp : 实现文件
//

#include "stdafx.h"
#include "picture8.h"
#include "Dlg2.h"
#include "afxdialogex.h"
#include "picture8Dlg.h"

#include <Kinect.h>
#include "GestureControl.h"
#include "PictureSynthesis.h"
#include "MouseControl.h"
#include "include.h"

using namespace cv;


// Dlg2 对话框

IMPLEMENT_DYNAMIC(Dlg2, CDialogEx)

Dlg2::Dlg2(CWnd* pParent /*=NULL*/)
	: CDialogEx(Dlg2::IDD, pParent)
{

}
Dlg2::Dlg2(BOOL firstIsPrint)
{
	wetherPrint = firstIsPrint;
}


Dlg2::~Dlg2()
{
}

void Dlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Dlg2, CDialogEx)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &Dlg2::OnBnClickedCancel)
	ON_WM_SIZE()
END_MESSAGE_MAP()


BOOL Dlg2::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//初始化kinect
	kinect = NULL;
	HRESULT hr = S_OK;
	hr = GetDefaultKinectSensor(&kinect);  //  得到默认的设备  

	if (FAILED(hr) || kinect == NULL)
	{
		//cout << "创建 sensor 失败\n";
		//return;
	}
	if (kinect->Open() != S_OK) // 是否打开成功  
	{
		//cout << "Kinect sensor 没准备好\n";
		//return;
	}

	this->gC.Open(kinect);
	this->pS.Open(kinect);
	this->pS.photoNumber = this->numberScenarios;

	// TODO:  在此添加额外的初始化'
	strFilter = L"D:\\screenpicture.jpg";  //初始化strFilter
	GetClientRect(&m_rect);
	
	/*在打印模式下，当弹出带有打印和不打印按钮的三级窗口后，用户可能来不及点击按钮，Ontimer的就被再次调用了，造成不断地弹出三级窗口的后果
	，wetherChoose变量用于标记用户是否点击按钮。wetherChoose保持为false,当弹出三级窗口时，wetherChoose设为true，当点击三级窗口上的按钮后，wetherChoose设位false.*/
	wetherChoose = false;  
	doShutScreen = true;  //用于标记是否截屏
	//wetherPrint = false;    //用于标记是否打印
	
	count = 0;

	 //将二级窗口设为全屏
	allScreen();

	
	SetTimer(1,33,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void Dlg2::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	
	// 不为绘图消息调用 CDialogEx::OnPaint()
}


void Dlg2::drawPicture(void)
{
	
	
	gC.GestureRecognition();
	pS.changeBg = gC.isChangeBg;
	//主区域显示图片
	Mat test = this->pS.PictureSynthesis();   //********************Mat接口***********************************
	//四个导航图片的借口   fuyuanbin(0)
	Mat navigate1 = this->pS.LastPictureSynthesize(); //当前显示的图片前面的前面的图片
	Mat naivgate2 = this->pS.NowPictureSynthesize(); //当前显示的图片前面的图片
	Mat navigate3 = this->pS.NextPictureSynthesize();//当前显示的图片后面的图片
	
	
	//在二级窗口显示图片，包括当前图片和导航区域的四张图片
	ShowInSecondDialog(test,navigate1,naivgate2,navigate3);

	doShutScreen = this->gC.isScreenshot;
	if(doShutScreen)  //doShutScreen是布尔变量，用于标记是否截屏
	{
		SetTimer(2, 800, nullptr);
	}

	if (this->gC.isStop)
	{
		this->getOut();
	}

	doShutScreen = false;
	
	
	
}

//在mID为myId的图片控件中显示IplImage*类型的TheImage
void Dlg2::ShowImage(IplImage* TheImage, UINT myId)
{
	 CDC* pDC=GetDlgItem(myId)->GetDC();
	 HDC hDC=pDC->GetSafeHdc();
	 CRect rect;

	 GetDlgItem(myId)->GetClientRect(&rect);

	 SetRect(rect,rect.left,rect.top,rect.right,rect.bottom);

	 CvvImage cimg;
	 cimg.CopyOf(TheImage);
	 cimg.DrawToHDC(hDC,&rect);
	 ReleaseDC(pDC);
}

//void Dlg2::ResizeImage(IplImage* img)
//{
//	//cvSetImageROI( TheImage, cvRect( 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT));// 对图片 img 进行缩放，并存入到 TheImage 中，相当于做了两件事
//	//cvResize( img, TheImage );// 重置 TheImage 的 ROI 准备读入下一幅图片
//	//cvResetImageROI( TheImage );
//}


Mat Dlg2::fuyuanbin(int order)
{
	char number[6];  //确定路径名
	sprintf(number,"%d",count);
	strcat(number,".jpg");
	
	Mat temp = imread(number,1);
	count = (count + 1) % 7;

	
	return temp;
}


void Dlg2::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		if (!wetherChoose)  //如果在三级窗口中点击了“打印”或“不打印”的按钮，则wetherChoose重新设为false
		{
			this->EnableWindow(true); //二级窗口接收输入消息，即只有在三级窗口中点击按钮，销毁三级窗口后，二机窗口才能接受输入消息
			this->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);  //将二级窗口设置为在最前端显示
			drawPicture();
		}
		break;
	case 2:
		if (wetherPrint)
		{
			showChoose();  //showChoose（）显示截屏的同时，供用户选择打印还是不打印
		}
		else
		{
			//只有照片，3秒关闭
			ShutScreen();  //ShutSceen()函数用于截屏
		}
		KillTimer(2);        //关闭定时器1
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void Dlg2::OnBnClickedCancel()
{
	
	
	
}


void Dlg2::ShutScreen(void)
{
	

	CRect rcPlot;  
			GetDlgItem(IDC_PHOTO)->GetWindowRect(&rcPlot); //获取IDC_PHOTO在整个屏幕中的像素位置,先获取ICD_PHOTO的指针，再根据指针获取IDC_PHOTO控件的大小，存入rcPlot  
			ScreenToClient(&rcPlot);//获取IDC_PHOTO在父窗口中的位置 ,ScreenToClient()是将屏幕坐标转换为客户区坐标 
      
			HDC hDC= ::GetWindowDC(GetDlgItem(IDC_PHOTO)->GetSafeHwnd());  
			HDC hDCMem= ::CreateCompatibleDC(hDC);// 建立和屏幕兼容的bitmap   
      
			HBITMAP hBitmap= ::CreateCompatibleBitmap(hDC,rcPlot.Width(),rcPlot.Height());  //rcPlot是客户区的坐标
			HBITMAP hOldBitmap= (HBITMAP)::SelectObject(hDCMem,hBitmap);////将memBitmap选入内存DC   
      
			::BitBlt(hDCMem,0,0,rcPlot.Width(),rcPlot.Height(),hDC,0,0,SRCCOPY);// 调解高度宽度   
			::SelectObject(hDCMem,hOldBitmap);  
			CImage myImage;  
			myImage.Attach(hBitmap); //将位图加入图片表中  
			
			/*******创建第三级对话框****************/
			this->EnableWindow(false);  //因为三级窗口时模态对话框，所以弹出三级窗口的前，将二级窗口设为不能响应消息
			showpicture *showing = new showpicture();
			showing->Create(IDD_SHUTSCREEN); 
			showing->ShowWindow(SW_SHOW); 
			showing->showShutPicture(myImage);  //调用三级窗口的显示截图函数
			
			

			
}


void Dlg2::showChoose(void)
{
	CRect rcPlot;  
			GetDlgItem(IDC_PHOTO)->GetWindowRect(&rcPlot); //获取IDC_PHOTO在整个屏幕中的像素位置,先获取ICD_PHOTO的指针，再根据指针获取IDC_PHOTO控件的大小，存入rcPlot  
			ScreenToClient(&rcPlot);//获取IDC_PHOTO在父窗口中的位置 ,ScreenToClient()是将屏幕坐标转换为客户区坐标 
      
			HDC hDC= ::GetWindowDC(GetDlgItem(IDC_PHOTO)->GetSafeHwnd());  
			HDC hDCMem= ::CreateCompatibleDC(hDC);// 建立和屏幕兼容的bitmap   
      
			HBITMAP hBitmap= ::CreateCompatibleBitmap(hDC,rcPlot.Width(),rcPlot.Height());  //rcPlot是客户区的坐标
			HBITMAP hOldBitmap= (HBITMAP)::SelectObject(hDCMem,hBitmap);////将memBitmap选入内存DC   
      
			::BitBlt(hDCMem,0,0,rcPlot.Width(),rcPlot.Height(),hDC,0,0,SRCCOPY);// 调解高度宽度   
			::SelectObject(hDCMem,hOldBitmap);  
			CImage myImage;  
			myImage.Attach(hBitmap); //将位图加入图片表中  
			
			/*******创建第三级对话框****************/
			this->EnableWindow(false);  //因为三级窗口时模态对话框，所以弹出三级窗口的前，将二级窗口设为不能响应消息
			Choose *choosing = new Choose();
			choosing->Create(IDD_CHOOSEPRINT); 
			choosing->ShowWindow(SW_SHOW); 
			choosing->showShutScreen(myImage,&wetherChoose);  //调用三级窗口的显示函数来显示截图，并传入布尔变量wetherChoose的地址。当在三级窗口中点击按钮时，改变wetherChoose的值，wetherChoose在计时器OnTimer9)中用来判断二级窗口是否开始接受响应消息
			
			
}


void Dlg2::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(nType!=SIZE_MINIMIZED )  //判断窗口是不是最小化了，因为窗口最小化之后 ，窗口的长和宽会变成0，当前一次变化的时就会出现除以0的错误操作
          {
                  ChangeSize(IDC_PHOTO,cx,cy);  //对每一个控件的大小依次做调整
				  ChangeSize(IDC_NAVIGATE1,cx,cy);
				  ChangeSize(IDC_NAVIGATE2,cx,cy);
				  ChangeSize(IDC_NAVIGATE3,cx,cy);
				 // ChangeSize(IDCANCEL,cx,cy);

					CRect sizeOfPicture;
					CWnd *pWnd = GetDlgItem(IDC_PHOTO);//IDC_picture为picture控件ID
					if(pWnd!=NULL)
					{
						pWnd->GetWindowRect(&sizeOfPicture);//sizeOfPicture为主显示区域图片控件的大小。
						int x=sizeOfPicture.right;
						int y=sizeOfPicture.bottom;

						GetDlgItem(IDC_NAVIGATE1)->SetWindowPos(NULL,0,y,0,0,SWP_NOZORDER | SWP_NOSIZE );  //改变导航区最左边图片的位置，以适应全屏后的二级窗口

						CRect sizeOfnavigate;    //sizeOfnavigate为导航区每个图片控件的大小
						GetDlgItem(IDC_NAVIGATE1)->GetClientRect(&sizeOfnavigate);
						int widthOfNavigate = sizeOfnavigate.right;

						GetDlgItem(IDC_NAVIGATE2)->SetWindowPos(NULL,(x-widthOfNavigate)/2,y,0,0,SWP_NOZORDER | SWP_NOSIZE );  //改变导航区中间图片的位置，以适应全屏后的二级窗口
						GetDlgItem(IDC_NAVIGATE3)->SetWindowPos(NULL,x-widthOfNavigate,y,0,0,SWP_NOZORDER | SWP_NOSIZE );   //改变导航区最右边图片的位置，以适应全屏后的二级窗口
						//GetDlgItem(IDCANCEL)->SetWindowPos(NULL,3*widthOfNavigate,y,0,0,SWP_NOZORDER | SWP_NOSIZE );   //改变导航区最取消按钮的位置，以适应全屏后的二级窗口
						
					}
                  GetClientRect(&m_rect);   //最后要更新对话框的大小，当做下一次变化的旧坐标；
         }
}


void Dlg2::ChangeSize(UINT nID, int xlabel, int ylabel)
{
	CWnd *pWnd;
    pWnd=GetDlgItem(nID); 
    if(pWnd!=NULL)     //判断是否为空，因为在窗口创建的时候也会调用OnSize函数，但是此时各个控件还没有创建，Pwnd为空
    {
            CRect rec;
            pWnd->GetWindowRect(&rec);  //获取控件变化前的大小
            ScreenToClient(&rec);   //将控件大小装换位在对话框中的区域坐标
            rec.left=rec.left*xlabel/m_rect.Width();  //按照比例调整空间的新位置
            rec.top=rec.top*ylabel/m_rect.Height();
            rec.bottom=rec.bottom*ylabel/m_rect.Height();
            rec.right=rec.right*xlabel/m_rect.Width();
            pWnd->MoveWindow(rec);   //伸缩控件
      }
}







void Dlg2::ShowInSecondDialog(Mat total, Mat navi1, Mat navi2, Mat navi3)
{
	//将Mat类型转换为IplImage类型
	IplImage myipl(total); 
	IplImage myipl1(navi1);
	IplImage myipl2(navi2);
	IplImage myipl3(navi3);
	//IplImage myipl4(navi4);

	IplImage *ipl = &myipl;
	IplImage *ipl1 = &myipl1;
	IplImage *ipl2 = &myipl2;
	IplImage *ipl3 = &myipl3;
	//IplImage *ipl4 = &myipl4;
	

	
	if(!ipl||!ipl1||!ipl2||!ipl3)return;
	//二级窗口显示图片
	ShowImage( ipl, IDC_PHOTO );// 利用控件显示图片,传入iplImage*和UNIT类型的参数，在UNIT控件上显示图片，iplIPmage类型决定显示什么图片，函数中需要调整大小与UNIT控件的大小一致
	ShowImage( ipl1, IDC_NAVIGATE1 );
	ShowImage( ipl2, IDC_NAVIGATE2 );
	ShowImage( ipl3, IDC_NAVIGATE3 );
	//ShowImage( ipl4, IDC_NAVIGATE4 );
	//释放内存
	ipl = NULL;
	ipl1 = NULL;
	ipl2 = NULL;
	ipl3 = NULL;
	//ipl4 = NULL;
}

//取消按钮的响应函数里调用的函数，用来销毁二级窗口
void Dlg2::getOut(void)
{
	
	CDialogEx::OnCancel();
}


void Dlg2::allScreen(void)
{
	dialogWidth = GetSystemMetrics(SM_CXSCREEN);
	dialogHeight = GetSystemMetrics(SM_CYSCREEN);
	CRect rcTemp; 
	rcTemp.BottomRight() = CPoint(dialogWidth, dialogHeight); 
	rcTemp.TopLeft() = CPoint(0, 0); 
	MoveWindow(&rcTemp);
}
