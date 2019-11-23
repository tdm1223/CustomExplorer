#include "Data.h"

Data::Data()
{
    ::ZeroMemory(fileName, sizeof(fileName));
    ::ZeroMemory(child, sizeof(child));
    ::ZeroMemory(filePath, sizeof(filePath));
    ::ZeroMemory(childSize, sizeof(childSize));
    ::ZeroMemory(childType, sizeof(childType));
    ::ZeroMemory(childAccessTime, sizeof(childAccessTime));
    childLength = 0;
    fileSize = 0;
}


Data::~Data()
{
}

void Data::Serialize(Data& file, char* buffer)
{
    memcpy(buffer, &file, sizeof(file));
}

void Data::DeSerialize(Data& file, char* buffer)
{
    memcpy(&file, buffer, sizeof(file));
}