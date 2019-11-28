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
        CListenSocket* serverSocket = static_cast<CListenSocket*>(listenSocket);
        Data receiveData;
        receiveData.DeSerialize(receiveData, receiveBuffer);
        ServerLog serverLog;
        serverLog.filePath = static_cast<CString>(receiveData.filePath);
        serverLog.port.Format(_T("%d"), portNumber);
        serverLog.ipAddress = ipAddress;
        switch (receiveData.protocol)
        {
        case kConnect:
            serverLog.message=_T("CONNECT");
            SetServerLog(serverLog);
            serverSocket->InitData(this, receiveData);
            break;
        case kUpdateTreeCtrl:
            serverLog.message = _T("UPDATE TREECTRL");
            SetServerLog(serverLog);
            serverSocket->UpdateTreeCtrl(this, receiveData);
            break;
        case kUpdateListCtrl:
            serverLog.message = _T("UPDATE LISTCTRL");
            SetServerLog(serverLog);
            serverSocket->UpdateListCtrl(this, receiveData);
            break;
        case kRequestData:
            serverLog.message = _T("REQUEST DATA");
            SetServerLog(serverLog);
            serverSocket->ResponseData(this, receiveData);
            break;
        }
    }
    CSocket::OnReceive(nErrorCode);
}

void CClientSocket::SetServerLog(ServerLog serverLog)
{
    CRemoteExplorerServerDlg* serverDialog = static_cast<CRemoteExplorerServerDlg*>(AfxGetMainWnd());
    serverDialog->listCtrl.InsertItem(0, serverLog.ipAddress);
    serverDialog->listCtrl.SetItemText(0, 1, serverLog.port);
    serverDialog->listCtrl.SetItemText(0, 2, serverLog.message);
    serverDialog->listCtrl.SetItemText(0, 3, serverLog.filePath);
}
void CClientSocket::SetListenSocket(CAsyncSocket* socket)
{
    listenSocket = socket;
}