#pragma once
#include "Dlg2.h"

// Choose 对话框

class Choose : public CDialogEx
{
	DECLARE_DYNAMIC(Choose)

public:
	Choose(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~Choose();

// 对话框数据
	enum { IDD = IDD_CHOOSEPRINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();


	BOOL printOrNot;
	BOOL *changed;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	//void showShutScreen2(CImage img);
	void showShutScreen(CImage img,BOOL *weChoose);

public:
	CString printPath;
};
