#ifndef REMOTEEXPLORERCLIENT_REMOTEEXPLORERCLIENTDLG_H
#define REMOTEEXPLORERCLIENT_REMOTEEXPLORERCLIENTDLG_H
#endif // !REMOTEEXPLORERCLIENT_REMOTEEXPLORERCLIENTDLG_H

#include "ConnectSocket.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <map>

struct SORTPARAM
{
    int sortColumn;
    BOOL sortDirect;
    CListCtrl *list;
};

class CRemoteExplorerClientDlg : public CDialogEx
{
public:
    const int kDirectoryIcon = 4;
    CRemoteExplorerClientDlg(CWnd* pParent = NULL);
    afx_msg void OnBnClickedConnectButton();
    afx_msg void InitComboBox();
    void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
    void UpdateTreeCtrl(const Packet & file);
    void AddVirtualFolder(const CString filePath);
    void DrawListCtrl(const Data & data);
    void InitTreeCtrl(const Packet & data);
    void GetSystemImage();
    afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
    void UpdateListCtrl(const Packet & file);
    void FocusTreeCtrl(CString fileName);
    void CollapseAllNode(HTREEITEM hItem);
    void ShowData(const Data & data);
    afx_msg void OnHdnItemClickList(NMHDR *pNMHDR, LRESULT *pResult);
    static int CALLBACK CompareItem(const LPARAM lParam1, const LPARAM lParam2, const LPARAM lParamSort);
    static int FileSizeConvertToInt(CString& strItem);
    void OnComboBoxChanged();

    std::map<CString, Data> clientCache; // 파일 경로와 그때의 Data를 가지고 있는 cache
    HTREEITEM FindItem(const CString fileName, HTREEITEM rootItem);
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
    SORTPARAM initSort;

protected:
    void InitSort();
    virtual void DoDataExchange(CDataExchange* pDX);
    HICON m_hIcon;
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
