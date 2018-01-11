#include "stdafx.h"
#include "Game.h"
#include "Objects/Boid.h"
#include "Utility/GamePacket.h"
Game::Game() :
	m_AppType(AppType::Client),
	m_FPS(30),
	m_Latency(0),
	m_FrameCounter(0),
	m_FrameCounterFPS(0),
	m_AppSocket(),
	m_ClientId(0),
	m_ClientColor(rand() % 255,rand()%255,rand()%255 ),
	m_LocalSequence(0),
	m_RemoteSequence(0)
{
}


Game::~Game()
{
}

void Game::run()
{
	sf::Clock netTimer;
	sf::Clock frameTimer;
	sf::Clock timer;
	sf::Time dt;

	/* Set non-blocking sockets */
	m_AppSocket.setBlocking(false);
	GameStart();

	//sf::Thread network{&Game::threadedNetworking,this};
	//network.launch();
	while (m_Window.isOpen())
	{
		dt = timer.restart();

		/* Handle input events and sends */
		pollEvent();

		Update(dt.asSeconds());

		/* Receive always!*/
		networkClientReceive();
		if (netTimer.getElapsedTime().asSeconds() > Settings::NET_RATE)
		{
			netTimer.restart();
			networkClientSend();
		}

		m_Window.clear();
		Draw();
		m_Window.display();

		++m_FrameCounter;
		++m_FrameCounterFPS;
		/* Update FPS*/
		if (frameTimer.getElapsedTime().asSeconds() > 1.0f)
		{
			frameTimer.restart();
			m_FPS = int(m_FrameCounterFPS / 1.0f);
			m_FrameCounterFPS = 0;
		}

	}
	GameEnd();
}

void Game::GameStart()
{
	m_Timer.restart();
	m_PingTimer.restart();
	sf::String title = "Network Systems(";
	switch (m_AppType)
	{
	case AppType::Server:
		title += "server)";
		break;
	case AppType::Client:
		title += "client)";
		break;
	default: break;
	}
	m_Window.create(sf::VideoMode(800, 600), title, sf::Style::Default & ~sf::Style::Resize);
	//m_Window.setVerticalSyncEnabled(true);

	m_InfoText = sf::Text{ "dt",Settings::m_DefaultFont,12 };

	m_WorldView = m_Window.getDefaultView();
	m_WorldView.zoom(2.0f);
	m_Window.setView(m_WorldView);

	Initialize();

	/* Send Start message to server*/
	PacketHeader h;
	h.protocol = int(MessageType::Start);
	Send(h, {});

}
void Game::Initialize()
{

}

/* GameEnd */
void Game::GameEnd()
{
	/* Disconnect from server */
	PacketHeader h;
	h.protocol = (int)MessageType::Quit;
	Send(h, {});

	/* Process the end messages */
	networkClientSend();

	/* Remove all objects from the scene */
	for (auto KeyPair : m_Obstacles)	delete KeyPair.second;
	m_Obstacles.clear();
}

