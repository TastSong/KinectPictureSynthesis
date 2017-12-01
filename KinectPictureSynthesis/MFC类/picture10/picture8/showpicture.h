#pragma once


// showpicture 对话框

class showpicture : public CDialogEx
{
	DECLARE_DYNAMIC(showpicture)

public:
	showpicture(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~showpicture();

// 对话框数据
	enum { IDD = IDD_SHUTSCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	void showShutPicture(CImage img);
	virtual BOOL OnInitDialog();
	CImage globalImage;
	
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
