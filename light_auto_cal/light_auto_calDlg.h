
// light_auto_calDlg.h : ��� ����
//

#pragma once


#include "gLogger.h"




// Clight_auto_calDlg ��ȭ ����
class Clight_auto_calDlg : public CDialogEx
{
// �����Դϴ�.
public:
	Clight_auto_calDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

	CEvent	EV_auto_light_ctrl;

	double _default_degree = 10;
	double _changed_degree = 0;

	int _target_value = 80;
	int _good_range = 10;
	int _current_val = 0;
	int _before_val = 0;

	bool bTestMode = 0;//������� ȯ��
	
	bool _bFirstStepDone = false;

	gLogger* _glogger;

	void Clight_auto_calDlg::CheckLightVal();
	void Clight_auto_calDlg::DoProcess(int corr_vals);
	int Clight_auto_calDlg::GetDiffValFromTarget(int current_val, int target_val);
	void Clight_auto_calDlg::SetLight(double val);
// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIGHT_AUTO_CAL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

	

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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
