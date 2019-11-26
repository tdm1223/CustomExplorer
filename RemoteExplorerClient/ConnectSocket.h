#ifndef REMOTEEXPLORERCLIENT_CONNECTSOCKET_H
#define REMOTEEXPLORERCLIENT_CONNECTSOCKET_H
#endif // !REMOTEEXPLORERCLIENT_CONNECTSOCKET_H

//#include "Data.h"
#include "../Data/Data.h"
class CConnectSocket : public CSocket
{
public:
    CConnectSocket();
    virtual ~CConnectSocket();
    virtual void OnClose(int nErrorCode);
    virtual void OnReceive(int nErrorCode);
};
