// codedTargetDlg.cpp : implementation file
//

#include "stdafx.h"
#include "codedTarget.h"
#include "codedTargetDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCodedTargetDlg dialog

CCodedTargetDlg::CCodedTargetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCodedTargetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCodedTargetDlg)
	m_target_num = 0;
	m_time = 0.0f;
	m_gray = 0;
	m_dia = 0;
	m_ck46 = -1;
	m_ck13 = -1;
	m_sn = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_init=false;

	AllocConsole();				freopen("CONOUT$","w+t",stdout);

}

void CCodedTargetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCodedTargetDlg)
	DDX_Text(pDX, IDC_EDIT2, m_target_num);
	DDX_Text(pDX, IDC_EDIT1, m_time);
	DDX_Text(pDX, IDC_EDIT5, m_gray);
	DDX_Text(pDX, IDC_EDIT6, m_dia);
	DDX_Radio(pDX, IDC_RADIO4, m_ck46);
	DDX_Radio(pDX, IDC_RADIO1, m_ck13);
	DDX_Text(pDX, IDC_EDIT7, m_sn);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCodedTargetDlg, CDialog)
	//{{AFX_MSG_MAP(CCodedTargetDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_RADIO4, OnRadio4)
	ON_BN_CLICKED(IDC_RADIO5, OnRadio5)
	ON_BN_CLICKED(IDC_RADIO6, OnRadio6)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON8, OnButton8)
	ON_BN_CLICKED(IDC_BUTTON9, OnButton9)
	ON_BN_CLICKED(IDC_BUTTON10, OnButton10)
	ON_BN_CLICKED(IDC_BUTTON11, OnButton11)
	ON_BN_CLICKED(IDC_BUTTON12, OnButton12)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCodedTargetDlg message handlers

BOOL CCodedTargetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	this->CheckRadioButton(IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);
	this->CheckRadioButton(IDC_RADIO4, IDC_RADIO6, IDC_RADIO5);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCodedTargetDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCodedTargetDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//载入图像和初始化
void CCodedTargetDlg::OnButton1() 
{
	CString cs;
	cs=OpenImage("load target image ");

	if (cs=="flase")
	{
		AfxMessageBox("载入图像失败！");
		return;

	}

///////////
	m_img=cvLoadImage(cs,0);

	CvSize sz=cvGetSize(m_img);

	int h=sz.height;
	int w=sz.width;

	if (  w%4 != 0 )
	{
//		AfxMessageBox("载入图像失败！图像宽应能被4整除！");
//		cvReleaseImage( &m_img);

//		return;
	}

	Initial(sz);

	m_ct.Initial(w, h);			// 检测初始化


	cvNamedWindow( "img", 0);
	cvResizeWindow("img",m_ww, m_wh);

	cvShowImage( "img", m_img);

	
}

CString CCodedTargetDlg::OpenImage(char *title)
{
	CFileDialog dlg(TRUE, _T("*.bmp"), "", 
		OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,
		"image files (*.*) |*.*|All Files (*.*)|*.*||",NULL);
		
	dlg.m_ofn.lpstrTitle= title;
	
	if (dlg.DoModal() == IDOK) 
	{
		
		return( dlg.GetPathName());  // selected filename
	}
	else 
		return "flase";

}

void CCodedTargetDlg::Initial(CvSize sz)
{
	int y=sz.height;
	int x=sz.width;

	int t=1200;
	if ( y > t )
	{
		float rt=t/(float)y;

		m_wh=t;
		m_ww=(float )x*rt;
	}
	else
	{
		m_wh=y;
		m_ww=x;

	}


	m_init=true;

}


BOOL CCodedTargetDlg::DestroyWindow() 
{
	FreeConsole();
	
	if (m_init)
	{
		cvReleaseImage( &m_img);

	}
 	
	return CDialog::DestroyWindow();
}

//一步解码
void CCodedTargetDlg::OnButton2() 
{
	if ( !m_init)
	{
		AfxMessageBox("请先载入图像！");
		return;
	}

	this->UpdateData(1);

clock_t start, finish;	
start = clock();

	m_ct.Decoding_oneStep(m_img);	//一步解码

finish = clock();				
m_time = (double)(finish - start) / CLOCKS_PER_SEC;

	m_ct.Draw_target();			//作图

////

	Output();

	int thresh=m_ct.m_thresh;	//二值化方法
	if ( thresh == 0)
	{
		m_ck46=0;

		GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
	}
	else if ( thresh == 2)
	{
//		CheckRadioButton(IDC_RADIO4, IDC_RADIO6, IDC_RADIO6);
		m_ck46=2;

	}

	this->UpdateData(0);

	printf("target num=%d, \n", m_ct.m_targetNum);

	for (int i=0; i<m_ct.m_targetNum; i++ )
	{
		printf("target code=%d, sn=%d, x=%f, y=%f, \n", m_ct.m_target[i].code, m_ct.m_target[i].sn, 
				m_ct.m_target[i].x, m_ct.m_target[i].y);

	}

/////

	cvNamedWindow( "target", 0 );
	cvResizeWindow("target",m_ww, m_wh);

	cvShowImage( "target", m_ct.m_imgc);

}

