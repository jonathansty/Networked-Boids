#include "stdafx.h"
#include "Server.h"

Server::Server(bool graphical) :
	m_bGraphical(graphical),
	m_FPS(30),
	m_Latency(15),
	m_FrameCounter(0),
	m_FrameCounterFPS(0),
	m_TimeOffset(0),
	m_InfoText(),
	m_LocalSequence(0),
	m_RemoteSequence(0),
	m_ClientConnectionManager(Settings::TIMEOUT_VAL),
	m_ServerConnectionManager(Settings::TIMEOUT_VAL)
{
}


Server::~Server()
{
}

void Server::run()
{
	bool m_bIsRunning = true;
	sf::Clock delta;

	/* Game starting */
	GameStart();
	while (m_bIsRunning)
	{
		float dt = delta.restart().asSeconds();

		Update(dt);

		/* Network sending and receiving */
		Predict(dt);
		ProcessReceive();
		if (m_NetTimer.getElapsedTime().asSeconds() > Settings::NET_RATE) {
			m_NetTimer.restart();
			ProcessSend();
		}


		/* Poll Event ands draw*/
		if (m_bGraphical)
		{
			m_bIsRunning = m_Window.isOpen();
			pollEvent();
			m_Window.clear();
			Draw();
			m_Window.display();
		}
	}
	/* Game Ending*/
	GameEnd();
}

void Server::GameStart()
{
	if (m_bGraphical)
	{
		m_Window.create(sf::VideoMode(800, 600), "Graphical Server window", sf::Style::Default & ~sf::Style::Resize);
		//m_Window.setVerticalSyncEnabled(true);
	}
	m_InfoText = sf::Text{ "infoText",Settings::m_DefaultFont,12 };

	m_WorldView = m_Window.getDefaultView();
	m_WorldView.zoom(2.0f);
	m_Window.setView(m_WorldView);

	m_AppSocket.setBlocking(false);

	m_Timer.restart();
	m_NetTimer.restart();
	Boid::SetParentGame(this);
	Initialize();
}

void Server::GameEnd()
{

	for (auto it = m_Boids.begin(); it != m_Boids.end(); ++it)
		delete it->second;

	m_Messages.clear();
	m_SendMessages.clear();
	m_Boids.clear();
	m_OwnedByThisServer.clear();
}

void Server::Predict(float dt) {
	/* Updating all our objects */
	/* Each server will have a simulation range and will exchange p2p data with each other */
	for (size_t i = 0; i < m_OwnedByThisServer.size(); ++i) {
		m_OwnedByThisServer[i]->update(dt);

		/* Update our predicted set */
		if (m_BoidsPrediction[m_OwnedByThisServer[i]->GetBoidId()].vel == sf::Vector2f{0, 0})
		{
			PositionData d;
			d.pos = m_OwnedByThisServer[i]->getPosition();
			d.vel = m_OwnedByThisServer[i]->GetVelocity();
			m_BoidsPrediction[m_OwnedByThisServer[i]->GetBoidId()] = d;
		}
		else {
			m_BoidsPrediction[i].pos += m_BoidsPrediction[i].vel;
		}
	}
}

