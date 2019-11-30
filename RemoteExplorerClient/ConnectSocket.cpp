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
    char buffer[sizeof(Packet)];
    ::ZeroMemory(buffer, sizeof(buffer));
    if (Receive(buffer, sizeof(buffer), 0) > 0)
    {
        CRemoteExplorerClientDlg* clientDialog = static_cast<CRemoteExplorerClientDlg*>(AfxGetMainWnd());
        Packet receivePacket;
        receivePacket.DeSerialize(buffer);
        switch (receivePacket.messageType)
        {
        case kConnect:
            clientDialog->InitTreeCtrl(receivePacket);
            break;
        case kUpdateTreeCtrl:
            clientDialog->UpdateTreeCtrl(receivePacket);
            break;
        case kUpdateListCtrl:
            clientDialog->UpdateListCtrl(receivePacket);
            break;
        case kRequestData:
            clientDialog->ShowData(receivePacket.data);
            break;
        default: break;
        }
    }
    CSocket::OnReceive(nErrorCode);
}