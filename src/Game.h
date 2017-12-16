#pragma once
#include "Objects/Boid.h"
#include "Utility/MessageList.h"
#include "AbstractGame.h"
/*
* Distributed simulation on multi server model
* Each server will be connected as a p2p configuration owning and simulating a part of the game world
* Currently only client server model 
*/
/* Interface */
enum class AppType
{
	Server,
	Client
};
class Game : public AbstractGame
{
public:
	Game();
	~Game();


	void run() override;
	void GameStart() override;
	void GameEnd() override;
	void Update(float dt) override;
	void Draw() override;

	void SetServerIp(const sf::IpAddress& a, int port);

	sf::RenderWindow* GetRenderWindow() { return &m_Window; }

	sf::Time GetElapsed() const { return m_Timer.getElapsedTime(); }

private:
	void Initialize();
	void pollEvent();

	void networkClientSend();
	void networkClientReceive();

	/* Sends the packet immediately */
	void Send(PacketHeader h, const sf::Packet& p);

	/* Graphical Views */
	sf::RenderWindow m_Window;
	sf::View m_WorldView;
	sf::Clock m_Timer;
	sf::Clock m_PingTimer;

	/* App Settings */
	AppType m_AppType;

	/* Debug info*/
	int m_FPS;
	float m_Latency;
	unsigned int m_FrameCounter;
	unsigned int m_FrameCounterFPS;
	std::map<int,Boid*> m_Obstacles;
	float m_TimeOffset = 0;
	/* Debug text*/
	sf::Text m_InfoText;

	/* Networking */
	sf::UdpSocket m_AppSocket;
	std::pair<sf::IpAddress, int> m_ServerAddress;
	std::map<std::pair<sf::IpAddress, int>,float> m_Connections;

	unsigned int m_ClientId = (unsigned int)INVALID_CLIENT_ID;
	unsigned int m_LocalSequence;
	unsigned int m_RemoteSequence;

	sf::Color m_ClientColor;
	/* Message queue */
	std::vector<sf::Packet> m_SendMessages;

	/* TEMP buffer for last received messages for reliability purposes*/
	MessageList<sf::Packet, 100> m_LastReceivedMessages;
	/* All World Related messages */
	/* Each message protocol is represented as an int */
	using MessageMap = std::map<MessageType, MessageList<sf::Packet,200>>;
	/* Will hold all different types of messages */
	MessageMap m_Messages;

	std::map<int, MessageList<sf::Packet, 100>> m_PosMessages;


};

