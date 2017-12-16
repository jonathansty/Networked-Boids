#pragma once
#include <SFML/Graphics.hpp>
#include "AbstractGame.h"
#include "Objects/Boid.h"
#include "Utility/MessageList.h"
#include "Utility/ConnectionManager.h"


class Server : public AbstractGame
{
public:
	Server(bool graphical = false);

	virtual ~Server();

	void run() override;
	void GameStart() override;
	void GameEnd() override;
	void Update(float dt) override;
	void Draw() override;

	/*
	* Binds the server to an available address in the connection list, if possible.
	* @param conns Connections to try binding to.
	*/
	void Bind(const std::vector<std::pair<AddressPair,int>>& conns);


	/*
	* Returns the render window created through SFML
	*/
	sf::RenderWindow* GetRenderWindow() { return &m_Window; }

	/*
	* Gets the elapsed time
	*/
	sf::Time GetElapsed() const { return m_Timer.getElapsedTime(); }

	/*
	* Gets the mouse position on the screen
	* @param clientId the id of the client for which we want to get the mouse position
	*/
	sf::Vector2f GetMousePos(int clientId)const {
		if (m_ConnMousePos.find(clientId) != m_ConnMousePos.end())
			return m_ConnMousePos.at(clientId);
		else
			return{ 0,0 };
	}

	/*
	* Sets the maximum amount of boids to be simulated on 1 server before offloading the simulation to a different server
	* @param n the maximum number of boids
	*/
	void SetSimulationNumber(int n) { m_SimNum = n; }

private:
	void Initialize();
	void pollEvent();
	void ProcessSend();
	void ProcessReceive();
	void Predict(float dt);
	void Send(const sf::Packet&, const sf::IpAddress&, int port);
	/* Send packet to all clients */
	void BroadcastPacket(const sf::Packet&);
	void BroadcastPacketToServer(const sf::Packet&);
	/* Construction Methods */
	Boid* CreateBoid(const sf::Vector2f& pos, float angle, int clientId = -1);

	/* Graphical Views */
	bool m_bGraphical;
	sf::RenderWindow m_Window;
	sf::View m_WorldView;
	sf::Clock m_Timer;
	sf::Clock m_NetTimer;
	sf::Clock m_PingTimer;
	sf::Clock m_FrameTimer;

	/* Debug info*/
	float m_FPS;
	float m_Latency;
	float m_deltaTime;
	unsigned int m_FrameCounter;
	unsigned int m_FrameCounterFPS;
	int m_IndexSimOffset = 0;
	unsigned int m_SimNum = unsigned(~0);

	/* Struct wrapper to keep boid positions for interpolation. Used to reduce bandwidth used. */
	struct PositionData
	{
		sf::Vector2f pos;
		sf::Vector2f vel;
	};
	std::map<int,PositionData> m_BoidsPrediction;

	/* Actual simulated boid collection */
	std::map<int,Boid*> m_Boids;
	std::vector<Boid*> m_OwnedByThisServer;


	float m_TimeOffset = 0;
	/* Debug text*/
	sf::Text m_InfoText;

	/* Networking */
	sf::UdpSocket m_AppSocket;

	/* Map to hold P2P master servers */
	ConnectionManager m_ClientConnectionManager;
	ConnectionManager m_ServerConnectionManager;
	std::vector<sf::Packet> m_SendMessages;
	/* Map that links clientId's to mouse positions */
	std::map<int, sf::Vector2f> m_ConnMousePos;
	std::map<int, sf::Color> m_ConnColor;
	/* Message queue */
	/* Each message protocol is represented as an int */
	/* Will hold all different types of messages */
	MessageMap m_Messages;

	/* Reliability control? */
	unsigned int m_LocalSequence;
	unsigned int m_RemoteSequence;
	unsigned int m_ServerId = (unsigned int)INVALID_CLIENT_ID;
	MessageList<sf::Packet, 100>m_LastReceivedMessages;
};


