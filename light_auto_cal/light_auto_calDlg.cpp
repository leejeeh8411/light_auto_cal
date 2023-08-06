
// light_auto_calDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "light_auto_cal.h"
#include "light_auto_calDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT ThreadAutoLightControl (LPVOID lParam)
{
	Clight_auto_calDlg *pView = (Clight_auto_calDlg *)lParam;

	DWORD dwRet = 0;

	while (1) {
		dwRet = WaitForSingleObject((HANDLE)pView->EV_auto_light_ctrl, INFINITE);
		if (dwRet == WAIT_FAILED) {
			return 0;
		}
		else if (dwRet == WAIT_ABANDONED) {
			ResetEvent(pView->EV_auto_light_ctrl);
		}
		else if (dwRet == WAIT_TIMEOUT) {
			continue;
		}
		else {
			pView->CheckLightVal();
		}
		Sleep(1);
	}
	return 0;
}
// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Clight_auto_calDlg ��ȭ ����

Clight_auto_calDlg::Clight_auto_calDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LIGHT_AUTO_CAL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Clight_auto_calDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Clight_auto_calDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &Clight_auto_calDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &Clight_auto_calDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &Clight_auto_calDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &Clight_auto_calDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &Clight_auto_calDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// Clight_auto_calDlg �޽��� ó����

BOOL Clight_auto_calDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	_glogger = new gLogger("suslog", std::string("D:\\Log\\") + std::string("syslog.txt"), false, 23, 59);

	AfxBeginThread(ThreadAutoLightControl, this, THREAD_PRIORITY_HIGHEST, 0);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void Clight_auto_calDlg::CheckLightVal()
{
	int current_val = _current_val;

	int corr_val = GetDiffValFromTarget(current_val, _target_value);

	CString strLog;
	strLog.Format("CheckLightVal ���ذ� ������� : %d", corr_val);
	_glogger->info(strLog);

	if (abs(corr_val) > _good_range){
		//�����ʿ�
		DoProcess(corr_val);
	}
	else {
		//good range
		_bFirstStepDone = false;
		strLog.Format("CheckLightVal GoodRange : %d", corr_val);
		_glogger->info(strLog);
	}
}

void Clight_auto_calDlg::DoProcess(int corr_vals)
{
	CString strLog;
	strLog.Format("DoProcess ������:%d, ���ذ� ������� ��:%d", _current_val, corr_vals);
	_glogger->info(strLog);

	int changed_val = abs(_before_val - _current_val);

	//�����Ⱚ�� �ϴ� ����
	_before_val = _current_val;

	if (_bFirstStepDone == false) {
		//first step
		//����Ʈ degree�� ��ȭ ���Ѻ���
		

		//������ ����-����üũ
		double change_degree = 0;
		if (corr_vals > 0) {
			change_degree = +(_default_degree);
		}
		else {
			change_degree = -(_default_degree);
		}
		_changed_degree = change_degree;

		strLog.Format("DoProcess FirstStep ������:%0.2f", _changed_degree);
		_glogger->info(strLog);

		//���� ������ set
		SetLight(_changed_degree);

		_bFirstStepDone = true;
	}
	else {
		//second step
		//����Ʈ degree�� ��ȭ������ �� ��ȭ���� ���� ok range�� ���� �� ���� ����
		
		//degree 1�� ���ϴ� ���
		double std_val = abs(_changed_degree )/ abs(changed_val);

		strLog.Format("DoProcess SecondStep degree 1�� ���ϴ� ��Ⱚ:%0.2f", std_val);
		_glogger->info(strLog);

		double final_val = corr_vals / std_val;

		_changed_degree = final_val;

		SetLight(final_val);
	}
}

//+�� ���� �÷��ߵ�, -�� ���� �����ߵ�.
int Clight_auto_calDlg::GetDiffValFromTarget(int current_val, int target_val)
{
	int ret_val = 0;

	if (current_val < target_val - _good_range) {
		ret_val = target_val - current_val;
	}
	else if(current_val > target_val + _good_range){
		ret_val = target_val - current_val;
	}
	else {
		ret_val = 0;
	}

	return ret_val;
}

void Clight_auto_calDlg::SetLight(double val)
{
	CString strLog;
	strLog.Format("SetLight ����setValue:%0.2f", val);
	_glogger->info(strLog);

	_current_val += val;
}

void Clight_auto_calDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.
void Clight_auto_calDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR Clight_auto_calDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Clight_auto_calDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialogEx::OnOK();
}


void Clight_auto_calDlg::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialogEx::OnCancel();
}


void Clight_auto_calDlg::OnBnClickedButton1()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetEvent(EV_auto_light_ctrl);
}


void Clight_auto_calDlg::OnBnClickedButton2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetLight(_current_val + 10);
}


void Clight_auto_calDlg::OnBnClickedButton3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SetLight(_current_val - 10);
}
