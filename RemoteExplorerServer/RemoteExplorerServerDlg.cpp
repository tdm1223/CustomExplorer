// RemoteExplorerServerDlg.cpp : 구현 파일

#include "stdafx.h"
#include "afxdialogex.h"
#include "RemoteExplorerServer.h"
#include "RemoteExplorerServerDlg.h"
#include "ClientSocket.h"
#include "ListenSocket.h"
//#include "Data.h"
#include "../Data/Data.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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

CRemoteExplorerServerDlg::CRemoteExplorerServerDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_REMOTEEXPLORERSERVER_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteExplorerServerDlg::OnDestroy()
{
    CDialog::OnDestroy();

    POSITION position;
    position = listenSocket.clientSocketList.GetHeadPosition();
    CClientSocket* client = NULL;

    while (position != NULL)
    {
        client = (CClientSocket*)listenSocket.clientSocketList.GetNext(position);
        if (client != NULL)
        {
            client->ShutDown();
            client->Close();
            delete client;
        }
    }

    listenSocket.ShutDown();
    listenSocket.Close();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

void CRemoteExplorerServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, list);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CRemoteExplorerServerDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()

// CRemoteExplorerServerDlg 메시지 처리기
BOOL CRemoteExplorerServerDlg::OnInitDialog()
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

    if (listenSocket.Create(21000, SOCK_STREAM))
    {
        if (!listenSocket.Listen())
        {
            MessageBox(_T("LISTEN 실패"), _T("실패"), 0);
        }
    }
    else
    {
        MessageBox(_T("서버 소켓 생성에 실패하였습니다."), _T("알림"), 0);
    }

    return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRemoteExplorerServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRemoteExplorerServerDlg::OnPaint()
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

HCURSOR CRemoteExplorerServerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}