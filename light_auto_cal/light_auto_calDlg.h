
// light_auto_calDlg.h : 헤더 파일
//

#pragma once


#include "gLogger.h"




// Clight_auto_calDlg 대화 상자
class Clight_auto_calDlg : public CDialogEx
{
// 생성입니다.
public:
	Clight_auto_calDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	CEvent	EV_auto_light_ctrl;

	double _default_degree = 10;
	double _changed_degree = 0;

	int _target_value = 80;
	int _good_range = 10;
	int _current_val = 0;
	int _before_val = 0;

	bool bTestMode = 0;//조명없는 환경
	
	bool _bFirstStepDone = false;

	gLogger* _glogger;

	void Clight_auto_calDlg::CheckLightVal();
	void Clight_auto_calDlg::DoProcess(int corr_vals);
	int Clight_auto_calDlg::GetDiffValFromTarget(int current_val, int target_val);
	void Clight_auto_calDlg::SetLight(double val);
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIGHT_AUTO_CAL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};
