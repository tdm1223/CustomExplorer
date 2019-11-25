#pragma once

// CClientSocket 명령 대상입니다.

class CClientSocket : public CSocket
{
public:
    CClientSocket();
    virtual ~CClientSocket();

    virtual void OnClose(int nErrorCode);
    virtual void OnReceive(int nErrorCode);
    void SetListenSocket(CAsyncSocket * pSocket);

    CAsyncSocket* listenSocket;
};