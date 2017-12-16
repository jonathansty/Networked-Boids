#pragma once
#include "GamePacket.h"
#include "MessageList.h"
class ConnectionManager
{
public:
	ConnectionManager(float timeout);
	~ConnectionManager();

	/* Delete copy operator and assignment  */
	ConnectionManager(const ConnectionManager&) = delete;
	ConnectionManager& operator=(const ConnectionManager&) = delete;

public:
	int CheckSender(const sf::IpAddress& address, int port);
	/* Update connections and time them out.*/
	void Update(float dt);

	bool Disconnect(const sf::IpAddress& address, int port);
	size_t GetConnectionsCount() const { return m_Connections.size(); }
	ConnectionData GetConnectionData(const sf::IpAddress& addr,int port) const;

	using connIter = std::map<AddressPair, ConnectionData>::iterator;
	connIter begin() { return m_Connections.begin();}
	connIter end() { return m_Connections.end(); }
private:
	std::map<AddressPair, ConnectionData> m_Connections;
	
	float m_TimeOut;
};




