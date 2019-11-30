#pragma once
#include "Data.h"

enum MessageType
{
    kConnect = 1,
    kUpdateTreeCtrl = 2,
    kUpdateListCtrl = 4,
    kRequestData = 8
};

class Packet
{
public:
    Packet();
    ~Packet();

    void Serialize(char * buffer);
    void DeSerialize(char * buffer);

    MessageType messageType;
    int dataSize;

    Data data;
};
