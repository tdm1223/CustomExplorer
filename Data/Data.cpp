#include "Data.h"

Data::Data()
{
    ::ZeroMemory(childType, sizeof(childType));
    ::ZeroMemory(fileName, sizeof(fileName));
    ::ZeroMemory(filePath, sizeof(filePath));
    ::ZeroMemory(childName, sizeof(childName));
    ::ZeroMemory(childAccessTime, sizeof(childAccessTime));
    ::ZeroMemory(childSize, sizeof(childSize));
    childLength = 0;
    fileSize = 0;
}


Data::~Data()
{
}
