#include "stdafx.h"
#include "RemoteExplorerClient.h"
#include "RemoteExplorerClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_ABOUTBOX };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

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

CRemoteExplorerClientDlg::CRemoteExplorerClientDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_REMOTEEXPLORERCLIENT_DIALOG, pParent), message(_T(""))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    isConnect = false;
    clientCache.clear();
}

void CRemoteExplorerClientDlg::OnBnClickedConnectButton()
{
    CString ipAddr;
    ipAddress.GetWindowText(ipAddr);
    CString port;
    GetDlgItemText(IDC_EDIT, port);

    if (connectSocket.Connect(ipAddr, _ttoi(port)) == FALSE)
    {
        AfxMessageBox(_T("서버 연결 실패"));
    }
    else
    {
        Data sendData;
        sendData.protocol = kConnect;
        char sendBuffer[sizeof(sendData)];
        sendData.Serialize(sendData, sendBuffer);
        connectSocket.Send(sendBuffer, sizeof(sendBuffer), 0);
        AfxMessageBox(_T("서버 연결 성공"));
    }
}

void CRemoteExplorerClientDlg::InitComboBox()
{
    //LV_VIEW_ICON            0x0000
    //LV_VIEW_DETAILS         0x0001
    //LV_VIEW_SMALLICON       0x0002
    //LV_VIEW_LIST            0x0003
    comboBox.AddString(_T("큰 아이콘"));
    comboBox.AddString(_T("자세히"));
    comboBox.AddString(_T("작은 아이콘"));
    comboBox.AddString(_T("간단히"));
    comboBox.SetCurSel(LV_VIEW_DETAILS); // 초기상태는 '자세히'
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

    if (clientCache.find(filePath) == clientCache.end())
    {
        selectedData.protocol = kUpdateTreeCtrl; // 트리뷰에 그려져있지 않다면 트리뷰 갱신
    }
    else
    {
        selectedData.protocol = kUpdateListCtrl; // 트리뷰에 이미 그려져있을땐 리스트뷰만 갱신
    }

    char buffer[sizeof(selectedData)];
    selectedData.Serialize(selectedData, buffer);
    connectSocket.Send(buffer, sizeof(buffer));
    *pResult = 0;
}

// treeCtrl을 그려주는 함수
void CRemoteExplorerClientDlg::UpdateTreeCtrl(const Data& receiveData)
{
    CString fileName;
    fileName = static_cast<CString>(receiveData.fileName);
    HTREEITEM hItem = FindItem(fileName, treeCtrl.GetRootItem());
    CString filePath;
    filePath = static_cast<CString>(receiveData.filePath);

    clientCache[filePath] = receiveData;
    for (int childIndex = 0; childIndex < receiveData.childLength; childIndex++)
    {
        CString childName;
        childName = receiveData.childName[childIndex];
        SHFILEINFO shFileInfo;
        SHGetFileInfo(childName, 0, &shFileInfo, sizeof(SHFILEINFO),
            SHGFI_USEFILEATTRIBUTES | SHGFI_ICON);
        if (receiveData.childType[childIndex] == kDirectory)
        {
            treeCtrl.InsertItem(childName, kDirectoryIcon, kDirectoryIcon, hItem);
        }
    }

    DrawListCtrl(receiveData);

    for (int listCtrlItem = 0; listCtrlItem < listCtrl.GetItemCount(); listCtrlItem++)
    {
        listCtrl.SetItemData(listCtrlItem, listCtrlItem);
    }

    listCtrl.SortItems(&CompareItem, (LPARAM)&initSort);

    FocusTreeCtrl(fileName);
}

// 상위 폴더로 이동하는 가상폴더를 추가하는 함수
void CRemoteExplorerClientDlg::AddVirtualFolder(const CString filePath)
{
    listCtrl.InsertItem(0, _T(".."), kDirectoryIcon);
    int index = -1;
    // 한칸 올라간 filePath를 추가해준다.
    for (int filePathIndex = filePath.GetLength() - 1; filePathIndex >= 0; filePathIndex--)
    {
        if (filePath.GetAt(filePathIndex) == '\\')
        {
            index = filePathIndex;
            break;
        }
    }
    listCtrl.SetItemText(0, 1, filePath.Left(index));
}

