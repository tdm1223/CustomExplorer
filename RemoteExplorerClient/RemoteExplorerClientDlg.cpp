
// RemoteExplorerClientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "RemoteExplorerClient.h"
#include "RemoteExplorerClientDlg.h"
#include "afxdialogex.h"
#include <vector>

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

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CRemoteExplorerClientDlg 대화 상자
CRemoteExplorerClientDlg::CRemoteExplorerClientDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_REMOTEEXPLORERCLIENT_DIALOG, pParent), message(_T(""))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    isConnect = false;
}

void CRemoteExplorerClientDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_IPADDRESS, ipAddress);
    DDX_Control(pDX, IDC_EDIT, portNum);
    DDX_Control(pDX, IDC_CONNECT_BUTTON, connectButton);
    DDX_Control(pDX, IDC_TREE, treeCtrl);
    DDX_Control(pDX, IDC_LIST, listCtrl);
    DDX_Control(pDX, IDC_COMBO1, comboBox);
}

BEGIN_MESSAGE_MAP(CRemoteExplorerClientDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_CONNECT_BUTTON, &CRemoteExplorerClientDlg::OnBnClickedConnectButton)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CRemoteExplorerClientDlg::OnTvnSelchangedTree)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CRemoteExplorerClientDlg::OnNMDblclkList)
END_MESSAGE_MAP()


// CRemoteExplorerClientDlg 메시지 처리기

BOOL CRemoteExplorerClientDlg::OnInitDialog()
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
    // 프레임워크가 이 작업을 자동으로 수행합니다.
    SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
    SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

    // TODO: 여기에 추가 초기화 작업을 추가합니다.
    connectSocket.Create();
    ipAddress.SetWindowText(_T("127.0.0.1"));
    portNum.SetWindowText(_T("21000"));

    InitComboBox();
    InitListCtrl();

    GetSystemImage();

    return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRemoteExplorerClientDlg::GetSystemImage()
{
    // 운영체제가 관리하는 작은 아이콘 이미지 리스트와 연결
    systemImageList = (HIMAGELIST)SHGetFileInfo((LPCTSTR)_T("C:\\"),
        0, &info, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

    imgSmallList.Attach(systemImageList);
    imgSmallList.Add(AfxGetApp()->LoadIcon(IDR_MAINFRAME));

    treeCtrl.SetImageList(&imgSmallList, LVSIL_NORMAL);
    imgSmallList.Detach();
}
void CRemoteExplorerClientDlg::InitListCtrl()
{
    CRect rect;
    listCtrl.GetWindowRect(&rect);
    listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    listCtrl.InsertColumn(0, _T("이름"), LVCFMT_LEFT, (int)(rect.Width()*0.25));
    listCtrl.InsertColumn(1, _T("파일경로"), LVCFMT_LEFT, 0); // 파일경로는 숨김
    listCtrl.InsertColumn(2, _T("유형"), LVCFMT_LEFT, (int)(rect.Width()*0.15));
    listCtrl.InsertColumn(3, _T("크기"), LVCFMT_LEFT, (int)(rect.Width()*0.25));
    listCtrl.InsertColumn(4, _T("수정한 날짜"), LVCFMT_LEFT, (int)(rect.Width()*0.35));
}

void CRemoteExplorerClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
void CRemoteExplorerClientDlg::OnPaint()
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
HCURSOR CRemoteExplorerClientDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CRemoteExplorerClientDlg::OnBnClickedConnectButton()
{
    CString ipAddr;
    ipAddress.GetWindowText(ipAddr);
    CString port;
    GetDlgItemText(IDC_EDIT, port);


    if (connectSocket.Connect(ipAddr, _ttoi(port)) == FALSE)
    {
        MessageBox(_T("서버 연결 실패"), _T("실패"), 0);
    }
    else
    {
        Data connectData;
        connectData.protocol = kConnect;
        char buffer[sizeof(connectData)];
        connectData.Serialize(connectData, buffer);
        connectSocket.Send(buffer, sizeof(buffer), 0);

        MessageBox(_T("서버 연결 성공"), _T("성공"), 0);
    }
}

void CRemoteExplorerClientDlg::InitComboBox()
{
    comboBox.AddString(_T("큰 아이콘"));
    comboBox.AddString(_T("작은 아이콘"));
    comboBox.AddString(_T("간단히"));
    comboBox.AddString(_T("자세히"));
}

void CRemoteExplorerClientDlg::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    // 폴더 클릭시 하위 폴더 목록을 보여준다.
    HTREEITEM selected = pNMTreeView->itemNew.hItem;

    CString filePath; // 최종으로 얻어올 경로
    CString fileName(_T(""));
    // 선택된 아이템을 시작점으로 하여 최상의 부모가 없을때까지 위로 올라간다.
    HTREEITEM parentItem = selected;
    while (parentItem != NULL)
    {
        filePath = _T("\\") + filePath;
        filePath = treeCtrl.GetItemText(parentItem) + filePath;
        if (fileName == "")
        {
            fileName = treeCtrl.GetItemText(parentItem);
        }
        parentItem = treeCtrl.GetParentItem(parentItem);
    }

    // 완성된 경로뒤에 *.*를 붙어 최종 경로를 만들어 저장
    filePath = filePath + _T("*.*");

    if (pNMTreeView->itemNew.hItem != pNMTreeView->itemOld.hItem)
    {
        listCtrl.DeleteAllItems();
    }

    Data selectedData;
    strcpy_s(selectedData.fileName, static_cast<CStringA>(fileName));
    strcpy_s(selectedData.filePath, static_cast<CStringA>(filePath));

    if (cache.find(filePath) == cache.end())
    {
        selectedData.protocol = kRefreshTreeCtrl; // 트리뷰에 그려져있지 않다면 트리뷰 갱신
    }
    else
    {
        selectedData.protocol = kRefreshListCtrl; // 트리뷰에 이미 그려져있을땐 리스트뷰만 갱신
    }

    char buffer[sizeof(selectedData)];
    selectedData.Serialize(selectedData, buffer);
    connectSocket.Send(buffer, sizeof(buffer));
    *pResult = 0;
}

