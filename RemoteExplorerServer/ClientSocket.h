#pragma once

struct ServerLog {
    CString ipAddress;
    CString port;
    CString message;
    CString filePath;
};

class CClientSocket : public CSocket
{
public:
    CClientSocket();
    virtual ~CClientSocket();

    virtual void OnClose(int nErrorCode);
    virtual void OnReceive(int nErrorCode);
    void SetServerLog(ServerLog serverLog);
    void SetListenSocket(CAsyncSocket * socket);
    CAsyncSocket* listenSocket;
};