// listCtrl을 그려주는 함수
void CRemoteExplorerClientDlg::DrawListCtrl(const Data& data)
{
    CString childPath;
    CString childName;
    CString childSize;
    CString childType;
    CString childAccessTime;
    for (int childIndex = 0; childIndex < data.childLength; childIndex++)
    {
        childPath = static_cast<CString>(data.filePath);
        childName = static_cast<CString>(data.childName[childIndex]);
        if (childPath.GetAt(childPath.GetLength() - 1) == '*')
        {
            childPath = childPath.Left(childPath.GetLength() - 4);
        }
        childPath += _T("\\");
        childPath += childName;

        SHFILEINFO shFileInfo;
        SHGetFileInfo(childName, 0, &shFileInfo, sizeof(SHFILEINFO),
            SHGFI_USEFILEATTRIBUTES | SHGFI_ICON);

        switch (data.childType[childIndex])
        {
        case kDirectory:
            childType = _T("Directory");
            childSize = _T("");
            break;
        case kFile:
            childType = _T("File");
            if ((data.childSize[childIndex] >> 10) > 1)
            {
                childSize.Format(_T("%d KB"), data.childSize[childIndex] >> 10);
            }
            else
            {
                childSize.Format(_T("%d B"), data.childSize[childIndex]);
            }
            break;
        case kDisk:
            childType = _T("Disk");
            break;
        }
        childAccessTime = static_cast<CString>(data.childAccessTime[childIndex]);

        if (data.childType[childIndex] == kDirectory)
        {
            listCtrl.InsertItem(0, childName, kDirectoryIcon);
        }
        else
        {
            listCtrl.InsertItem(0, childName, shFileInfo.iIcon);
        }
        listCtrl.SetItemText(0, 1, childPath);
        listCtrl.SetItemText(0, 2, childType);
        listCtrl.SetItemText(0, 3, childSize);
        listCtrl.SetItemText(0, 4, childAccessTime);
    }

    CString filePath;
    filePath = static_cast<CString>(data.filePath);
    if (filePath.GetAt(filePath.GetLength() - 1) == '*')
    {
        filePath = filePath.Left(filePath.GetLength() - 4);
    }
    if (data.fileType == kDirectory)
    {
        AddVirtualFolder(filePath);
    }
}

// 처음 연결했을때 받아온 데이터를 CtreeCtrl에 뿌려주는 함수
void CRemoteExplorerClientDlg::InitTreeCtrl(const Data& receiveData)
{
    SHFILEINFO shFileInfo;
    SHGetFileInfo(static_cast<CString>(receiveData.filePath), 0, &shFileInfo, sizeof(SHFILEINFO)
        , SHGFI_USEFILEATTRIBUTES | SHGFI_ICON);
    treeCtrl.InsertItem(static_cast<CString>(receiveData.fileName), shFileInfo.iIcon, shFileInfo.iIcon);
}

// 시스템 이미지 초기화하는 함수
void CRemoteExplorerClientDlg::GetSystemImage()
{
    systemImageList = (HIMAGELIST)SHGetFileInfo(static_cast<LPCTSTR>(_T("C:\\")),
        0, &info, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
    imgSmallList.Attach(systemImageList);
    imgSmallList.Add(AfxGetApp()->LoadIcon(IDR_MAINFRAME));
    treeCtrl.SetImageList(&imgSmallList, LVSIL_NORMAL);
    listCtrl.SetImageList(&imgSmallList, LVSIL_SMALL);
    imgSmallList.Detach();
}

// listCtrl 더블클릭시 실행하는 함수
void CRemoteExplorerClientDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    if (pNMItemActivate->iItem != -1)
    {
        Data sendData;
        CString fileName = listCtrl.GetItemText(pNMItemActivate->iItem, 0);
        CString filePath = listCtrl.GetItemText(pNMItemActivate->iItem, 1);
        CString clientCacheKey;
        clientCacheKey = filePath + _T("\\*.*");
        // 캐시에 없을 경우 데이터 요청
        if (clientCache.find(clientCacheKey) == clientCache.end())
        {
            sendData.protocol = kRequestData;
            strcpy_s(sendData.filePath, static_cast<CStringA>(filePath));
            strcpy_s(sendData.fileName, static_cast<CStringA>(fileName));
        }
        // 캐시에 있다면 화면 요청
        else
        {
            filePath += _T("\\*.*");
            strcpy_s(sendData.filePath, static_cast<CStringA>(filePath));
            sendData.protocol = kUpdateListCtrl;
            listCtrl.DeleteAllItems();
        }

        char sendBuffer[sizeof(Data)];
        sendData.Serialize(sendData, sendBuffer);
        connectSocket.Send(sendBuffer, sizeof(sendBuffer));
    }
    *pResult = 0;
}

