// codedTarget.h : main header file for the CODEDTARGET application
//

#if !defined(AFX_CODEDTARGET_H__D14A188F_EE70_40A0_B7EA_CE5069699E4B__INCLUDED_)
#define AFX_CODEDTARGET_H__D14A188F_EE70_40A0_B7EA_CE5069699E4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCodedTargetApp:
// See codedTarget.cpp for the implementation of this class
//

class CCodedTargetApp : public CWinApp
{
public:
	CCodedTargetApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCodedTargetApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCodedTargetApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CODEDTARGET_H__D14A188F_EE70_40A0_B7EA_CE5069699E4B__INCLUDED_)
