// codedTargetDlg.h : header file
//

#if !defined(AFX_CODEDTARGETDLG_H__5C55C04F_6F81_40AC_8111_B058F1643F17__INCLUDED_)
#define AFX_CODEDTARGETDLG_H__5C55C04F_6F81_40AC_8111_B058F1643F17__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCodedTargetDlg dialog

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "_codedTarget.h"


class CCodedTargetDlg : public CDialog
{
// Construction
public:
	void Output();
	void Initial(CvSize sz);
	CString OpenImage(char *title);
	CCodedTargetDlg(CWnd* pParent = NULL);	// standard constructor

	IplImage * m_img;   //输入图像
	bool m_init;

	CodedTarget m_ct;   //标志检测对象

	int m_wh;
	int m_ww;

// Dialog Data
	//{{AFX_DATA(CCodedTargetDlg)
	enum { IDD = IDD_CODEDTARGET_DIALOG };
	int		m_target_num;
	float	m_time;
	int		m_gray;
	int		m_dia;
	int		m_ck46;
	int		m_ck13;
	int		m_sn;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCodedTargetDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCodedTargetDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnRadio4();
	afx_msg void OnRadio5();
	afx_msg void OnRadio6();
	afx_msg void OnButton7();
	afx_msg void OnButton8();
	afx_msg void OnButton9();
	afx_msg void OnButton10();
	afx_msg void OnButton11();
	afx_msg void OnButton12();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CODEDTARGETDLG_H__5C55C04F_6F81_40AC_8111_B058F1643F17__INCLUDED_)
