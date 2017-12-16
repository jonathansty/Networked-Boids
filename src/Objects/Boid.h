#pragma once
#include "../stdafx.h"
#include "ViewShape.h"

class Server;
class Boid : public sf::Drawable,public sf::Transformable
{
public:
	Boid(sf::Vector2f pos,float angle = 0.0f);
	~Boid();

	Boid(const Boid& b);
	Boid& operator=(const Boid& b) = delete;


public:
	
	virtual void initialize();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void update(float dt);

	void SetBoidColor(const sf::Color& c)
	{
		for (size_t i = 0; i < m_Vertices.getVertexCount(); ++i)
			m_Vertices[i].color = c;
		m_BoidColor = c;
	}
	void rotateTowards(float angle,float t);
	void rotateTowards(const sf::Vector2f newDir, float t);


	/* Getters and setters */
	sf::Vector2f GetVelocity()			const	{ return m_Speed*m_Direction; }
	float	GetSpeed()					const	{ return m_Speed; }
	void	SetOwnerId(int id)					{ m_OwnerId = id; }
	int		GetOwnerId()				const	{ return m_OwnerId; }
	void	SetServerOwnerId(int id)			{ m_ServerOwnerId = id; }
	int		GetServerOwnerId()			const	{ return m_ServerOwnerId; }
	unsigned int GetBoidId()			const { return m_BoidId; }
	void SetboidId(unsigned int id) { m_BoidId = id; }
	void SetSeperation(float force) { m_SepForce = force; };
	void SetCohesion(float force)	{ m_CohForce = force; }
	void SetAlignment(float force)	{ m_AlignForce = force; };
	
	float GetSeperation() const { return m_SepForce;}
	float GetCohesion()	  const	{ return m_CohForce; }
	float GetAlignment()  const { return m_AlignForce; }
	sf::Color GetColor()  const { return m_BoidColor; }
	/* Static functions */
	static void SetParentGame(Server* const pGame){ m_pParentGame = pGame; }
	static void FixID();
private:
	void flocking(const std::vector<Boid*>& neighbours);


	/* VARS */
	sf::VertexArray m_Vertices;
	/* Internal properties */
	sf::Vector2f m_Direction;
	/* Boid characteristics */
	float m_Speed;
	float m_Range;
	float m_AngleDiff;

	ViewShape m_RangeVisual;
	sf::Vector2f m_Velocity;
	sf::Vector2f m_PrevPos;
	sf::Vector2f m_CurrPos;

	sf::VertexArray m_CohVisual;
	sf::VertexArray m_SepVisual;
	sf::VertexArray m_AlignVisual;

	sf::Vector2f m_SepDir;
	sf::Vector2f m_CohDir;
	sf::Vector2f m_AlignDir;

	sf::Vector2f m_AvoidanceDir;
	sf::Text m_MyText;
	
	sf::Color m_BoidColor;

	/* id of the client that 'owns' this boid */
	int m_OwnerId;
	int m_ServerOwnerId;
	unsigned int m_BoidId;
	/* statics */
	static std::list<Boid*> m_AliveBoids;
	static Server* m_pParentGame;

	/* Start forces */
	static const float SEP_FORCE;
	static const float COH_FORCE;
	static const float ALIGN_FORCE;
	
	float m_SepForce;
	float m_CohForce;
	float m_AlignForce;

	friend sf::Packet& operator >> (sf::Packet& stream,Boid& rhs);
};

inline sf::Packet& operator >>(sf::Packet& stream, Boid& rhs)
{
	sf::Vector2f pos;
	float a;
	stream >> rhs.m_BoidId >> rhs.m_ServerOwnerId >> rhs.m_OwnerId
	>> pos.x >> pos.y >> a
	>> rhs.m_SepForce >> rhs.m_CohForce >> rhs.m_AlignForce
	>> rhs.m_BoidColor.r >> rhs.m_BoidColor.g >> rhs.m_BoidColor.b;

	rhs.setPosition(pos);
	rhs.setRotation(a);
	rhs.SetBoidColor(rhs.m_BoidColor);
	return stream;

}
inline sf::Packet& operator<<(sf::Packet& stream, const Boid& rhs)
{
	return stream << rhs.GetBoidId() << rhs.GetServerOwnerId() << rhs.GetOwnerId()
		<< rhs.getPosition().x << rhs.getPosition().y << rhs.getRotation()
		<< rhs.GetSeperation() << rhs.GetCohesion() << rhs.GetAlignment()
		<< rhs.GetColor().r << rhs.GetColor().g << rhs.GetColor().b;
}
