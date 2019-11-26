#include "stdafx.h"
#include "RemoteExplorerServer.h"
#include "ClientSocket.h"
#include "ListenSocket.h"
#include "RemoteExplorerServerDlg.h"

CClientSocket::CClientSocket()
{
    listenSocket = NULL;
}

CClientSocket::~CClientSocket()
{
}

void CClientSocket::OnClose(int nErrorCode)
{
    CSocket::OnClose(nErrorCode);
    CListenSocket* cListenSocket = static_cast<CListenSocket*>(listenSocket);
    cListenSocket->CloseClientSocket(this);
}

void CClientSocket::OnReceive(int nErrorCode)
{
    CString listBoxString = _T("");
    CString ipAddress = _T("");
    UINT portNumber = 0;
    char receiveBuffer[sizeof(Data)];
    ::ZeroMemory(receiveBuffer, sizeof(receiveBuffer));

    GetPeerName(ipAddress, portNumber);
    if (Receive(receiveBuffer, sizeof(receiveBuffer)) > 0)
    {
        CRemoteExplorerServerDlg* serverDialog = static_cast<CRemoteExplorerServerDlg*>(AfxGetMainWnd());
        CListenSocket* serverSocket = static_cast<CListenSocket*>(listenSocket);
        Data receiveData;
        receiveData.DeSerialize(receiveData, receiveBuffer);
        CString filePath;
        filePath = static_cast<CString>(receiveData.filePath);
        CString port;
        port.Format(_T("%d"), portNumber);
        switch (receiveData.protocol)
        {
        case kConnect:
            serverDialog->list.AddString(ipAddress + _T("::") + port + (" CONNECT"));
            serverSocket->InitData(this, receiveData);
            break;
        case kUpdateTreeCtrl:
            serverDialog->list.AddString(ipAddress + _T("::") + port + _T(" CLICK TREE ") + filePath);
            serverSocket->UpdateTreeCtrl(this, receiveData);
            break;
        case kUpdateListCtrl:
            serverDialog->list.AddString(ipAddress + _T("::") + port + _T(" DOUBLE CLICK LISTCTRL ") + filePath);
            serverSocket->UpdateListCtrl(this, receiveData);
            break;
        case kRequestData:
            serverDialog->list.AddString(ipAddress + _T("::") + port + _T(" MAKE DATA ") + filePath + _T("\\*.*"));
            serverSocket->ResponseData(this, receiveData);
            break;
        }
    }
    CSocket::OnReceive(nErrorCode);
}

void CClientSocket::SetListenSocket(CAsyncSocket* socket)
{
    listenSocket = socket;
}