/* Updating functions */
void Game::Update(float dt)
{
	sf::String s = "dt: " + std::to_string(dt) + "\nnet_rate: " + std::to_string(Settings::NET_RATE) + "\nfps:" + std::to_string(m_FPS) + "\nLatency: " + std::to_string((int)m_Latency) + "\n";
	s += "Time Offset: " + std::to_string(m_TimeOffset) + "\n";
	s += "MsgCnt: ";
	for (auto& c : m_Messages)
		s += "\n\t" + std::to_string((int)c.first) + ": " + std::to_string(c.second.GetSize());

	s += "\nBoids: " + std::to_string(m_Obstacles.size());
	s += "\nClientId:" + std::to_string(m_ClientId);
	m_InfoText.setString(s);

	/* Interpolate between current and previous messages */
	for (auto it = m_Obstacles.begin(); it != m_Obstacles.end(); ++it)
	{
		std::vector<PacketWrapper> messages = m_PosMessages[it->first].GetMessages();
		/* Interpolate between the 2 messages */
		if (messages.size() >= 2)
		{
			PacketWrapper m1 = messages[0];
			PacketWrapper m2 = messages[1];

			float currTime = GetElapsed().asSeconds();

			/* Convert to Server Time properly */
			m2.h.time += m_TimeOffset;
			m1.h.time += m_TimeOffset;

			float dtPercentage = currTime - m2.h.time;
			float tot = m1.h.time - m2.h.time;
			dtPercentage /= tot;
			/* Interpolate between these */
			sf::Packet packet = m1.data;
			int size1;
			packet >> size1;
			sf::Packet packet2 = m2.data;
			int size2;
			packet2 >> size2;

			Boid* p = m_Obstacles[it->first];

			sf::Vector2f pos;
			packet >> pos.x >> pos.y;

			sf::Vector2f pos2;
			packet2 >> pos2.x >> pos2.y;

			sf::Vector2f fPos = pos2 + dtPercentage*(pos - pos2); // prediction
			sf::Vector2f playerPos = p->getPosition() + 0.25f*(fPos - p->getPosition()); // interpolation

			/* Calculate angle */
			sf::Vector2f dir = pos - pos2;
			/* Second smooth interpol between current pos and next*/
			float l = Math::dot(dir, dir);
			float vel = p->GetSpeed();
			if (l < 2 * vel*vel)
				p->setPosition(playerPos);
			else
				p->setPosition(pos2);
			
			/* Set our predicted rotation */
			float a = Math::AngleBetween({ 1,0 }, dir);
			p->setRotation(a);
		}
	}
}
void Game::Draw()
{
	for (auto pair : m_Obstacles)
		m_Window.draw(*pair.second);

	/* Set our view back to default for DEBUG UI */
	sf::View v = m_Window.getView();
	m_Window.setView(m_Window.getDefaultView());
	m_Window.draw(m_InfoText);
	m_Window.setView(v);
}

/* Set our destination IP */
void Game::SetServerIp(const sf::IpAddress& a, int port)
{
	std::cout << "Server ip: " << a << ":" << port << std::endl;
	m_ServerAddress.first = a;
	m_ServerAddress.second = port;
}

/* Poll event loops for client and server */
void Game::pollEvent()
{
	sf::Event e;
	while (m_Window.pollEvent(e))
	{
		if (e.type == sf::Event::Closed)
			m_Window.close();
		if (e.type == sf::Event::KeyPressed)
		{
			switch (e.key.code)
			{
			case sf::Keyboard::N:
				break;
			case sf::Keyboard::P:
				break;
			case sf::Keyboard::D: {
				Settings::bDebug = !Settings::bDebug;
				/* Send message to server */
				PacketHeader h;
				h.protocol = int(MessageType::Setting);
				sf::Packet packet{};
				packet << Settings::bDebug;
				std::cout << "Debug message send " << std::endl;
				Send(h,packet);

			}break;
			default:break;
			}
		}

		if (e.type == sf::Event::MouseButtonPressed) {
			if (e.mouseButton.button == sf::Mouse::Button::Left) {

				sf::Vector2i pos = sf::Mouse::getPosition(m_Window);
				sf::Vector2f transPos = m_Window.mapPixelToCoords(pos, m_WorldView);

				/* Generate more precise angle */
				float angle = (float)((rand() % 3600)/10.0f);

				sf::Packet inputEvent;
				PacketHeader h;
				h.protocol = int(MessageType::Input);

				/* Create our local boid */
				size_t newBoidId = m_Obstacles.size();
				Boid* b = new Boid{ transPos,angle };
				m_Obstacles[newBoidId] = b;
				b->SetBoidColor(m_ClientColor);
				b->SetSeperation(Math::Random(11.f, 100.f));
				b->SetCohesion(Math::Random(0.1f, 2.f));
				b->SetAlignment(Math::Random(0.f, 2.f));
				b->SetOwnerId(m_ClientId);
				b->SetServerOwnerId(-1);
				inputEvent << (int)InputType::CreateBoid << *b;

				Send(h,inputEvent);

			}
		}

		if (e.type == sf::Event::MouseMoved)
		{
			sf::Vector2i pos = { e.mouseMove.x,e.mouseMove.y };
			sf::Vector2f worldCoord = m_Window.mapPixelToCoords(pos, m_WorldView);
			/* Send */
			PacketHeader h;
			h.protocol = (int)MessageType::Input;
			sf::Packet p;
			p  << (int)InputType::MouseUpdate << worldCoord.x << worldCoord.y;
			Send(h,p);
		}
	}
}

