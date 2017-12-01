
// picture8Dlg.h : 头文件
//

#pragma once
#include "Dlg2.h" //将二级窗口的头文件包括进来

#include <Kinect.h>
#include "MouseControl.h"
#include "include.h"
// Cpicture8Dlg 对话框
class Cpicture8Dlg : public CDialogEx
{
// 构造
public:
	Cpicture8Dlg(CWnd* pParent = NULL);	// 标准构造函数
	int isPrint;   //标记是否进入打印模式
	//Dlg2 *myDlg;   //创建二级窗口的对象
	//Dlg2 myDlg; 
	 

// 对话框数据
	enum { IDD = IDD_PICTURE8_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButton1();
//	void doDrawPicture(void);
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	IKinectSensor *kinect = NULL;
	CMouseControl mC;
	int m_nNumberScenarios;
};
