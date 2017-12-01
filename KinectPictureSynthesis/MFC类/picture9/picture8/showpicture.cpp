// showpicture.cpp : 实现文件
//

#include "stdafx.h"
#include "picture8.h"
#include "showpicture.h"
#include "afxdialogex.h"


// showpicture 对话框

IMPLEMENT_DYNAMIC(showpicture, CDialogEx)

showpicture::showpicture(CWnd* pParent /*=NULL*/)
	: CDialogEx(showpicture::IDD, pParent)
{
	
}

showpicture::~showpicture()
{
}

void showpicture::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(showpicture, CDialogEx)
	ON_WM_TIMER()
	ON_WM_TIMER()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// showpicture 消息处理程序


void showpicture::showShutPicture(CImage img)
{
	
	CRect rect;//定义矩形类   
				GetDlgItem(IDC_SHUTPICTURE)->GetWindowRect(&rect);//将窗口矩形选中到picture控件上   
				ScreenToClient(&rect);//将客户区选中到Picture控件表示的矩形区域内   
				  
				GetDlgItem(IDC_SHUTPICTURE)->MoveWindow(rect);//将窗口移动到Picture控件表示的矩形区域   
  
				CWnd *pWnd=GetDlgItem(IDC_SHUTPICTURE);//获得pictrue控件窗口的句柄   
  
				pWnd->GetClientRect(&rect);//获得pictrue控件所在的矩形区域   
  
				CDC *pDC=pWnd->GetDC();//获得pictrue控件的DC   
				
				CImage imgs;
				//imgs.Load(L"D:\\screenpicture.jpg");
				img.Draw(pDC->m_hDC, rect); //将图片画到Picture控件表示的矩形区域  

				time_t tt = time(NULL);//这句返回的只是一个时间cuo
				tm* t = localtime(&tt);
				CString strTime;
				strTime.Format(_T("%02d%02d%02d"), t->tm_hour, t->tm_min, t->tm_sec);
				CString screenshotPath;
				screenshotPath = _T("../Screenshot/") + strTime + _T(".jpg");

				img.Save(screenshotPath);
				ReleaseDC(pDC);//释放picture控件的DC

				Sleep(3000);

				//连接打印机进行照片打印
				char szprinter[80];
				char* szDevice, *szDriver, *szOutput;
				HDC hdcprint; // 定义一个设备环境句柄
				//定义一个打印作业
				static DOCINFO di = { sizeof(DOCINFO), _T("printer"), NULL };
				// 得到设备字符串存入数组szprinter中
				GetProfileString((LPTSTR)(LPCTSTR)"windows", (LPTSTR)(LPCTSTR)"device", (LPTSTR)(LPCTSTR)",,,", (LPTSTR)(LPCTSTR)szprinter, 80);
				// 将设备字符串分解
				if (NULL != (szDevice = strtok(szprinter, ",")) && NULL != (szDriver = strtok(NULL, ",")) &&
					NULL != (szOutput = strtok(NULL, ",")))
				{
					// 创建一个打印机设备句柄	
					if ((hdcprint = CreateDC((LPWSTR)szDriver, (LPWSTR)szDevice, (LPWSTR)szOutput, NULL)) != 0)
					{
						if (StartDoc(hdcprint, &di) > 0) //开始执行一个打印作业
						{
							StartPage(hdcprint); //打印机走纸,开始打印
							SaveDC(hdcprint); //保存打印机设备句柄
							// 输出一行文字
							Escape(hdcprint, STARTDOC, 8, (LPCSTR)(LPCTSTR)screenshotPath, NULL);
							RestoreDC(hdcprint, -1); //恢复打印机设备句柄
							EndPage(hdcprint); //打印机停纸,停止打印
							EndDoc(hdcprint); //结束一个打印作业
							MessageBox(_T("打印完毕!"), _T("提示"), MB_OK | MB_SYSTEMMODAL);
						}
						// 用API函数DeleteDC销毁一个打印机设备句柄 
						DeleteDC(hdcprint);

						CDialogEx::OnCancel();
					}
				}
				else
				{
					MessageBox(_T("没有默认打印机,或者没有安装打印机!"), _T("OK"), MB_OK | MB_SYSTEMMODAL);

					CDialogEx::OnCancel();
					return;
				}

				CDialogEx::OnCancel();
}


BOOL showpicture::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect cr;  
    GetClientRect(&cr);//获取对话框客户区域大小   
    ClientToScreen(&cr);//转换为荧幕坐标  
 
    //showShutPicture(globalImage);
	//SetTimer(1,3000,NULL);
	int xx = GetSystemMetrics(SM_CXSCREEN);
	int yy = GetSystemMetrics(SM_CYSCREEN);
	
	SetWindowPos(NULL,xx/3,yy/3,0,0,SWP_NOZORDER | SWP_NOSIZE );
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}





void showpicture::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

		showShutPicture(globalImage);
	
		
	CDialogEx::OnTimer(nIDEvent);
}
