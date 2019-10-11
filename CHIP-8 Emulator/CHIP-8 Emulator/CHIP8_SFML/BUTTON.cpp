#include "BUTTON.h"

BUTTON::BUTTON(float px, float py, float sx, float sy)
{
	defaultFont.loadFromFile("C:/Windows/Fonts/arial.ttf");
	text.setFont(defaultFont);
	text.setStyle(sf::Text::Regular);
	text.setFillColor(sf::Color::Green);
	text.setCharacterSize(8);
	text.setString("BUTTON");
	position.x = px;
	position.y = py;
	size.x = sx;
	size.y = sy;
	bshape.setSize(size);
	bshape.setFillColor(sf::Color(190, 190, 190, 8));
	bshape.setOutlineColor(sf::Color(200, 200, 200, 255));
	bshape.setOutlineThickness(1);
	bshape.setPosition(position);
	text.setPosition(position.x + (size.x / 16), position.y + (size.y / 4));
	border = 1;
}

bool BUTTON::b_contains(sf::Vector2f pos)
{
	return false;
}

BUTTON::~BUTTON()
{
}

void BUTTON::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// apply the entity's transform -- combine it with the one that was passed by the caller
	states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

										// apply the texture
	states.texture = &bTexture;

	// you may also override states.shader or states.blendMode if you want

	// draw the vertex array
	if (border)
	{
		target.draw(bshape, states);
	}
	target.draw(text, states);
}