// listCtrl을 업데이트 하는 함수
void CRemoteExplorerClientDlg::UpdateListCtrl(const Data& receiveData)
{
    Data sendData;
    CString filePath;
    filePath = static_cast<CString>(receiveData.filePath);

    sendData = clientCache[filePath];
    DrawListCtrl(sendData);

    for (int listCtrlItem = 0; listCtrlItem < listCtrl.GetItemCount(); listCtrlItem++)
    {
        listCtrl.SetItemData(listCtrlItem, listCtrlItem);
    }

    listCtrl.SortItems(&CompareItem, (LPARAM)&initSort);

    // 트리뷰에도 반영
    CString fileName;
    fileName = static_cast<CString>(sendData.fileName);
    FocusTreeCtrl(fileName);
}

// 매개변수로 들어오는 파일명에 해당하는 파일을 treeCtrl에서 찾아 트리뷰의 맨 위에 나타나게 하는 함수
void CRemoteExplorerClientDlg::FocusTreeCtrl(CString fileName)
{
    HTREEITEM current = FindItem(fileName, treeCtrl.GetRootItem());
    CollapseAllNode(current);
    current = FindItem(fileName, treeCtrl.GetRootItem());
    treeCtrl.Select(current, TVGN_FIRSTVISIBLE);
    while (current != treeCtrl.GetRootItem())
    {
        treeCtrl.Expand(current, TVE_EXPAND);
        current = treeCtrl.GetNextItem(current, TVGN_PARENT);
    }
    treeCtrl.Expand(treeCtrl.GetRootItem(), TVE_EXPAND);
}

// 트리뷰의 모든 노드를 접는 함수
void CRemoteExplorerClientDlg::CollapseAllNode(HTREEITEM hItem)
{
    treeCtrl.Expand(hItem, TVE_COLLAPSE);
    hItem = treeCtrl.GetChildItem(hItem);
    while ((hItem = treeCtrl.GetNextSiblingItem(hItem)) != NULL)
    {
        CollapseAllNode(hItem);
    }
}

void CRemoteExplorerClientDlg::ShowData(const Data& receiveData)
{
    CString filePath;
    filePath = static_cast<CString>(receiveData.filePath);
    filePath += _T("\\*.*");
    clientCache[filePath] = receiveData;

    CString fileName;
    fileName = static_cast<CString>(receiveData.fileName);

    // 트리뷰에서 현재 파일의 경로를 찾는다.
    HTREEITEM current = FindItem(fileName, treeCtrl.GetRootItem());

    // 부모에 자식으로 가져온 데이터들을 추가해준다.
    for (int childIndex = 0; childIndex < receiveData.childLength; childIndex++)
    {
        CString childName;
        childName = static_cast<CString>(receiveData.childName[childIndex]);
        SHFILEINFO shFileInfo;

        CString childPath;
        childPath = static_cast<CString>(receiveData.filePath);
        childPath += "\\";
        childPath = childName;
        SHGetFileInfo(childPath, 0, &shFileInfo, sizeof(SHFILEINFO),
            SHGFI_USEFILEATTRIBUTES | SHGFI_ICON);
        if (receiveData.childType[childIndex] == kDirectory)
        {
            treeCtrl.InsertItem(childName, kDirectoryIcon, kDirectoryIcon, current);
        }
        else
        {
            treeCtrl.InsertItem(childName, shFileInfo.iIcon, shFileInfo.iIcon, current);
        }
    }

    // 받아온 데이터로 ListCtrl을 그려준다.
    listCtrl.DeleteAllItems();
    DrawListCtrl(receiveData);

    for (int listCtrlItem = 0; listCtrlItem < listCtrl.GetItemCount(); listCtrlItem++)
    {
        listCtrl.SetItemData(listCtrlItem, listCtrlItem);
    }

    listCtrl.SortItems(&CompareItem, (LPARAM)&initSort);

    // 트리뷰에 반영
    FocusTreeCtrl(fileName);
}

