
// light_auto_calDlg.cpp : 구현 파일
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
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// Clight_auto_calDlg 대화 상자

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


// Clight_auto_calDlg 메시지 처리기

BOOL Clight_auto_calDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	_glogger = new gLogger("suslog", std::string("D:\\Log\\") + std::string("syslog.txt"), false, 23, 59);

	AfxBeginThread(ThreadAutoLightControl, this, THREAD_PRIORITY_HIGHEST, 0);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void Clight_auto_calDlg::CheckLightVal()
{
	int current_val = _current_val;

	int corr_val = GetDiffValFromTarget(current_val, _target_value);

	CString strLog;
	strLog.Format("CheckLightVal 기준과 밝기차이 : %d", corr_val);
	_glogger->info(strLog);

	if (abs(corr_val) > _good_range){
		//조정필요
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
	strLog.Format("DoProcess 현재밝기:%d, 기준과 밝기차이 값:%d", _current_val, corr_vals);
	_glogger->info(strLog);

	int changed_val = abs(_before_val - _current_val);

	//현재밝기값을 일단 저장
	_before_val = _current_val;

	if (_bFirstStepDone == false) {
		//first step
		//디폴트 degree로 변화 시켜본다
		

		//조명밝기 조정-방향체크
		double change_degree = 0;
		if (corr_vals > 0) {
			change_degree = +(_default_degree);
		}
		else {
			change_degree = -(_default_degree);
		}
		_changed_degree = change_degree;

		strLog.Format("DoProcess FirstStep 조정값:%0.2f", _changed_degree);
		_glogger->info(strLog);

		//조명 조정값 set
		SetLight(_changed_degree);

		_bFirstStepDone = true;
	}
	else {
		//second step
		//디폴트 degree로 변화시켰을 때 변화량을 보고 ok range로 가게 될 값을 예측
		
		//degree 1당 변하는 밝기
		double std_val = abs(_changed_degree )/ abs(changed_val);

		strLog.Format("DoProcess SecondStep degree 1당 변하는 밝기값:%0.2f", std_val);
		_glogger->info(strLog);

		double final_val = corr_vals / std_val;

		_changed_degree = final_val;

		SetLight(final_val);
	}
}

//+면 값을 올려야됨, -면 값을 내려야됨.
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
	strLog.Format("SetLight 조명setValue:%0.2f", val);
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.
void Clight_auto_calDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR Clight_auto_calDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Clight_auto_calDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}


void Clight_auto_calDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}


void Clight_auto_calDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetEvent(EV_auto_light_ctrl);
}


void Clight_auto_calDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetLight(_current_val + 10);
}


void Clight_auto_calDlg::OnBnClickedButton3()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetLight(_current_val - 10);
}
