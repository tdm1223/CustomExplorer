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
    struct SORTPARAM
    {
        int sortColumn;
        bool sortDirect;
        CListCtrl *list;
    };
    const int kDirectoryIcon = 4;

    CRemoteExplorerClientDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

    afx_msg void OnBnClickedConnectButton();
    afx_msg void InitComboBox();
    void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
    void UpdateTreeCtrl(const Data & file);
    void AddVirtualFolder(const CString filePath);
    void DrawListCtrl(const Data & data);
    void InitTreeCtrl(const Data & data);
    void GetSystemImage();
    afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
    void UpdateListCtrl(const Data & file);
    void FocusTreeCtrl(CString fileName);
    void CollpaseAllNode(HTREEITEM hItem);
    void ShowData(const Data & data);
    afx_msg void OnHdnItemClickList(NMHDR *pNMHDR, LRESULT *pResult);
    static int CALLBACK CompareItem(const LPARAM lParam1, const LPARAM lParam2, const LPARAM lParamSort);
    static int FileSizeConvertToInt(CString& strItem);
    void OnComboBoxChanged();

    std::map<CString, Data> clientCache; // 파일 경로와 그때의 Data를 가지고 있는 cache
    HTREEITEM FindItem(const CString & fileName, HTREEITEM hRoot);
    CComboBox comboBox;
    BOOL sorting;
    CImageList imgSmallList;
    HIMAGELIST systemImageList;
    SHFILEINFO info;
    CConnectSocket connectSocket;
    CString message;
    CListBox list;
    CIPAddressCtrl ipAddress;
    CEdit portNum;
    CButton connectButton;
    BOOL isConnect;
    CTreeCtrl treeCtrl;
    CListCtrl listCtrl;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
    HICON m_hIcon;
    // 생성된 메시지 맵 함수
    virtual BOOL OnInitDialog();
    void InitListCtrl();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_REMOTEEXPLORERCLIENT_DIALOG };
#endif

};