void Server::Update(float dt)
{
	m_deltaTime = dt;
	sf::String s = "dt: " + std::to_string(dt) + "\nnet_rate: " + std::to_string(Settings::NET_RATE) + "\nfps:" + std::to_string(m_FPS) + "\nLatency: " + std::to_string((int)m_Latency) + "\n";
	s += "Connections: " + std::to_string(m_ClientConnectionManager.GetConnectionsCount());
	s += "\nLocalSeq:" + std::to_string(m_LocalSequence);
	s += "\nRemoteSed:" + std::to_string(m_RemoteSequence);
	s += "\nServers:" + std::to_string(m_ServerConnectionManager.GetConnectionsCount());
	s += "\nBoids: " + std::to_string(m_Boids.size());
	m_InfoText.setString(s);

	++m_FrameCounter;
	++m_FrameCounterFPS;

	float updateTime = 0.1f;
	if (m_FrameTimer.getElapsedTime().asSeconds() > updateTime)
	{
		m_FrameTimer.restart();
		m_FPS = m_FPS + 0.5f*((m_FrameCounterFPS / updateTime) - m_FPS);
		m_FrameCounterFPS = 0;
	}
	if (m_PingTimer.getElapsedTime().asSeconds() > Settings::PING_RATE)
	{
		m_PingTimer.restart();
		PacketHeader h;
		h.protocol = (int)MessageType::Latency;
		h.fromServer = true;
		h.time = GetElapsed().asSeconds();
		h.sequenceId = m_LocalSequence;
		sf::Packet p;
		p << h;
		BroadcastPacketToServer(p);
	}
	m_ClientConnectionManager.Update(dt);
	m_ServerConnectionManager.Update(dt);
}

void Server::Draw()
{
	for (auto &b : m_Boids)
	{
		if(b.second)
			m_Window.draw(*b.second);
	}

	/* Set our view back to default for DEBUG UI */
	sf::View v = m_Window.getView();
	m_Window.setView(m_Window.getDefaultView());
	m_Window.draw(m_InfoText);
	m_Window.setView(v);
}

void Server::Bind(const std::vector<std::pair<AddressPair,int>>& conns)
{
	int serverId = 0;
	for (auto it = conns.begin(); it != conns.end(); it++) {
		bool isBound = false;
		if (m_AppSocket.getLocalPort() == 0)
		{
			const sf::IpAddress& a = it->first.first;
			int port = it->first.second;

			std::cout << "Binding server to " << a << ":" << port << std::endl;
			sf::Socket::Status s = m_AppSocket.bind(port, a);
			if (s == sf::Socket::Status::Done)
			{
				std::cout << "Server successfully bound to " << a << ":" << port << std::endl;
				std::cout << "Simulation count is " << it->second << std::endl;
				SetSimulationNumber(it->second);
				isBound = true;
				m_ServerId = serverId;
			}
			else
				m_ServerConnectionManager.CheckSender(it->first.first, it->first.second);
		}
		else
		{
			m_ServerConnectionManager.CheckSender(it->first.first, it->first.second);
		}
		m_ServerId++;
	}
}

void Server::Initialize()
{
	//int cnt = 0;
	//float step = 360.0f / cnt;
	//for (int i = 0; i < cnt; ++i)
	//{
	//	sf::Vector2f pos = sf::Vector2f(m_Window.getSize()) / 2.0f;
	//	CreateBoid(pos, 0.0f, -1);
	//}
}

void Server::pollEvent()
{
	sf::Event e;
	while (m_Window.pollEvent(e))
	{
		if (e.type == sf::Event::Closed) {
			m_Window.close();
		}
		if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape)
			m_Window.close();
		else if (e.type == sf::Event::KeyPressed)
		{
			if (e.key.code == sf::Keyboard::D)
				Settings::bDebug = !Settings::bDebug;
		}
	}
}

