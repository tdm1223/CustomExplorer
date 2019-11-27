#pragma once
#include "afx.h"

enum Protocol
{
    kConnect = 100,
    kUpdateTreeCtrl = 200,
    kUpdateListCtrl = 300,
    kRequestData = 400
};
enum FileType
{
    kDirectory = 0,
    kFile = 1,
    kDisk = 2
};

class Data
{
public:
    Data();
    ~Data();

    void Serialize(Data & file, char * buffer);
    void DeSerialize(Data & file, char * buffer);

    Protocol protocol;
    FileType fileType;
    FileType childType[500];
    char fileName[MAX_PATH];
    char filePath[1024];
    int fileSize;
    int childLength;
    char child[500][MAX_PATH];
    char childAccessTime[500][100];
    ULONGLONG childSize[500];
};

