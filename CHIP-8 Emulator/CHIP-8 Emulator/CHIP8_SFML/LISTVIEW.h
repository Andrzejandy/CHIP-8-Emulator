#pragma once
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <SFML/Graphics.hpp>
class LISTVIEW : public sf::Drawable, public sf::Transformable
{
private:
	sf::Sprite listSprite;
	sf::Texture listTexture;
	std::vector<std::string> listElements;
	sf::RectangleShape borderShape;
	sf::RectangleShape selectedShape;

	bool active;
	bool border;

	bool VerticalList;

	bool scroolAbleHorizontal;
	bool scroolAbleVertical;


	sf::Font defaultFont;
	sf::Text text;
public:
	int maxElementsHeight;
	int maxElementsWidth;
	int firstElement;
	int selectedElement;
	int oneLineWidth;
	int oneLineHeight;
	LISTVIEW();
	~LISTVIEW();

	void clear();

	sf::Sprite& getSprite();
	sf::RectangleShape& getRectangleShape();
	sf::Texture& getTexture();

	sf::Text& getText();
	std::vector<std::string>& getElements();

	void addElement(std::string text, int index);

	void selectElement(int sEl);

	void nextElement();

	void previousElement();

	void moreElements();

	void lessElements();

	void update();

	void setVerticalList(bool isVertical);

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};