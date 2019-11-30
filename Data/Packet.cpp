#include "Packet.h"

Packet::Packet()
{
}

Packet::~Packet()
{
}

void Packet::Serialize(char* buffer)
{
    memcpy(buffer, this, sizeof(Packet));
}

void Packet::DeSerialize(char* buffer)
{
    memcpy(this, buffer, sizeof(Packet));
}