#include "stdafx.h"
#include "RemoteExplorerServer.h"
#include "ListenSocket.h"
#include "ClientSocket.h"

CListenSocket::CListenSocket()
{
}

CListenSocket::~CListenSocket()
{
}

void CListenSocket::OnAccept(int nErrorCode)
{
    CClientSocket* clientSocket = new CClientSocket;
    if (Accept(*clientSocket))
    {
        clientSocket->SetListenSocket(this);
        clientSocketList.AddTail(clientSocket);
    }
    else
    {
        delete clientSocket;
        AfxMessageBox(_T("새 클라이언트 연결 실패"));
    }
    CAsyncSocket::OnAccept(nErrorCode);
}

void CListenSocket::CloseClientSocket(CSocket* clientSocket)
{
    POSITION position;
    position = clientSocketList.Find(clientSocket);
    if (position != NULL)
    {
        if (clientSocket != NULL)
        {
            clientSocket->ShutDown();
            clientSocket->Close();
        }
        clientSocketList.RemoveAt(position);
        delete clientSocket;
    }
}

// 처음 연결되었을 때 호출되는 함수. 모든 partition 탐색
void CListenSocket::InitData(CSocket* clientSocket, Data& receiveData)
{
    DWORD drives = GetLogicalDrives();
    for (int alphabet = 0; alphabet < 26; alphabet++)
    {
        if ((drives & (1 << alphabet)))
        {
            CString currentDriveName;
            TCHAR driveName[] = { TEXT('A') + alphabet, TEXT(':'), TEXT('\0') };
            currentDriveName = driveName;
            strcpy_s(receiveData.fileName, (CStringA)currentDriveName);
            MakeAndResponseData(clientSocket, receiveData, kConnect, currentDriveName);
        }
    }
}

// 클라이언트에서 트리뷰를 클릭했을때 데이터를 만들어서 다시 전달해주는 함수
void CListenSocket::UpdateTreeCtrl(CSocket* clientSocket, const Data& receiveData)
{
    CString filePath(receiveData.filePath);
    MakeAndResponseData(clientSocket, receiveData, kUpdateTreeCtrl, filePath);
}

// 만들어진 데이터를 응답하는 함수
void CListenSocket::ResponseData(CSocket* clientSocket, const Data& receiveData)
{
    CString filePath(receiveData.filePath);
    filePath += _T("\\*.*");
    MakeAndResponseData(clientSocket, receiveData, kRequestData, filePath);
}

// 리스트뷰 클릭시 리스트 재조정 및 트리뷰에 반영
void CListenSocket::UpdateListCtrl(CSocket* clientSocket, const Data& receiveData)
{
    CClientSocket* client = static_cast<CClientSocket*>(clientSocket);
    Data sendData;
    sendData.protocol = kUpdateListCtrl;
    strcpy_s(sendData.filePath, receiveData.filePath);

    char sendBuffer[sizeof(sendData)];
    sendData.Serialize(sendData, sendBuffer);
    client->Send(sendBuffer, sizeof(sendBuffer), 0);
}

// 데이터 만들어서 반환해주는 함수
void CListenSocket::MakeAndResponseData(CSocket* clientSocket, const Data& receiveData, const Protocol protocol, const CString filePath)
{
    CClientSocket* client = static_cast<CClientSocket*>(clientSocket);
    Data sendData;
    sendData.protocol = protocol;
    CFileFind finder;
    BOOL fileIterator = finder.FindFile(filePath);
    int count = 0;
    while (fileIterator)
    {
        fileIterator = finder.FindNextFile();
        if (finder.IsDots() || finder.IsHidden())
        {
            continue;
        }

        if (finder.IsDirectory())
        {
            sendData.childType[count] = kDirectory;
            sendData.childSize[count] = 0;
        }
        else
        {
            sendData.childType[count] = kFile;
            sendData.childSize[count] = finder.GetLength();
        }
        CTime cTime;
        finder.GetLastAccessTime(cTime);
        CString timeConvertToString = cTime.Format("%Y-%m-%d %H:%M:%S");
        strcpy_s(sendData.childAccessTime[count], static_cast<CStringA>(timeConvertToString));

        CString fileName = finder.GetFileName();
        strcpy_s(sendData.child[count++], static_cast<CStringA>(fileName));
    }

    sendData.childLength = count;
    if (sendData.childLength == 0)
    {
        sendData.fileType = kFile;
    }
    else
    {
        if (filePath.GetLength() == kDriveLength)
        {
            sendData.fileType = kDisk;
        }
        else
        {
            sendData.fileType = kDirectory;
        }
    }
    strcpy_s(sendData.filePath, receiveData.filePath);
    strcpy_s(sendData.fileName, receiveData.fileName);

    char sendBuffer[sizeof(sendData)];
    sendData.Serialize(sendData, sendBuffer);
    client->Send(sendBuffer, sizeof(sendBuffer), 0);
}