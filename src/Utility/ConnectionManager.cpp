#include "stdafx.h"
#include "ConnectionManager.h"

ConnectionManager::ConnectionManager(float timeout) :m_TimeOut(timeout)
{
}

ConnectionManager::~ConnectionManager()
{
	m_Connections.clear();
}

int ConnectionManager::CheckSender(const sf::IpAddress & address, int port)
{
	std::pair<sf::IpAddress, int> conn{ address,port };
	int id = -1;
	if (m_Connections.find(conn) == m_Connections.end())
	{
		// New connection
		id = int(m_Connections.size());
		m_Connections[conn].id = id;
		// Return our new id
	}

	/* Reset our received time */
	m_Connections[conn].timeSinceLast = 0;
	return id;
}

void ConnectionManager::Update(float dt)
{
	for (auto it = m_Connections.begin(); it != m_Connections.end(); )
	{
		it->second.timeSinceLast += dt;

		if (m_TimeOut > 0.002f && it->second.timeSinceLast > m_TimeOut) {
			std::cout << "[CONNECTION]: " << it->first.first << it->first.second << " timed out. " << std::endl;
			m_Connections.erase(it++);
		}
		else
			++it;
	}
}
bool ConnectionManager::Disconnect(const sf::IpAddress & address, int port) {
	auto it = m_Connections.find({ address,port });
	if (it != m_Connections.end()) {
		m_Connections.erase(it);
		return true;
	}
	return false;
}
ConnectionData ConnectionManager::GetConnectionData(const sf::IpAddress & addr,int port) const
{
	return m_Connections.at({ addr, port });
}
