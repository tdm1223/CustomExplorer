#pragma once

//#include "Data.h"
#include "../Data/Data.h"

// CListenSocket 명령 대상입니다.
class CListenSocket : public CAsyncSocket
{
public:
    CListenSocket();
    virtual ~CListenSocket();
    virtual void OnAccept(int nErrorCode);
    void CloseClientSocket(CSocket * pClient);
    void InitData(CSocket * clientSocket, Data & file);
    void RefreshTreeCtrl(CSocket * clientSocket, Data & data);
    void MakeData(CSocket * clientSocket, Data & receiveData, Protocol protocol, CString filePath);
    void RefreshListCtrl(CSocket * clientSocket, Data & data);
    void ResponseData(CSocket * clientSocket, Data & receiveData);
    CPtrList clientSocketList; // 연결된 소켓 클래스 객체를 링크드 리스트로 관리

};