void CCodedTargetDlg::Output()
{
	m_gray=m_ct.m_th;

	m_dia=m_ct.m_dia;

	m_target_num=m_ct.m_targetNum;

	this->UpdateData(0);

}

//保存结果图
void CCodedTargetDlg::OnButton3() 
{
	if ( !m_init)
		return;

	CString s;

	CTime tm;
	tm=CTime::GetCurrentTime();

	s.Format(_T("%d_%d_%d_%d_%d.bmp"), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(), tm.GetSecond());
	
	cvSaveImage(s, m_ct.m_imgc);

	AfxMessageBox("保存结果图成功！");
	
}

//保存坐标文件
void CCodedTargetDlg::OnButton4() 
{
	if ( !m_init)
		return;

	if ( m_ct.m_targetNum < 1 )
		return;
	
	CString s;

	CTime tm;
	tm=CTime::GetCurrentTime();

	s.Format(_T("%d_%d_%d_%d_%d.txt"), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(), tm.GetSecond());

	FILE	*fp= NULL;

	fp=fopen(s, "w");

	for (int i=0; i<m_ct.m_targetNum; i++ )
	{
		fprintf(fp, "target code=%d, sn=%d, x=%f, y=%f, \n", m_ct.m_target[i].code, m_ct.m_target[i].sn, 
				m_ct.m_target[i].x, m_ct.m_target[i].y);

	}

	fclose(fp);

	AfxMessageBox("保存坐标文件成功！");
	
}

//自动设置参数
void CCodedTargetDlg::OnButton5() 
{
	if ( !m_init)
		return;

	this->UpdateData(1);

	int ck=m_ck13;	

	int mode;
	if ( ck == 0)
		mode=0;
	else if ( ck == 1)
	{
		mode=1;
	}
	else
	{
		mode=2;
	}
		
	m_ct.Get_adaptive_parameter(m_img, mode);	//获取参数

	m_gray=m_ct.m_th;
	m_dia=m_ct.m_dia;


	int thresh=m_ct.m_thresh;	//二值化方法
	if ( thresh == 0)
	{

		m_ck46=0;

		GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
	}
	else if ( thresh == 2)
	{
//		CheckRadioButton(IDC_RADIO4, IDC_RADIO6, IDC_RADIO6);
		m_ck46=2;
	}
	
	this->UpdateData(0);

}


void CCodedTargetDlg::OnRadio4() 
{
	int ck=GetCheckedRadioButton(IDC_RADIO4, IDC_RADIO6);

	if ( ck == IDC_RADIO4)
		GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT5)->EnableWindow(false);

 	
}

void CCodedTargetDlg::OnRadio5() 
{
	int ck=GetCheckedRadioButton(IDC_RADIO4, IDC_RADIO6);

	if ( ck == IDC_RADIO4)
		GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT5)->EnableWindow(false);
 	
}

void CCodedTargetDlg::OnRadio6() 
{
	 int ck=GetCheckedRadioButton(IDC_RADIO4, IDC_RADIO6);

	if ( ck == IDC_RADIO4)
		GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_EDIT5)->EnableWindow(false);
}

//解码
void CCodedTargetDlg::OnButton7() 
{

	if ( !m_init)
	{
		AfxMessageBox("请先载入图像！");
		return;
	}
////

	this->UpdateData(1);

	int ck=GetCheckedRadioButton(IDC_RADIO4, IDC_RADIO6);

	if ( ck == IDC_RADIO4)
	{
		m_ct.m_thresh=0;			//二值化方法
		m_ct.m_th=GetDlgItemInt(IDC_EDIT5);		//固定阈值
		
	}
	else if ( ck == IDC_RADIO5)
	{
		m_ct.m_thresh=1;;
	}
	else
	{
		m_ct.m_thresh=2;;

	}

	m_ct.m_dia=GetDlgItemInt(IDC_EDIT6);		//中心圆直径

/////

clock_t start, finish;	
start = clock();

	m_ct.Decoding(m_img);	//解码 	m_ct.Decoding_oneStep(m_img)

finish = clock();				
m_time = (double)(finish - start) / CLOCKS_PER_SEC;


	m_ct.Draw_target();			//作图

////

	Output();

	printf("target num=%d, \n", m_ct.m_targetNum);

	for (int i=0; i<m_ct.m_targetNum; i++ )
	{
		printf("target code=%d, sn=%d, x=%f, y=%f, \n", m_ct.m_target[i].code, m_ct.m_target[i].sn, 
				m_ct.m_target[i].x, m_ct.m_target[i].y);

	}

/////

	cvNamedWindow( "target", 0 );
	cvResizeWindow("target",m_ww, m_wh);

	cvShowImage( "target", m_ct.m_imgc);
	
}

