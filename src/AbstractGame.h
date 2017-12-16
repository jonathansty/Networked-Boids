#pragma once

class AbstractGame
{
public:
	virtual ~AbstractGame(){}

	virtual void run() = 0;
	virtual void GameStart() = 0;
	virtual void GameEnd() = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;
};