void Server::ProcessSend()
{
	/* Send pos updates*/
	/* For each boid send our position data over */

	/* Check our updating for boids*/
	std::vector<int> ids;
	for (size_t i = 0; i < m_OwnedByThisServer.size(); ++i)
	{
		Boid* b = m_OwnedByThisServer[i];
		/* if dist is longer than vel * 2 update our boid */
		sf::Vector2f simulatedPos = b->getPosition();
		sf::Vector2f predictedPos = b->getPosition() + b->GetVelocity()*55.f;
		if(m_BoidsPrediction.size() > i)
			predictedPos = m_BoidsPrediction[i].pos;

		float len = Math::length(simulatedPos - predictedPos);
		if (len > 0.2f*b->GetSpeed()*m_deltaTime)
		{
			ids.push_back(i);
			m_BoidsPrediction[i].pos = simulatedPos;
			m_BoidsPrediction[i].vel = b->GetVelocity()*m_deltaTime;
		}
	}

	/* Sends our boids that need updates */
	for (size_t i = 0; i < ids.size(); ++i) {
		Boid* p = m_OwnedByThisServer[ids[i]];

		sf::Packet pack;
		PacketHeader h;
		h.protocol = (int)MessageType::GameState;
		h.time = GetElapsed().asSeconds();
		h.sequenceId = m_LocalSequence;
		pack << h << p->GetBoidId() << p->getPosition().x << p->getPosition().y;
		m_SendMessages.push_back(pack);

		h.fromServer = true;
		sf::Packet pack2;
		pack2 << h << p->GetBoidId() << p->getPosition().x << p->getPosition().y << p->getRotation();
		BroadcastPacketToServer(pack2);
	}

	for (auto it = m_SendMessages.begin(); it != m_SendMessages.end(); ++it) {
		BroadcastPacket(*it);
	}
	m_SendMessages.clear();

}

