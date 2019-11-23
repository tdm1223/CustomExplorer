#include "stdafx.h"
#include "RemoteExplorerServer.h"
#include "ClientSocket.h"
#include "ListenSocket.h"
#include "RemoteExplorerServerDlg.h"

// CClientSocket

CClientSocket::CClientSocket()
{
    listenSocket = NULL;
}

CClientSocket::~CClientSocket()
{
}

// CClientSocket 멤버 함수
void CClientSocket::OnClose(int nErrorCode)
{
    CSocket::OnClose(nErrorCode);

    CListenSocket* cListenSocket = static_cast<CListenSocket*>(listenSocket);
    cListenSocket->CloseClientSocket(this);
}

void CClientSocket::SetListenSocket(CAsyncSocket* socket)
{
    listenSocket = socket;
}

void CClientSocket::OnReceive(int nErrorCode)
{
    CString listBoxString = _T("");
    CString ipAddress = _T("");
    UINT portNumber = 0;
    char buffer[sizeof(Data)];
    ::ZeroMemory(buffer, sizeof(buffer));

    GetPeerName(ipAddress, portNumber);
    if (Receive(buffer, sizeof(buffer)) > 0)
    {
        CRemoteExplorerServerDlg* serverDialog = static_cast<CRemoteExplorerServerDlg*>(AfxGetMainWnd());
        CListenSocket* serverSocket = static_cast<CListenSocket*>(listenSocket);

        Data data;
        data.DeSerialize(data, buffer);
        CString filePath;
        switch (data.protocol)
        {
        case kConnect:
            serverDialog->list.AddString(_T("CONNECT"));
            serverSocket->InitData(this, data);
            break;
        case kRefreshTreeCtrl:
            filePath = data.filePath;
            serverDialog->list.AddString(_T("CLICK TREE ") + filePath);
            serverSocket->RefreshTreeCtrl(this, data);
            break;
        case kRefreshListCtrl:
            filePath = data.filePath;
            serverDialog->list.AddString(_T("DOUBLE CLICK LISTCTRL ") + filePath);
            serverSocket->RefreshListCtrl(this, data);
            break;
        case kRequestData:
            filePath = data.filePath;
            serverDialog->list.AddString(_T("MAKE DATA ") + filePath+_T("\\*.*"));
            serverSocket->ResponseData(this, data);
            break;
        }

    }

    CSocket::OnReceive(nErrorCode);
}