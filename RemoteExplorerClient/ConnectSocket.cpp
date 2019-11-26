#include "ConnectSocket.h"
#include "stdafx.h"
#include "RemoteExplorerClient.h"
#include "RemoteExplorerClientDlg.h"

CConnectSocket::CConnectSocket()
{
}

CConnectSocket::~CConnectSocket()
{
}

void CConnectSocket::OnClose(int nErrorCode)
{
    ShutDown();
    Close();

    CSocket::OnClose(nErrorCode);

    AfxMessageBox(_T("서버와 연결 끊김"));
}

void CConnectSocket::OnReceive(int nErrorCode)
{
    char buffer[sizeof(Data)];
    ::ZeroMemory(buffer, sizeof(buffer));
    if (Receive(buffer, sizeof(buffer), 0) > 0)
    {
        CRemoteExplorerClientDlg* clientDialog = static_cast<CRemoteExplorerClientDlg*>(AfxGetMainWnd());
        Data receiveData;
        receiveData.DeSerialize(receiveData, buffer);
        switch (receiveData.protocol)
        {
        case kConnect:
            clientDialog->InitTreeCtrl(receiveData);
            break;
        case kUpdateTreeCtrl:
            clientDialog->UpdateTreeCtrl(receiveData);
            break;
        case kUpdateListCtrl:
            clientDialog->UpdateListCtrl(receiveData);
            break;
        case kRequestData:
            clientDialog->ShowData(receiveData);
            break;
        default: break;
        }
    }
    CSocket::OnReceive(nErrorCode);
}