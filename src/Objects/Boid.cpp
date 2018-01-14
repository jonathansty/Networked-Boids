#include "stdafx.h"
#include "../Server.h"
#include "Boid.h"

std::list<Boid*> Boid::m_AliveBoids;
Server* Boid::m_pParentGame;

/* Initialize our start forces */
const float Boid::SEP_FORCE = 7.5f;
const float Boid::COH_FORCE = 2.0f;
const float Boid::ALIGN_FORCE  = 2.0f;

Boid::Boid(sf::Vector2f pos,float angle):
	m_Vertices{sf::Triangles},
	m_Direction(1,0),
	m_Speed(55.0f),
	m_Range(45),
	m_AngleDiff(180.0f),
	m_RangeVisual(m_AngleDiff,m_Range),
	m_Velocity(0,0),
	m_PrevPos(0,0),
	m_CurrPos(0,0),
	m_CohVisual{sf::Lines,2},
	m_SepVisual{sf::Lines,2},
	m_AlignVisual{sf::Lines,2},
	m_SepDir(0,0),
	m_CohDir(0,0),
	m_AlignDir(0,0),
	m_AvoidanceDir(0,0),
	m_MyText{},
	m_BoidColor(sf::Color::White),
	m_OwnerId{INVALID_CLIENT_ID},
	m_ServerOwnerId{INVALID_CLIENT_ID},
	m_SepForce(SEP_FORCE),
	m_CohForce(COH_FORCE),
	m_AlignForce(ALIGN_FORCE)
{
	m_BoidId = unsigned(m_AliveBoids.size());
	m_AliveBoids.push_back(this);
	setPosition(pos);
	setRotation(angle);

	/* Set our vertices only the first time */
	m_Vertices.append(sf::Vertex({-10,-5}, sf::Color::White, {0,0}));
	m_Vertices.append(sf::Vertex({10,0}, sf::Color::White, {0,0}));
	m_Vertices.append(sf::Vertex({-10,5}, sf::Color::White, {0,0}));
	m_RangeVisual.setFillColor(sf::Color(0, 60, 120, 40));

	m_CohVisual[0].color = sf::Color::Blue;
	m_CohVisual[1].color = sf::Color::Blue;

	m_SepVisual[0].color = sf::Color::Red;
	m_SepVisual[1].color = sf::Color::Red;

	m_AlignVisual[0].color = sf::Color::Green;
	m_AlignVisual[1].color = sf::Color::Green;

}

Boid::~Boid()
{
	m_AliveBoids.remove(this);
	//TODO: Optimize this
	FixID();
}

void Boid::FixID()
{
	int id = 0;
	for (auto it = m_AliveBoids.begin(); it != m_AliveBoids.end(); ++it)
	{
		Boid* b = *it;
		b->m_BoidId= (id);
		id++;
	}
}
Boid::Boid(const Boid& b): Boid(b.getPosition(),b.getRotation())
{
}

void Boid::initialize()
{
	m_Direction = {cos(getRotation()*3.1415f/180.0f),sin(getRotation()*3.1415f/180.0f)};
	m_MyText = sf::Text("Server Owner: " + std::to_string(m_ServerOwnerId), *Settings::m_DefaultFont, 12);
}

void Boid::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if(Settings::bDebug)
	{
		target.draw(m_CohVisual,states);
		target.draw(m_SepVisual,states);
		target.draw(m_AlignVisual,states);
		sf::Transform pos;
		pos.translate(getPosition());
		states.transform *= pos;
		target.draw(m_MyText, states);
		states.transform *= pos.getInverse();
		states.transform *= getTransform();
		target.draw(m_RangeVisual,states);
		states.transform *= getTransform().getInverse();

	}
	
	states.transform *= getTransform();
	target.draw(m_Vertices, states);
}

