#pragma once
#include "afxwin.h"
#include "ListenSocket.h"
//#include "Data.h"
#include "../Data/Data.h"

class CRemoteExplorerServerDlg : public CDialogEx
{
public:
    CRemoteExplorerServerDlg(CWnd* pParent = NULL);
    afx_msg void OnDestroy();
    CListenSocket listenSocket; // 클라이언트 접속을 기다리는 소켓
    CListBox list;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
    HICON m_hIcon;

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_REMOTEEXPLORERSERVER_DIALOG };
#endif
};
