#pragma once
#include "../stdafx.h"
#define INVALID_CLIENT_ID ~0
#define INVALID_PROTOCOL ~0

struct PacketHeader
{

	/* Reliabilty/ordering */
	unsigned int protocol = (unsigned int)INVALID_PROTOCOL;
	unsigned int sequenceId = (unsigned int)0;
	unsigned int clientId = (unsigned int)INVALID_CLIENT_ID;
	float time = -1;
	bool fromServer = false;
};

struct PacketWrapper
{
	PacketHeader h;
	sf::Packet data;
};
inline sf::Packet& operator<<(sf::Packet& packet, const PacketHeader& m)
{
	return packet << m.protocol << m.sequenceId/* << m.ackId << m.ackBitfield */ << m.clientId << m.time << m.fromServer;
}
inline sf::Packet& operator >> (sf::Packet& packet, PacketHeader& m)
{
	return packet >> m.protocol >> m.sequenceId /*>> m.ackId >> m.ackBitfield >> */ >> m.clientId >> m.time >> m.fromServer;
}
enum class MessageType : int
{
	GameState = 0,
	Setting = 1,
	Start = 2,
	Input = 3,
	Latency = 4,
	Quit = 5,
	OtherConnQuit = 6,
};
enum class InputType : int
{
	CreateBoid = 0,
	MouseUpdate = 1,
};
/* Specific enums for our different message types */

/* int is the client ID */
struct ConnectionData
{
	int id;
	float timeSinceLast;
	sf::Vector2i mousePos;
};

