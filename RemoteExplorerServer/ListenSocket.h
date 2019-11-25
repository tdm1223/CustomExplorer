#pragma once

//#include "Data.h"
#include "../Data/Data.h"

// CListenSocket 명령 대상입니다.
class CListenSocket : public CAsyncSocket
{
public:
    const int kDriveLength = 7;

    CListenSocket();
    virtual ~CListenSocket();

    virtual void OnAccept(int nErrorCode);
    void CloseClientSocket(CSocket * pClient);
    void InitData(CSocket * clientSocket, Data & file);
    void RefreshTreeCtrl(CSocket * clientSocket, const Data & data);
    void ResponseData(CSocket * clientSocket, const Data & receiveData);
    void RefreshListCtrl(CSocket * clientSocket, const Data & data);
    void MakeData(CSocket * clientSocket, const Data & receiveData, const Protocol protocol, const CString filePath);

    CPtrList clientSocketList; // 연결된 소켓 클래스 객체를 링크드 리스트로 관리
};