void Server::ProcessReceive()
{
	/* Receive stuff */
	sf::Packet r;
	sf::IpAddress sender;
	unsigned short port;
	while (m_AppSocket.receive(r, sender, port) == sf::Socket::Done)
	{
		/* Extract + copy packet */
		sf::Packet copy = r;
		PacketHeader h;
		r >> h;
		m_Messages[(MessageType)h.protocol].push(copy);

		/* Update connections */
		std::pair<sf::IpAddress, int> conn = { sender,port };

		int id = -1;
		if (!h.fromServer)
			id = m_ClientConnectionManager.CheckSender(conn.first, conn.second);
		else
			id = m_ServerConnectionManager.CheckSender(conn.first, conn.second);
		if (id != -1 && h.protocol == int(MessageType::Start))
		{
			PacketHeader idHeader;
			idHeader.protocol = int(MessageType::Start);
			idHeader.clientId = id;
			idHeader.sequenceId = m_LocalSequence;
			sf::Packet p;
			p << idHeader;
			Send(p, conn.first, conn.second);
		}
		if (h.fromServer && h.protocol == int(MessageType::GameState))
		{
			/* Broadcast to all Clients */
			unsigned int id=INVALID_CLIENT_ID;
			sf::Vector2f pos;
			float a;
			r >> id >> pos.x >> pos.y >> a;
			//assert( id >= 0 && id < m_Boids.size() && "ID received by our client is out of range");
			if(m_Boids[id] != nullptr){
				m_Boids[id]->setPosition(pos);
				m_Boids[id]->setRotation(a);
				BroadcastPacket(copy);
			}
		}
		if (h.protocol == int(MessageType::Start))
		{
			/* Get all positions and send to client */
			for (auto it = m_Boids.begin(); it != m_Boids.end();++it) {
				const Boid* b =it->second;
				sf::Vector2f pos = b->getPosition();

				sf::Packet p;
				PacketHeader h;
				h.protocol = int(MessageType::GameState);
				h.time = GetElapsed().asSeconds();

				p << h << it->first << pos.x << pos.y;
				BroadcastPacket(p);

				p.clear();
				/* Send a packet that belongs to 1 second in the future */
				//h.time += 1.0f;
				//pos += b->GetVelocity();
				//p << h << i << pos.x << pos.y;
				//BroadcastPacket(p);
			}
		}
		if (h.protocol == int(MessageType::Quit))
		{
			if (!h.fromServer)
			{
				int id = m_ClientConnectionManager.GetConnectionData(sender, port).id;

				std::vector<int> ids;
				bool isDc = m_ClientConnectionManager.Disconnect(sender, port);
				if (isDc) {
					std::cout << "Client at " << conn.first << ":" << conn.second << " has disconnected from the server." << std::endl;
					m_ConnMousePos.erase(h.clientId);
				}

				/* Remove all my boids */
				m_BoidsPrediction.clear();
				m_OwnedByThisServer.clear();
				for (auto it = m_Boids.begin(); it != m_Boids.end();)
				{
					Boid *b = it->second;
					if (b->GetServerOwnerId() == m_ServerId && b->GetOwnerId() == id)
					{
						delete it->second;
						ids.push_back(it->first);
						m_Boids.erase(it++);
					}
					else
					{
						++it;
						if (b->GetServerOwnerId() == m_ServerId)
						{
							m_OwnedByThisServer.push_back(b);
						}
					}

				}
				/* Broadcast it */
				PacketHeader toServers = h;
				toServers.fromServer = true;
				toServers.time = GetElapsed().asSeconds();
				toServers.sequenceId = m_LocalSequence;
				toServers.protocol = (int)MessageType::Quit;

				/* Add id's to send */
				sf::Packet b;
				b << toServers << m_ServerId;
				b << sf::Uint64(ids.size());
				for (size_t i = 0; i < ids.size(); ++i)
					b << ids[i];
				BroadcastPacketToServer(b);

				PacketHeader quit = h;
				quit.protocol = (int)MessageType::OtherConnQuit;
				quit.fromServer = true;
				quit.time = GetElapsed().asSeconds();
				quit.sequenceId = m_LocalSequence;
				sf::Packet connQuit;
				connQuit << quit << sf::Uint64(ids.size());
				for (size_t i = 0; i < ids.size(); ++i)
					connQuit << ids[i];

				BroadcastPacket(connQuit);

			}
			else
			{
				int clientId = h.clientId;
				int serverId;
				r >> serverId;

				/* Delete the boids associated with clientId and serverId */

				int ids;
				r >> ids;
				PacketHeader quit = h;
				quit.protocol = (int)MessageType::OtherConnQuit;
				quit.fromServer = true;
				sf::Packet connQuit;
				connQuit << quit << ids;

				for (int i = 0; i < ids; ++i)
				{
					int id;
					r >> id;

					delete m_Boids[id];
					m_Boids.erase(id);

					connQuit << id;
				}

				BroadcastPacket(connQuit);
			}
		}
		/* Toggle debug */
		if (h.protocol == int(MessageType::Setting))
		{
			bool d;
			r >> d;
			Settings::bDebug = d;
			if (!h.fromServer) {

				sf::Packet p;
				PacketHeader h2 = h;
				h2.fromServer = true;
				h2.time = GetElapsed().asSeconds();
				p << h2 << d;
				BroadcastPacketToServer(p);
			}
			else if( h.fromServer)
			{
				sf::Packet p;
				PacketHeader h2 = h;
				h2.time = GetElapsed().asSeconds();
				p << h2 << d;
				BroadcastPacket(p);
			}
		}
		if (h.protocol == int(MessageType::Latency))
		{
			if (!h.fromServer)
			{
				PacketHeader t;
				t.protocol = (int)MessageType::Latency;
				t.time = GetElapsed().asSeconds();
				t.sequenceId = m_LocalSequence;
				sf::Packet pack;
				pack << t << h.time;
				//BroadcastPacket(pack);
				Send(pack, sender, port);
			}
		}
		if (h.protocol == int(MessageType::Input))
		{
			int t;
			r >> t;

			switch ((InputType)t) {
			case InputType::MouseUpdate: {

				float x, y;
				r >> x >> y;
				m_ConnMousePos[h.clientId] = { x,y };
				//if (!h.fromServer)
				//{
				//	sf::Packet pack;
				//	PacketHeader h2 = h;
				//	h2.time = GetElapsed().asSeconds();
				//	h2.sequenceId = m_LocalSequence;
				//	h2.fromServer = true;
				//	pack << h2 << t << x << y;
				//	BroadcastPacketToServer(pack);
				//}
			}break;
			case InputType::CreateBoid:
			{
				if (!h.fromServer)
				{
					/* Create a new boid at position ... */
					int clientId = h.clientId;
					sf::Vector2f pos;
					Boid* b = new Boid{ {0,0},0 };
					r >> *b;
					m_Boids[b->GetBoidId()] = b;
					std::cout << "Created a new boid at position " << b->getPosition().x << "," << b->getPosition().y << ". " << std::endl;
					/* Create our boid */
					if (m_OwnedByThisServer.size() < m_SimNum)
					{
						m_OwnedByThisServer.push_back(b);
						b->SetServerOwnerId(m_ServerId);
					}
					b->initialize();
					b->SetOwnerId(clientId);
					PositionData d;
					d.pos = b->getPosition();
					d.vel = b->GetVelocity();
					m_BoidsPrediction[b->GetBoidId()] = d;
					/* Broadcast to all our servers */
					sf::Packet p;
					PacketHeader h2;
					h2.protocol = (int)MessageType::Input;
					h2.time = GetElapsed().asSeconds();
					h2.sequenceId = m_LocalSequence;
					h2.clientId = h.clientId;
					h2.fromServer = true;
					p << h2 << t << *b;
					BroadcastPacketToServer(p);
				}
				else{
					
					Boid* b = new Boid({ 0,0 }, 0);
					r >> *b;
					std::cout << "Received create boid message from server... " << std::endl;
					std::cout << "Created a new boid " << b->GetServerOwnerId() << " at position " << b->getPosition().x << "," << b->getPosition().y << ". " << std::endl;
					m_Boids[b->GetBoidId()] = b;
					PositionData d;
					d.pos = b->getPosition();
					d.vel = b->GetVelocity();
					m_BoidsPrediction[b->GetBoidId()] = d;
					if (b->GetServerOwnerId() < 0 && m_OwnedByThisServer.size() < m_SimNum)
					{
						// Claim this boid 
						m_OwnedByThisServer.push_back(b);
					}
					b->initialize();
				}
			}
			break;
			default:
				std::cout << "Unrecognized input type: " << t << std::endl;
				break;
			}

		}
	}
}