// 처음 연결했을때 받아온 데이터를 treeCtrl에 뿌려주는 함수
void CRemoteExplorerClientDlg::InitTreeCtrl(Data& file)
{
    HTREEITEM hItem;
    SHFILEINFO sfi;
    SHGetFileInfo((CString)file.filePath, 0, &sfi, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES
        | SHGFI_ICON);
    //hItem = treeCtrl.InsertItem(CString(file.fileName), file.fileType, file.fileType);
    hItem = treeCtrl.InsertItem(CString(file.fileName), sfi.iIcon, sfi.iIcon);
    treeCtrl.EnsureVisible(hItem);
}

// treeCtrl을 확장 시켜주는 함수
void CRemoteExplorerClientDlg::ExpandTreeCtrl(HTREEITEM hItem, UINT nCode)
{
    while (hItem != NULL)
    {
        treeCtrl.Expand(hItem, nCode);
        hItem = treeCtrl.GetNextItem(hItem, TVGN_NEXTVISIBLE);
    }
}

// treeCtrl에 가져온 데이터를 추가하는 함수
void CRemoteExplorerClientDlg::RefreshTreeCtrl(Data& file)
{
    HTREEITEM hItem;
    hItem = treeCtrl.GetNextItem(NULL, TVGN_CARET);
    CString filePath;
    filePath = file.filePath;

    CString key;
    key = filePath;
    if (cache.find(key) == cache.end())
    {
        cache[key] = file;
        for (int i = 0; i < file.childLength; i++)
        {
            CString childName;
            childName = file.child[i];
            SHFILEINFO sfi;
            CString childPath;
            childPath = filePath;
            childPath += "\\";
            childPath = childName;

            SHGetFileInfo(childPath, 0, &sfi, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES
                | SHGFI_ICON);
            treeCtrl.InsertItem(childName, sfi.iIcon, sfi.iIcon, hItem);
        }
        GetChildListCtrl(file);
    }
}