void Boid::update(float dt)
{
	// Update direction value
	m_PrevPos = m_CurrPos;
	m_CurrPos = getPosition();
	m_Velocity = m_CurrPos - m_PrevPos;

	float angle = this->getRotation();
	m_Direction = {cos(angle*3.1415f/180.0f),sin(angle*3.1415f/180.0f)};

	// Get neighbors
	double maxDot = cos(m_AngleDiff*3.1415 /180.0f);
	std::vector<Boid*> m_Neighbours;
	std::map<int, int> m_idMap;
	for(auto it = m_AliveBoids.begin(); it != m_AliveBoids.end(); ++it)
	{
		if (*it == this)
			continue;

		Boid& p = *(*it);
		sf::Vector2f dirToBoid = p.getPosition()-getPosition();
		sf::Vector2f nDir = Math::Normalize(dirToBoid);

		float dp = Math::dot(dirToBoid, m_Direction);
		
		float z = Math::Cross(m_Direction, dirToBoid);

		float l = Math::length(dirToBoid);
		if(dp < maxDot  || l > m_Range)
		{
			// Not a neighbor
			continue;
		}

		if(p.m_OwnerId != m_OwnerId)
			m_idMap[p.m_OwnerId]++;

		// Add to neighbor list
		m_Neighbours.push_back(*it);
		
	}

	/* Get our new owner ID*/
	float perc = 0.5;
	int newId = m_OwnerId;
	for (auto it = m_idMap.begin(); it != m_idMap.end(); ++it)
	{
		float currPerc = (float)it->second / m_Neighbours.size();
		if (currPerc > perc)
		{
			newId = it->first;
			perc = currPerc;
		}
	}
	if (m_OwnerId != newId)
	{
		m_OwnerId = newId;

		/* TODO: Create a map of boid colors linked to clientId */
		SetBoidColor(sf::Color::Red);
	}


	sf::Vector2f fDir = { 0,0 };
	if (m_OwnerId != INVALID_CLIENT_ID) {
		sf::Vector2f mousePos = m_pParentGame->GetMousePos(m_OwnerId);
		sf::Vector2f mouseDir = mousePos -  getPosition() ;
		mouseDir = Math::Normalize(mouseDir);
		fDir += 0.05f*mouseDir;
	}
	if(m_Neighbours.size() > 0)
	{
		flocking(m_Neighbours);
		fDir += m_AlignForce * m_AlignDir + m_CohForce * m_CohDir + m_SepForce * m_SepDir;
	}

	fDir = Math::Normalize(fDir);
	rotateTowards(fDir,1.0f*dt);
	// Apply moving in the direction
	sf::RenderWindow* gpWindow = m_pParentGame->GetRenderWindow();
	sf::Vector2f pos = {(float)sf::Mouse::getPosition(*gpWindow).x, (float)sf::Mouse::getPosition(*gpWindow).y};
	sf::Vector2f toMouse = Math::Normalize(pos - getPosition());

	// Look at that pos 
	float d = toMouse.x * m_Direction.x + toMouse.y * m_Direction.y;

	sf::Vector2f dir{m_Speed * m_Direction.x * dt,m_Speed * m_Direction.y * dt};
	move(dir);

	/* Loop around */
	sf::Vector2f s = gpWindow->getView().getSize();
	sf::Vector2f center = gpWindow->getView().getCenter();
	sf::FloatRect size{ center.x - s.x / 2.0f,center.y - s.y / 2.0f,s.x,s.y };

	sf::Vector2f pos2 = sf::Vector2f(getPosition());
	if (pos2.x > size.left + size.width + 1)
		pos2.x = size.left;
	if(pos2.y > size.top + size.height + 1)
		pos2.y = size.top;
	if (pos2.x < size.left)
		pos2.x = size.left + size.width;
	if (pos2.y < size.top)
		pos2.y = size.top + size.height;
	setPosition(sf::Vector2f(pos2));

	/* Set visuals*/
	float l = 25.0f;
	m_AlignVisual[0].position = getPosition();
	m_AlignVisual[1].position = getPosition() + l*m_AlignDir;
	m_CohVisual[0].position = getPosition();
	m_CohVisual[1].position = getPosition() + l*m_CohDir;
	m_SepVisual[0].position = getPosition();
	m_SepVisual[1].position = getPosition() + l*m_SepDir;

	/* Setting debug text */
	m_MyText.setString("ClientId: " + std::to_string(m_OwnerId) + "\nServerId: " + std::to_string(m_ServerOwnerId) + "\nBoidId" + std::to_string(m_BoidId));
}

void Boid::rotateTowards(float angle, float t)
{
	float curr = getRotation();

	float a1 = curr;
	float a2 = angle;

	float inter = a1 + (a2 - a1)*t;

	setRotation(inter);
}

/* Rotates towards the mouse using an angular speed */
void Boid::rotateTowards(const sf::Vector2f newDir,float dt)
{
	sf::Vector2f unit{ 1,0 };
	sf::Vector2f n = Math::Normalize(newDir);
	/* Make sure newDir is normalize*/
	float a = float(acos(Math::dot(unit, n))*180.0f/Math::PI);

	float c = Math::Cross(m_Direction, n);
	float dp = Math::dot(m_Direction, n);

	float rotSpeed = 200.0f*dt;
	if (c >  0)
	{
		setRotation(getRotation() + rotSpeed);
	}
	if (c < 0)
	{
		setRotation(getRotation() - rotSpeed);
	}
}

void Boid::flocking(const std::vector<Boid*>& neighbours)
{
		// Separation: Steer away from flock mates
		 m_SepDir ={ 0,0 };
		// Alignment: Align towards average heading of local flock mates
		m_AlignDir = { 0,0 };

		// Cohesion
		sf::Vector2f avgPos{ 0,0 };
		
		/* Loop through */
		for(const Boid* n : neighbours)
		{
			/* seperation direction */
			sf::Vector2f tmpSep = n->getPosition() - getPosition();
			m_SepDir += tmpSep;

			/* Cohesion: Steer to move towards average position of flock */
			avgPos += n->getPosition();

			const Boid* e = reinterpret_cast<const Boid*>(n);
			m_AlignDir += e->m_Direction;

		}
		/* Steer towards our final look direction */
		/* Each aspect gets counted as 1/3th of our simulation */
		m_SepDir = Math::Normalize(-m_SepDir);
		m_AlignDir = Math::Normalize(m_AlignDir);
		avgPos.x /= neighbours.size();
		avgPos.y /= neighbours.size();

		m_CohDir = Math::Normalize(avgPos - getPosition());
}
