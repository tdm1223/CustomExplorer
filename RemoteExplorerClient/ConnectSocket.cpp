#include "ConnectSocket.h"
#include "stdafx.h"
#include "RemoteExplorerClient.h"
#include "RemoteExplorerClientDlg.h"

// CConnectSocket

CConnectSocket::CConnectSocket()
{
}

CConnectSocket::~CConnectSocket()
{
}


// CConnectSocket 멤버 함수
void CConnectSocket::OnClose(int nErrorCode)
{
    ShutDown();
    Close();

    CSocket::OnClose(nErrorCode);

    AfxMessageBox(_T("서버와 연결 끊김"));
    //::PostQuitMessage(0);
}


void CConnectSocket::OnReceive(int nErrorCode)
{
    char buffer[sizeof(Data)];
    ::ZeroMemory(buffer, sizeof(buffer));
    if (Receive(buffer, sizeof(buffer), 0) > 0)
    {
        Data data;
        data.DeSerialize(data, buffer);
        CRemoteExplorerClientDlg* clientDialog = static_cast<CRemoteExplorerClientDlg*>(AfxGetMainWnd());

        switch (data.protocol)
        {
        case kConnect:
            clientDialog->InitTreeCtrl(data);
            break;
        case kRefreshTreeCtrl:
            clientDialog->RefreshTreeCtrl(data);
            break;
        case kRefreshListCtrl:
            clientDialog->RefreshListCtrl(data);
            break;
        case kRequestData:
            clientDialog->MakeData(data);
            break;
        default: break;
        }
    }
    CSocket::OnReceive(nErrorCode);
}