void CRemoteExplorerClientDlg::GetChildListCtrl(Data& data)
{
    for (int i = 0; i < data.childLength; i++)
    {
        CString childName;
        CString childPath;
        CString childSize;
        CString childType;
        CString childAccessTime;
        childName = data.child[i];
        childPath = data.filePath;
        if (childPath.GetAt(childPath.GetLength() - 1) == '*')
        {
            childPath = childPath.Left(childPath.GetLength() - 3) + childName;
        }
        else
        {
            childPath += _T("\\");
            childPath += childName;
        }

        switch (data.childType[i])
        {
        case kDirectory:
            childType = _T("Directory");
            childSize = _T("0 KB");
            break;
        case kFile:
            childType = _T("File");
            childSize.Format(_T("%d KB", data.childSize[i]));
            break;
        case kDisk:
            childType = _T("Disk");
            break;
        }

        childAccessTime = data.childAccessTime[i];
        listCtrl.InsertItem(0, childName);
        listCtrl.SetItemText(0, 1, childPath);
        listCtrl.SetItemText(0, 2, childType);
        listCtrl.SetItemText(0, 3, childSize);
        listCtrl.SetItemText(0, 4, childAccessTime);
    }
}

// listCtrl 더블클릭시 실행하는 함수
void CRemoteExplorerClientDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // 필드 선택
    if (pNMItemActivate->iItem != -1)
    {
        Data sendData;
        CString fileName = listCtrl.GetItemText(pNMItemActivate->iItem, 0);
        CString filePath = listCtrl.GetItemText(pNMItemActivate->iItem, 1);
        CString fileType = listCtrl.GetItemText(pNMItemActivate->iItem, 2);
        CString fileSize = listCtrl.GetItemText(pNMItemActivate->iItem, 3);
        CString key;
        key = filePath + _T("\\*.*");
        // 캐시에 없을 경우 데이터 요청
        if (cache.find(key) == cache.end())
        {
            sendData.protocol = kRequestData;
            strcpy_s(sendData.filePath, static_cast<CStringA>(filePath));
            strcpy_s(sendData.fileName, static_cast<CStringA>(fileName));

            char buffer[sizeof(Data)];
            sendData.Serialize(sendData, buffer);
            connectSocket.Send(buffer, sizeof(buffer));
        }
        // 캐시에 있다면 있는거로 화면 요청
        else
        {
            filePath += _T("\\*.*");
            strcpy_s(sendData.filePath, static_cast<CStringA>(filePath));
            sendData.protocol = kRefreshListCtrl;

            listCtrl.DeleteAllItems();

            char buffer[sizeof(Data)];
            sendData.Serialize(sendData, buffer);
            connectSocket.Send(buffer, sizeof(buffer));
        }

    }
    *pResult = 0;
}

void CRemoteExplorerClientDlg::RefreshListCtrl(Data& receiveData)
{
    Data sendData;
    CString filePath;
    filePath = receiveData.filePath;
    if (cache.find(filePath) == cache.end())
    {
        sendData.protocol = kRequestData;
        char buffer[sizeof(Data)];
        sendData.Serialize(sendData, buffer);
        connectSocket.Send(buffer, sizeof(buffer));
    }
    else
    {
        sendData = cache[filePath];
        GetChildListCtrl(sendData);
        CString fileName;
        fileName = sendData.fileName;
        int checkIndex = CheckFilePath(filePath);
        if (sendData.fileType != kDisk)
        {
            listCtrl.InsertItem(0, _T(".."));
            listCtrl.SetItemText(0, 1, filePath.Left(checkIndex));
        }
        // 트리뷰에도 반영
        //HTREEITEM current = FindItem(fileName, treeCtrl.GetRootItem());
        //while (current != treeCtrl.GetRootItem())
        //{
        //    treeCtrl.Expand(current, TVE_EXPAND);
        //    current = treeCtrl.GetNextItem(current, TVGN_PARENT);
        //}
    }
}

