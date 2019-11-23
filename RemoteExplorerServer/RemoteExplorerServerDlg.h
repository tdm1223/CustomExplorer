// RemoteExplorerServerDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "ListenSocket.h"
//#include "Data.h"
#include "../Data/Data.h"

// CRemoteExplorerServerDlg 대화 상자
class CRemoteExplorerServerDlg : public CDialogEx
{
    // 생성입니다.
public:
    CRemoteExplorerServerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
    CListenSocket listenSocket; // 클라이언트 접속을 기다리는 소켓
    Data file;
    CListBox list;
    afx_msg void OnDestroy();


#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_REMOTEEXPLORERSERVER_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
    HICON m_hIcon;
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
};