// '자세히' 모드에서 Header 클릭시 정렬해주는 함수
void CRemoteExplorerClientDlg::OnHdnItemClickList(NMHDR * pNMHDR, LRESULT * pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    int column = pNMLV->iItem;
    for (int listCtrlItem = 0; listCtrlItem < listCtrl.GetItemCount(); listCtrlItem++)
    {
        listCtrl.SetItemData(listCtrlItem, listCtrlItem);
    }

    sorting = !sorting;
    SORTPARAM sortParams;
    sortParams.list = &listCtrl;
    sortParams.sortColumn = column;
    sortParams.sortDirect = sorting;
    listCtrl.SortItems(&CompareItem, (LPARAM)&sortParams);
    *pResult = 0;
}

// 정렬시 사용할 인자
int CRemoteExplorerClientDlg::CompareItem(const LPARAM itemOne, const LPARAM itemTwo, const LPARAM paramSort)
{
    CListCtrl *list = ((SORTPARAM*)paramSort)->list;
    int sortColumn = ((SORTPARAM*)paramSort)->sortColumn;
    BOOL sortDirect = ((SORTPARAM*)paramSort)->sortDirect;

    LVFINDINFO infoOne, infoTwo;
    infoOne.flags = LVFI_PARAM;
    infoOne.lParam = itemOne;
    infoTwo.flags = LVFI_PARAM;
    infoTwo.lParam = itemTwo;
    int rowOne = list->FindItem(&infoOne, -1);
    int rowTwo = list->FindItem(&infoTwo, -1);

    CString itemStringOne = list->GetItemText(rowOne, sortColumn);
    CString itemStringTwo = list->GetItemText(rowTwo, sortColumn);

    if (sortDirect)
    {
        if (sortColumn == 3)
        {
            int paramOneSize = FileSizeConvertToInt(itemStringOne);
            int paramTwoSize = FileSizeConvertToInt(itemStringTwo);
            return paramOneSize < paramTwoSize;
        }
        else
        {
            return strcmp(static_cast<CStringA>(itemStringOne), static_cast<CStringA>(itemStringTwo));
        }
    }
    else
    {
        if (sortColumn == 3)
        {
            int size1 = FileSizeConvertToInt(itemStringOne);
            int size2 = FileSizeConvertToInt(itemStringTwo);
            return size1 > size2;
        }
        else
        {
            return -strcmp(static_cast<CStringA>(itemStringOne), static_cast<CStringA>(itemStringTwo));
        }
    }
}

// 파일 크기를 int형으로 바꿔주는 함수
int CRemoteExplorerClientDlg::FileSizeConvertToInt(CString& itemString)
{
    int size = 0;
    BOOL isKB = false;
    int idx = -1;
    if (itemString.GetLength() > 1)
    {
        for (int itemStringIndex = 0; itemStringIndex < itemString.GetLength(); itemStringIndex++)
        {
            if (itemString.GetAt(itemStringIndex) == ' ')
            {
                idx = itemStringIndex;
                if (itemString.GetAt(itemStringIndex + 1) == 'K')
                {
                    isKB = true;
                }
                break;
            }
        }
        itemString = itemString.Left(idx);
        size = _ttoi(itemString);
        if (isKB == TRUE)
        {
            size *= 1024;
        }
    }
    else
    {
        size = -1;
    }
    return size;
}

