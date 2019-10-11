#pragma once
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <SFML/Graphics.hpp>

class BUTTON : public sf::Drawable, public sf::Transformable
{
	bool active;
	bool border;
	


	std::string name;
	sf::Font defaultFont;
public:
	sf::Text text;
	sf::Vector2f size;
	sf::Vector2f position;
	int selectedElement;
	sf::Sprite bSprite;
	sf::Texture bTexture;
	sf::RectangleShape bshape;

	bool hovered;
	bool pressed;
	bool released;

	BUTTON(float px, float py, float sx = 25, float sy = 25);
	bool b_contains(sf::Vector2f pos);

	~BUTTON();

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};