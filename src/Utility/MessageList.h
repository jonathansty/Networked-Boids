#pragma once
#include <list>
#include "GamePacket.h"

namespace sf {
	class Packet;
}

/* a MessageList can be specified with a maximum length. It will extend up until that length and discard any more messages */
template<typename C, int T>
class MessageList
{
public:

	void push(const sf::Packet& msg)
	{
		bool added = false;
		if (m_Packets.size() > 0)
		{
			/* Copy our packet and header */
			sf::Packet c = msg;
			PacketHeader h;
			c >> h;

			auto it = m_Packets.begin();
			while (it != m_Packets.end())
			{
				sf::Packet curr = *it;
				PacketHeader currH;
				curr >> currH;

				if (h.sequenceId == currH.sequenceId)
				{
					/* Duplicated packet m8 */
					return;
				}
				/* if this header is smaller than the packet we want to insert */
				if (h.time > currH.time)
				{
					m_Packets.insert(it, msg);

					if ((int)m_Packets.size() > MAX_SIZE)
						m_Packets.pop_back();
					break;
				}
				++it;
			}
		}
		else
			m_Packets.push_front(msg);

	}
	void clear()
	{
		m_Packets.clear();
	}

	size_t GetSize()const { return m_Packets.size(); }

	static int GetMaxSize() { return MAX_SIZE; }


	std::vector<PacketWrapper> GetMessages(int count = 2)
	{
		assert(count < MAX_SIZE);
		if(count > GetSize())
		{
			return{};
		}
		std::vector<PacketWrapper> result;
		auto it = m_Packets.begin();
		for(int i = 0; i < count; ++i)
		{
			sf::Packet p = *it;
			PacketWrapper pw;
			p >> pw.h;
			pw.data = p;
			result.push_back(pw);
			++it;
		}
		return result;
	}
private:
	static const int MAX_SIZE;
	std::list<C> m_Packets;
};

template <typename C, int T>
const int MessageList<C, T>::MAX_SIZE = T;

/* typedefs for simplicity */
using AddressPair = std::pair<sf::IpAddress, int>;
using MessageMap = std::map<MessageType, MessageList<sf::Packet, 3>>;

/* Implementation */