void CRemoteExplorerClientDlg::InitSort()
{
    sorting = TRUE;
    initSort;
    initSort.list = &listCtrl;
    initSort.sortColumn = 2;
    initSort.sortDirect = sorting;
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
    ON_NOTIFY(HDN_ITEMCLICK, 0, &CRemoteExplorerClientDlg::OnHdnItemClickList)
    ON_CBN_SELCHANGE(IDC_COMBO1, &CRemoteExplorerClientDlg::OnComboBoxChanged)
END_MESSAGE_MAP()

BOOL CRemoteExplorerClientDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    // IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    //CMenu* pSysMenu = GetSystemMenu(FALSE);
    //if (pSysMenu != NULL)
    //{
    //    BOOL bNameValid;
    //    CString strAboutMenu;
    //    bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
    //    ASSERT(bNameValid);
    //    if (!strAboutMenu.IsEmpty())
    //    {
    //        pSysMenu->AppendMenu(MF_SEPARATOR);
    //        pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    //    }
    //}

    // 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
    // 프레임워크가 이 작업을 자동으로 수행합니다.
    SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
    SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

    connectSocket.Create();

    ipAddress.SetWindowText(_T("127.0.0.1"));
    portNum.SetWindowText(_T("21000"));

    InitComboBox();
    InitListCtrl();
    GetSystemImage();
    InitSort();

    return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// listCtrl을 초기화하는 함수. 초기 상태는 '자세히' 모드
void CRemoteExplorerClientDlg::InitListCtrl()
{
    CRect rect;
    listCtrl.GetWindowRect(&rect);
    listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
    const double kRate[5] = { 0.35,0.0,0.15,0.15,0.35 };
    const CString kColumnName[5] = { _T("이름"),_T("파일경로"),_T("유형"),_T("크기"),_T("수정한 날짜") };
    for (int columnIndex = 0; columnIndex < 5; columnIndex++)
    {
        listCtrl.InsertColumn(columnIndex, kColumnName[columnIndex], LVCFMT_LEFT, static_cast<int>(rect.Width()*kRate[columnIndex]));
    }
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

void CRemoteExplorerClientDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR CRemoteExplorerClientDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// treeCtrl에서 string에 해당하는 파일을 찾는 함수
HTREEITEM CRemoteExplorerClientDlg::FindItem(const CString fileName, HTREEITEM rootItem)
{
    CString text = treeCtrl.GetItemText(rootItem);
    if (text.Compare(fileName) == 0)
    {
        return rootItem;
    }
    HTREEITEM sibling = treeCtrl.GetChildItem(rootItem);
    while (sibling)
    {
        HTREEITEM found = FindItem(fileName, sibling);
        if (found)
        {
            return found;
        }
        sibling = treeCtrl.GetNextSiblingItem(sibling);
    }
    return NULL;
}

// 콤보박스에 있는 모드를 다른것을 선택하면 호출되는 함수
void CRemoteExplorerClientDlg::OnComboBoxChanged()
{
    //LV_VIEW_ICON            0x0000
    //LV_VIEW_DETAILS         0x0001
    //LV_VIEW_SMALLICON       0x0002
    //LV_VIEW_LIST            0x0003
    switch (comboBox.GetCurSel())
    {
    case LV_VIEW_ICON:
        systemImageList = (HIMAGELIST)SHGetFileInfo((LPCTSTR)_T("C:\\"),
            0, &info, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_LARGEICON);
        imgSmallList.Attach(systemImageList);
        listCtrl.SetImageList(&imgSmallList, LVSIL_NORMAL);
        break;
    case LV_VIEW_DETAILS:
    case LV_VIEW_SMALLICON:
    case LV_VIEW_LIST:
    default:
        systemImageList = (HIMAGELIST)SHGetFileInfo((LPCTSTR)_T("C:\\"),
            0, &info, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
        imgSmallList.Attach(systemImageList);
        listCtrl.SetImageList(&imgSmallList, LVSIL_SMALL);
        break;
    }
    imgSmallList.Detach();
    listCtrl.SetView(comboBox.GetCurSel());
}