void Server::Send(const sf::Packet & pack, const sf::IpAddress & sender, int port)
{
	++m_LocalSequence;
	if (m_AppSocket.send(pack.getData(), pack.getDataSize(), sender, port) == sf::Socket::Status::Error) {
		std::cout << "Failed to send packet to  " << sender << ":" << port << std::endl;
	}
}

void Server::BroadcastPacket(const sf::Packet & packet)
{
	for (auto& addr : m_ClientConnectionManager)
		Send(packet, addr.first.first, addr.first.second);
}

void Server::BroadcastPacketToServer(const sf::Packet & packet)
{

	for (auto& addr : m_ServerConnectionManager)
		Send(packet, addr.first.first, addr.first.second);
}

Boid* Server::CreateBoid(const sf::Vector2f & pos, float angle, int clientId)
{
	Boid* newBoid = new Boid(pos, angle);

	auto it = m_ConnColor.find(clientId);
	if (it == m_ConnColor.end())
	{
		sf::Uint8 offset = 80;
		sf::Uint8 randomRange = 255 - 80;

		auto getRandomUint8 = [offset, randomRange]() {
			return sf::Uint8(offset + rand() % randomRange);
		};

		m_ConnColor[clientId] = sf::Color{ getRandomUint8(),getRandomUint8(),getRandomUint8() };
	}
	newBoid->SetBoidColor(m_ConnColor.at(clientId));
	newBoid->SetOwnerId(clientId);
	newBoid->initialize();

	m_Boids[newBoid->GetBoidId()] = newBoid;

	/* Add our position data for prediction */
	PositionData d;
	d.pos = newBoid->getPosition();
	d.vel = newBoid->GetVelocity();
	m_BoidsPrediction[newBoid->GetBoidId()] = d;

	return newBoid;
}
