#pragma once
#include "afx.h"

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

    FileType fileType;
    FileType childType[500];
    char fileName[MAX_PATH];
    char filePath[1024];
    int fileSize;
    int childLength;
    char childName[500][MAX_PATH];
    char childAccessTime[500][100];
    ULONGLONG childSize[500];
};

