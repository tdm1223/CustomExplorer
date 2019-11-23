#pragma once

// CClientSocket 명령 대상입니다.

class CClientSocket : public CSocket
{
public:
    CClientSocket();
    virtual ~CClientSocket();

    CAsyncSocket* listenSocket;
    virtual void OnClose(int nErrorCode);
    void SetListenSocket(CAsyncSocket * pSocket);
    virtual void OnReceive(int nErrorCode);
};