//解码, 目标尺寸（中心圆直径）无限制  
void CCodedTargetDlg::OnButton8() 
{
	if ( !m_init)
	{
		AfxMessageBox("请先载入图像！");
		return;
	}
////

	this->UpdateData(1);

	int ck=GetCheckedRadioButton(IDC_RADIO4, IDC_RADIO6);

	if ( ck == IDC_RADIO4)
	{
		m_ct.m_thresh=0;			//二值化方法
		m_ct.m_th=GetDlgItemInt(IDC_EDIT5);		//固定阈值
		
	}
	else if ( ck == IDC_RADIO5)
	{
		m_ct.m_thresh=1;;
	}
	else
	{
		m_ct.m_thresh=2;;

	}

//	m_ct.m_dia=GetDlgItemInt(IDC_EDIT6);		//中心圆直径

/////

clock_t start, finish;	
start = clock();

	m_ct.Decoding_unlimitedSize(m_img);	 //解码 	m_ct.Decoding_oneStep(m_img), 	m_ct.Decoding(m_img);

finish = clock();				
m_time = (double)(finish - start) / CLOCKS_PER_SEC;


	m_ct.Draw_target();			//作图

////

	Output();

	printf("target num=%d, \n", m_ct.m_targetNum);

	for (int i=0; i<m_ct.m_targetNum; i++ )
	{
		printf("target code=%d, sn=%d, x=%f, y=%f, \n", m_ct.m_target[i].code, m_ct.m_target[i].sn, 
				m_ct.m_target[i].x, m_ct.m_target[i].y);

	}

/////

	cvNamedWindow( "target", 0 );
	cvResizeWindow("target",m_ww, m_wh);

	cvShowImage( "target", m_ct.m_imgc);
	
	
}


void CCodedTargetDlg::OnButton9() 
{
	if ( !m_init)
		return;

	this->UpdateData(1);

	int ck=GetCheckedRadioButton(IDC_RADIO4, IDC_RADIO6);

	if ( ck == IDC_RADIO4)
	{
		m_ct.m_thresh=0;			//二值化方法
		m_ct.m_th=GetDlgItemInt(IDC_EDIT5);		//固定阈值
		
	}
	else if ( ck == IDC_RADIO5)
	{
		m_ct.m_thresh=1;;
	}
	else
	{
		m_ct.m_thresh=2;;

	}

	m_ct.m_dia=GetDlgItemInt(IDC_EDIT6);		//中心圆直径

	m_ct.m_img_orig=m_img;

	m_ct.Binary(m_ct.m_th);

	cvNamedWindow( "imgbw", 0 );
	cvResizeWindow("imgbw",m_ww, m_wh);

	cvShowImage( "imgbw", m_ct.m_img_bw);

	
}

void CCodedTargetDlg::OnButton10() 
{
if ( !m_init)
		return;

	if (!m_ct.m_bw)
	{
		AfxMessageBox("请先执行二值化！");
		return;

	}
	
	CvSize sz=cvGetSize(m_img);
	int h=sz.height;
	int w=sz.width;

	UpdateData(1);
	m_ct.m_dia=this->m_dia;

	m_ct.CenterCircle(m_ct.m_img_bw);

	m_ct.Draw_circle();

	m_target_num=m_ct.m_circleNum;

	UpdateData(0);

	cvNamedWindow( "imgcircle", 0 );
	cvResizeWindow("imgcircle",m_ww, m_wh);

	cvShowImage( "imgcircle", m_ct.m_imgc);

 	
}

void CCodedTargetDlg::OnButton11() 
{
	if ( !m_init)
		return;

	if (!m_ct.m_bw)
	{
		AfxMessageBox("请先执行二值化！");
		return;

	}

	this->UpdateData(1);

	m_ct.Rectifying(m_sn);

	cvNamedWindow( "imgtarget", 0 );
//	cvResizeWindow("img4",m_ww, m_wh);

	cvShowImage( "imgtarget", m_ct.m_imgt);
 	
}

void CCodedTargetDlg::OnButton12() 
{
	if ( !m_init)
		return;

	if (!m_ct.m_bw)
	{
		AfxMessageBox("请先执行二值化！");
		return;

	}
	this->UpdateData(1);

	int ck=GetCheckedRadioButton(IDC_RADIO4, IDC_RADIO6);

	if ( ck == IDC_RADIO4)
	{
		m_ct.m_thresh=0;			//二值化方法
		m_ct.m_th=GetDlgItemInt(IDC_EDIT5);		//固定阈值
		
	}
	else if ( ck == IDC_RADIO5)
	{
		m_ct.m_thresh=1;;
	}
	else
	{
		m_ct.m_thresh=2;;

	}

	m_ct.m_dia=GetDlgItemInt(IDC_EDIT6);		//中心圆直径

/////
	int t=0;
//		if (m_ct.m_thresh == 2)
			t=m_ct.Target_gray(m_ct.m_imgt, m_ct.m_rad);

//	t=157;

	int cd=m_ct.Decode(m_ct.m_imgt, m_ct.m_rad, t);


	printf("code = %d , th=%d, \n", cd, t);

	
}