/* Client receiving and sending */
void Game::networkClientReceive()
{
	/* Receive all messages and process */
	sf::Packet r;
	sf::IpAddress sender;
	unsigned short port;
	while (m_AppSocket.receive(r, sender, port) == sf::Socket::Done)
	{
		sf::Packet copy = r;
		PacketHeader h{  };
		r >> h;
		
		if (h.sequenceId > m_RemoteSequence)
			m_RemoteSequence = h.sequenceId;

		/* Push a copy of our message to the list */
		m_Messages[(MessageType)h.protocol].push(copy);

		if (h.protocol == (int)MessageType::Latency)
		{
			float sendAt;
			r >> sendAt;

			float currTime = GetElapsed().asSeconds();
			float delta = (currTime - sendAt) / 2.0f;
			m_Latency = m_Latency + 0.2f*((delta * 1000) - m_Latency);

			m_TimeOffset = currTime - h.time;
		}
		if (h.protocol == (int)MessageType::GameState)
		{
			int id;
			sf::Vector2f pos;
			r >> id >> pos.x >> pos.y;

			/* Only add obstacle when it does not exist yet*/
			if(m_Obstacles.find(id) == m_Obstacles.end())
				m_Obstacles[id] = new Boid(pos, 0.0f);

			m_PosMessages[id].push(copy);
		}
		if (h.protocol == (int)MessageType::OtherConnQuit)
		{
			int ids;
			r >> ids;
			for (int i = 0; i < ids; ++i)
			{
				int id;
				r >> id;
				delete m_Obstacles[id];
				m_Obstacles.erase(id);
			}
			m_PosMessages.clear();
		}
		if (h.protocol == (int)MessageType::Start)
		{
			m_ClientId = h.clientId;
			std::cout << "START" << std::endl;
		}
		if (h.protocol == (int)MessageType::Setting)
		{
			bool b;
			r >> b;
			Settings::bDebug = b;
			std::cout << "Debug " << b << std::endl;
		}


	}
}
void Game::networkClientSend()
{
	/* Send a ping update */
	if (m_PingTimer.getElapsedTime().asSeconds() > Settings::PING_RATE) {
		m_PingTimer.restart();

		PacketHeader h;
		h.protocol = (int)MessageType::Latency;
		Send(h, {});
	}

	/* Process message queue */
	for (sf::Packet& s : m_SendMessages)
	{
		int er = (m_AppSocket.send(s, m_ServerAddress.first, m_ServerAddress.second));
		if (er == sf::Socket::Status::Error)
		{
			std::cout << "Send failed (" << er << ")" << std::endl;
		}
	}
	m_SendMessages.clear();
}
void Game::Send(PacketHeader h,  const sf::Packet& p)
{
	/* Queue up messages for a send */
	h.clientId = m_ClientId;
	h.time = GetElapsed().asSeconds();
	h.sequenceId = m_LocalSequence;
	//h.ackId = m_RemoteSequence;
	//h.ackBitfield = 0;

	sf::Packet n;
	n << h;
	n.append(p.getData(), p.getDataSize());

	/* Queue message and update local sequence */
	++m_LocalSequence;
	m_SendMessages.push_back(n);
}