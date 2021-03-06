﻿#pragma once

//#include "Data.h"
#include "../Data/Packet.h"

class CListenSocket : public CAsyncSocket
{
public:
    const int kDriveLength = 6;

    CListenSocket();
    virtual ~CListenSocket();

    virtual void OnAccept(int nErrorCode);
    void CloseClientSocket(CSocket * pClient);
    void InitData(CSocket * clientSocket, Packet & file);
    bool IsCorrectDrive(CString & currentDriveName);
    void UpdateTreeCtrl(CSocket * clientSocket, const Packet & data);
    void ResponseData(CSocket * clientSocket, const Packet & receiveData);
    void UpdateListCtrl(CSocket * clientSocket, const Packet & data);
    void MakeAndResponseData(CSocket * clientSocket, const Packet & receiveData, const MessageType messageType, const CString filePath);

    CPtrList clientSocketList; // 연결된 소켓 클래스 객체를 링크드 리스트로 관리
};