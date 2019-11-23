#ifndef REMOTEEXPLORERCLIENT_REMOTEEXPLORERCLIENTDLG_H
#define REMOTEEXPLORERCLIENT_REMOTEEXPLORERCLIENTDLG_H
#endif // !REMOTEEXPLORERCLIENT_REMOTEEXPLORERCLIENTDLG_H

#include "ConnectSocket.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <map>

// CRemoteExplorerClientDlg 대화 상자
class CRemoteExplorerClientDlg : public CDialogEx
{
    // 생성입니다.
public:
    CRemoteExplorerClientDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
    CConnectSocket connectSocket;
    // 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_REMOTEEXPLORERCLIENT_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
    HICON m_hIcon;

    // 생성된 메시지 맵 함수
    virtual BOOL OnInitDialog();
    void InitListCtrl();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    CString message;
    CListBox list;
    afx_msg void OnBnClickedConnectButton();
    CIPAddressCtrl ipAddress;
    CEdit portNum;
    CButton connectButton;
    BOOL isConnect;
    CTreeCtrl treeCtrl;
    CListCtrl listCtrl;
    afx_msg void InitComboBox();
    void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
    void RefreshTreeCtrl(Data & file);
    void GetChildListCtrl(Data & data);
    void AddListCtrl(Data & data);
    void InitTreeCtrl(Data & file);
    void ExpandTreeCtrl(HTREEITEM hItem, UINT nCode);

    std::map<CString, Data> cache; // 파일 경로와 그때의 Data를 가지고 있는 cache
    afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
    void RefreshListCtrl(Data & file);
    HTREEITEM ExpandItem(const CString & fileName, HTREEITEM rootItem);
    int CheckFilePath(CString filePath);
    void MakeData(Data & data);
    void UpdateListCtrl(Data & data);
    void UpdateTreeCtrl(Data & data);
    HTREEITEM FindItem(const CString & fileName, HTREEITEM hRoot);
    CComboBox comboBox;
};
