/*
 * To start a server type ./src_sfml.exe -b 127.0.0.1:5555
 * To start a client and connect to a server type ./src_sfml.exe -c 127.0.0.1:5555
 */
#include "stdafx.h"
#include "Game.h"
#include "Objects/Boid.h"
#include "Server.h"
#if PLATFORM_WINDOWS
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 
#endif
int main(int argc, char **argvs)
{
#if PLATFORM_WINDOWS
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(2598);
#endif

	AppType t = AppType::Client;

	sf::IpAddress a;
	AbstractGame* game = nullptr;
	if (argc >= 3)
	{
		std::string a1 = argvs[1];
		if (a1 == "-s")
		{
			std::string filename = argvs[2];
			t = AppType::Server;


			std::cout << "Starting server..." << std::endl;
			Server* s = new Server(true);
			game = s;

			/* Read server Ip from file */
			if (filename.empty())
				filename = "servers.txt";

			std::ifstream file("Resources/" + filename);
			if (file.fail())
			{
				std::cout << "Failed to open configuration file." << std::endl;
				abort();
			}

			std::string line;
			std::vector<std::pair<AddressPair,int>> addresses;
			while (std::getline(file, line))
			{
				size_t commentidx = line.find('#');
				if (commentidx != std::string::npos)
				{
					line = line.substr(0, commentidx);
				}
				if (line.size() > 0)
				{
					std::stringstream ss;
					ss << line;
				
					std::string ip;
					ss >> ip;
					std::string simNum;
					ss >> simNum;
					if (ip.size() > 0)
					{
						size_t semicolon = ip.find(":");
						AddressPair tmpAddress = { sf::IpAddress(ip.substr(0, semicolon)), stoi(ip.substr(semicolon+1)) };
						addresses.push_back({ tmpAddress,std::stoi(simNum) });
					}
				}
			}
			std::cout << "Server binding to " << addresses.size() << "" << std::endl;
			s->Bind(addresses);
		}
		if (a1 == "-c")
		{
			std::string ip = argvs[2];
			size_t portIdx = ip.find(":");
			std::string port = ip.substr(portIdx + 1);
			ip = ip.substr(0, portIdx);
			int serverPort = stoi(port);
			sf::IpAddress serverIp = sf::IpAddress(ip);
			Game* pGame = new Game();
			game = pGame;
			pGame->SetServerIp(serverIp, serverPort);

		}
	}
	else
	{
		Game* pGame = new Game();
		game = pGame;
		pGame->SetServerIp(sf::IpAddress(127, 0, 0, 1), 5555);
	}

	/* Load default fallbacks */
	Settings::m_DefaultFont.loadFromFile("Resources/FreeSans.ttf");
	Settings::bDebug = false;

	if(game != nullptr)
		game->run();
	delete game;
}