// treeCtrl에서 string에 해당하는 파일을 찾으며 펼쳐주는 함수
HTREEITEM CRemoteExplorerClientDlg::ExpandItem(const CString& fileName, HTREEITEM rootItem)
{
    CString text = treeCtrl.GetItemText(rootItem);
    if (text.Compare(fileName) == 0)
    {
        return rootItem;
    }

    HTREEITEM child = treeCtrl.GetChildItem(rootItem);
    while (child)
    {
        HTREEITEM hFound = ExpandItem(fileName, child);
        if (hFound)
        {
            treeCtrl.Expand(hFound, TVE_EXPAND);
            return hFound;
        }

        child = treeCtrl.GetNextSiblingItem(child);
    }

    return NULL;
}

// 파일경로 체크하는 함수 위로 두번 올라감
// 두번 올라가지 못하는 파일경로라면 -1반환
// 두번 올라갈 수 있는 파일경로라면 두번 올라간 파일경로의 마지막 index 반환
int CRemoteExplorerClientDlg::CheckFilePath(CString filePath)
{
    int idx = -1;
    bool flag = true;
    for (int i = filePath.GetLength() - 1; i >= 0; i--)
    {
        if (filePath.GetAt(i) == '\\')
        {
            if (flag)
            {
                flag = false;
            }
            else
            {
                idx = i;
                break;
            }
        }
    }
    return idx;
}

void CRemoteExplorerClientDlg::MakeData(Data& data)
{
    CString filePath;
    filePath = data.filePath;
    filePath += _T("\\*.*");
    cache[filePath] = data;
    listCtrl.DeleteAllItems();
    UpdateTreeCtrl(data);
    UpdateListCtrl(data);
}

void CRemoteExplorerClientDlg::UpdateListCtrl(Data& data)
{
    // 받아온 데이터의 아이템들을 표시해준다.
    GetChildListCtrl(data);

    CString filePath;
    filePath = data.filePath;


    listCtrl.InsertItem(0, _T(".."));

    int idx = -1;
    // 한칸 올라간 filePath를 추가해준다.
    for (int i = filePath.GetLength() - 1; i >= 0; i--)
    {
        if (filePath.GetAt(i) == '\\')
        {
            idx = i;
            break;
        }
    }
    listCtrl.SetItemText(0, 1, filePath.Left(idx));
}

void CRemoteExplorerClientDlg::UpdateTreeCtrl(Data& data)
{
    CString fileName;
    fileName = data.fileName;
    // 트리뷰에서 현재 파일의 경로를 찾는다.
    HTREEITEM current = FindItem(fileName, treeCtrl.GetRootItem());

    // 부모에 자식으로 가져온 데이터들을 추가해준다.
    for (int i = 0; i < data.childLength; i++)
    {
        CString childName;
        childName = data.child[i];

        SHFILEINFO sfi;
        CString childPath;
        childPath = data.filePath;
        childPath += "\\";
        childPath = childName;
        SHGetFileInfo(childPath, 0, &sfi, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES
            | SHGFI_ICON);
        treeCtrl.InsertItem(childName, sfi.iIcon, sfi.iIcon, current);
    }
}

// treeCtrl에서 string에 해당하는 파일을 찾는 함수
HTREEITEM CRemoteExplorerClientDlg::FindItem(const CString& fileName, HTREEITEM rootItem)
{
    CString text = treeCtrl.GetItemText(rootItem);
    if (text.Compare(fileName) == 0)
    {
        return rootItem;
    }

    HTREEITEM sibling = treeCtrl.GetChildItem(rootItem);
    while (sibling)
    {
        HTREEITEM hFound = FindItem(fileName, sibling);
        if (hFound)
        {
            return hFound;
        }

        sibling = treeCtrl.GetNextSiblingItem(sibling);
    }

    return NULL;
}