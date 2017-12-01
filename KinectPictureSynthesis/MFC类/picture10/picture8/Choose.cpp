// Choose.cpp : 实现文件
//

#include "stdafx.h"
#include "picture8.h"
#include "picture8Dlg.h"
#include "Choose.h"
#include "afxdialogex.h"




// Choose 对话框

IMPLEMENT_DYNAMIC(Choose, CDialogEx)

Choose::Choose(CWnd* pParent /*=NULL*/)
	: CDialogEx(Choose::IDD, pParent)
{

}

Choose::~Choose()
{
}

void Choose::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Choose, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &Choose::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &Choose::OnBnClickedCancel)
END_MESSAGE_MAP()


// Choose 消息处理程序


void Choose::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	printOrNot = true;
	this->ShowWindow(SW_HIDE);
	if(changed!=NULL)
		*changed = false;
	//changed = NULL;

	//连接打印机进行照片打印
	char szprinter[80];
	char* szDevice, * szDriver, * szOutput;
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
				Escape(hdcprint, STARTDOC, 8, (LPCSTR)(LPCTSTR)this->printPath, NULL);
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
		MessageBox( _T("没有默认打印机,或者没有安装打印机!"), _T("OK"), MB_OK | MB_SYSTEMMODAL);

		CDialogEx::OnCancel();
		return;
	}

	CDialogEx::OnCancel();
}


BOOL Choose::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect cr;  
    GetClientRect(&cr);//获取对话框客户区域大小   
    ClientToScreen(&cr);//转换为荧幕坐标 
	
	printOrNot = false;
	changed = NULL;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void Choose::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	printOrNot = true;
	this->ShowWindow(SW_HIDE);
	if(changed!=NULL)
		*changed = false;
	//changed =NULL;
	CDialogEx::OnCancel();
}


//void Choose::showShutScreen2(CImage img)
//{
//	CRect rect;//定义矩形类   
//				
//				  
//				
//
//
//				GetDlgItem(IDC_CHOOSESCREEN)->GetWindowRect(&rect);//将窗口矩形选中到picture控件上   
//				ScreenToClient(&rect);//将客户区选中到Picture控件表示的矩形区域内    
//
//
//				GetDlgItem(IDC_CHOOSESCREEN)->MoveWindow(rect);//将窗口移动到Picture控件表示的矩形区域   
//  
//				CWnd *pWnd=GetDlgItem(IDC_CHOOSESCREEN);//获得pictrue控件窗口的句柄   
//  
//				pWnd->GetClientRect(&rect);//获得pictrue控件所在的矩形区域   
//  
//				CDC *pDC=pWnd->GetDC();//获得pictrue控件的DC   
//				
//				img.Draw(pDC->m_hDC, rect); //将图片画到Picture控件表示的矩形区域  
//				img.Save(L"D:\\screenpicture.jpg");
//				ReleaseDC(pDC);//释放picture控件的DC
//
//				
//}


void Choose::showShutScreen(CImage img,BOOL *weChoose)
{
	changed = weChoose;
	*changed = true;
	CRect rect;//定义矩形类   
				GetDlgItem(IDC_CHOOSESCREEN)->GetWindowRect(&rect);//将窗口矩形选中到picture控件上   
				ScreenToClient(&rect);//将客户区选中到Picture控件表示的矩形区域内    


				GetDlgItem(IDC_CHOOSESCREEN)->MoveWindow(rect);//将窗口移动到Picture控件表示的矩形区域   
  
				CWnd *pWnd=GetDlgItem(IDC_CHOOSESCREEN);//获得pictrue控件窗口的句柄   
  
				pWnd->GetClientRect(&rect);//获得pictrue控件所在的矩形区域   
  
				CDC *pDC=pWnd->GetDC();//获得pictrue控件的DC   
				
				img.Draw(pDC->m_hDC, rect); //将图片画到Picture控件表示的矩形区域  

				time_t tt = time(NULL);//这句返回的只是一个时间cuo
				tm* t = localtime(&tt);
				CString strTime;
				strTime.Format(_T("%02d%02d%02d"), t->tm_hour, t->tm_min, t->tm_sec);
				CString screenshotPath;
				screenshotPath = _T("../Screenshot/") + strTime + _T(".jpg");

				this->printPath = screenshotPath;

				img.Save(screenshotPath);
				ReleaseDC(pDC);//释放picture控件的DC
		
}
