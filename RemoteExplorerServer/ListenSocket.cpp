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
void CListenSocket::InitData(CSocket* clientSocket, Packet& receivePacket)
{
    DWORD drives = GetLogicalDrives();
    for (int alphabet = 0; alphabet < 26; alphabet++)
    {
        if ((drives & (1 << alphabet)))
        {
            CString currentDriveName;
            TCHAR driveName[] = { TEXT('A') + alphabet, TEXT(':'), TEXT('\0') };
            currentDriveName = driveName;
            strcpy_s(receivePacket.data.fileName, (CStringA)currentDriveName);
            bool isDrive = IsCorrectDrive(currentDriveName);
            if (!isDrive)
            {
                continue;
            }
            MakeAndResponseData(clientSocket, receivePacket, kConnect, currentDriveName);
        }
    }
}

// 올바른 드라이브인지 체크하는 함수
bool CListenSocket::IsCorrectDrive(CString& currentDriveName)
{
    ULARGE_INTEGER avail, free, total;
    avail.QuadPart = 0L;
    free.QuadPart = 0L;
    total.QuadPart = 0L;
    GetDiskFreeSpaceEx(currentDriveName, &avail, &total, &free);
    if (total.QuadPart == 0 && avail.QuadPart == 0 && free.QuadPart == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

// 클라이언트에서 트리뷰를 클릭했을때 데이터를 만들어서 다시 전달해주는 함수
void CListenSocket::UpdateTreeCtrl(CSocket* clientSocket, const Packet& receiveData)
{
    CString filePath;
    filePath = static_cast<CString>(receiveData.data.filePath);
    MakeAndResponseData(clientSocket, receiveData, kUpdateTreeCtrl, filePath);
}

// 만들어진 데이터를 응답하는 함수
void CListenSocket::ResponseData(CSocket* clientSocket, const Packet& receiveData)
{
    CString filePath;
    filePath = static_cast<CString>(receiveData.data.filePath);
    filePath += _T("\\*.*");
    MakeAndResponseData(clientSocket, receiveData, kRequestData, filePath);
}

// 리스트뷰 클릭시 리스트 재조정 및 트리뷰에 반영
void CListenSocket::UpdateListCtrl(CSocket* clientSocket, const Packet& receiveData)
{
    CClientSocket* client = static_cast<CClientSocket*>(clientSocket);
    Packet sendPacket;
    sendPacket.messageType = kUpdateListCtrl;
    strcpy_s(sendPacket.data.filePath, receiveData.data.filePath);

    char sendBuffer[sizeof(sendPacket)];
    sendPacket.Serialize(sendBuffer);
    client->Send(sendBuffer, sizeof(sendBuffer), 0);
}

// 데이터 만들어서 반환해주는 함수
void CListenSocket::MakeAndResponseData(CSocket* clientSocket, const Packet& receiveData, const MessageType messageType, const CString filePath)
{
    CClientSocket* client = static_cast<CClientSocket*>(clientSocket);
    Packet sendPacket;
    sendPacket.messageType = messageType;
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
        else if (finder.IsDirectory())
        {
            sendPacket.data.childType[count] = kDirectory;
            sendPacket.data.childSize[count] = 0;
        }
        else if (finder.IsArchived())
        {
            sendPacket.data.childType[count] = kFile;
            sendPacket.data.childSize[count] = finder.GetLength();
        }
        CTime cTime;
        finder.GetLastWriteTime(cTime);
        CString timeConvertToString = cTime.Format("%Y-%m-%d %H:%M");
        strcpy_s(sendPacket.data.childAccessTime[count], static_cast<CStringA>(timeConvertToString));

        CString fileName = finder.GetFileName();
        strcpy_s(sendPacket.data.childName[count], static_cast<CStringA>(fileName));
        count++;
    }

    sendPacket.data.childLength = count;
    if (sendPacket.data.childLength == 0)
    {
        sendPacket.data.fileType = kFile;
    }
    else
    {
        if (filePath.GetLength() == kDriveLength)
        {
            sendPacket.data.fileType = kDisk;
        }
        else
        {
            sendPacket.data.fileType = kDirectory;
        }
    }
    strcpy_s(sendPacket.data.filePath, receiveData.data.filePath);
    strcpy_s(sendPacket.data.fileName, receiveData.data.fileName);

    char sendBuffer[sizeof(Packet)];
    sendPacket.Serialize(sendBuffer);
    client->Send(sendBuffer, sizeof(sendBuffer), 0);
}