#pragma once
#include "afx.h"

enum Protocol
{
    kConnect = 100,
    kRefreshTreeCtrl = 200,
    kRefreshListCtrl = 300,
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
    Protocol protocol;
    char fileName[MAX_PATH];
    char filePath[1024];
    int fileSize;
    FileType fileType;
    int childLength;
    char child[500][MAX_PATH];
    char childAccessTime[500][100];
    int childSize[500];
    FileType childType[500];

    Data();
    ~Data();
    void Serialize(Data & file, char * buffer);
    void DeSerialize(Data & file, char * buffer);

};

