
// light_auto_cal.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// Clight_auto_calApp:
// �� Ŭ������ ������ ���ؼ��� light_auto_cal.cpp�� �����Ͻʽÿ�.
//

class Clight_auto_calApp : public CWinApp
{
public:
	Clight_auto_calApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern Clight_auto_calApp theApp;