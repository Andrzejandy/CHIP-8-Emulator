#include "LISTVIEW.h"

LISTVIEW::LISTVIEW()
{
	defaultFont.loadFromFile("C:/Windows/Fonts/arial.ttf");
	text.setFont(defaultFont);
	text.setStyle(sf::Text::Regular);
	text.setFillColor(sf::Color::White);
	text.setCharacterSize(10);
	text.setString("DEFAULT TEXT");
	oneLineHeight = text.getGlobalBounds().height;
	oneLineWidth = text.getGlobalBounds().width;
	maxElementsWidth = 8;
	maxElementsHeight = 8;
	firstElement = 0;
	selectedElement = -1;
	active = 1;
	border = 1;

	borderShape.setFillColor(sf::Color(190, 190, 190, 8));
	borderShape.setOutlineColor(sf::Color(200, 200, 200, 255));
	borderShape.setOutlineThickness(1);
	borderShape.setSize(sf::Vector2f(text.getGlobalBounds().width, text.getGlobalBounds().height));

	selectedShape.setFillColor(sf::Color(190, 190, 190, 25));
	selectedShape.setSize(sf::Vector2f(text.getGlobalBounds().width, oneLineHeight));
	VerticalList = true;
}

LISTVIEW::~LISTVIEW()
{
}

void LISTVIEW::clear()
{
	maxElementsWidth = 8;
	maxElementsHeight = 8;
	firstElement = 0;
	selectedElement = -1;
	active = 1;
	border = 1;
	std::vector<std::string>().swap(listElements);
	text.setString("");
}

sf::Sprite& LISTVIEW::getSprite()
{
	return listSprite;
}

sf::RectangleShape & LISTVIEW::getRectangleShape()
{
	return borderShape;
}

sf::Texture& LISTVIEW::getTexture()
{
	return listTexture;
}

sf::Text& LISTVIEW::getText()
{
	return text;
}

std::vector<std::string>& LISTVIEW::getElements()
{
	return listElements;
}

void LISTVIEW::addElement(std::string text, int index)
{
	if (index < listElements.size())
	{
		listElements.at(index) = text;
	}
	else
	{
		listElements.push_back(text);
	}
	
}

void LISTVIEW::selectElement(int sEl)
{
	if (sEl >= 0 && sEl < listElements.size())
	{
		selectedElement = sEl;
	}
	update();
}

void LISTVIEW::nextElement()
{
	firstElement++;
	std::cout << " selected elemenet: " << firstElement << std::endl;
	if (firstElement >= listElements.size() - 2)
	{
		firstElement = listElements.size() - 2;
	}
	update();
}

void LISTVIEW::previousElement()
{
	firstElement--;
	if (firstElement < 0)
	{
		firstElement = 0;
	}
	update();
}

void LISTVIEW::moreElements()
{
	maxElementsHeight++;
	if (maxElementsHeight > listElements.size())
	{
		maxElementsHeight = listElements.size();
	}
	update();
}

void LISTVIEW::lessElements()
{
	maxElementsHeight--;
	if (maxElementsHeight <= 0)
	{
		maxElementsHeight = 1;
	}
	update();
}

void LISTVIEW::update()
{
	std::stringstream ss;
	std::string wholeline;
	int newLineIndex;
		for (int n = firstElement; n < firstElement + maxElementsHeight; n++)
		{
			if (n < listElements.size())
				ss << listElements[n];
			if (!VerticalList)
			{
				wholeline = ss.str();
				wholeline.replace(wholeline.begin(), wholeline.end(),  "\n", ' ');
				ss.str(wholeline);
			}
		}
		
		text.setString(ss.str());
		ss.str("");
	
	if (border)
	{
		borderShape.setSize(sf::Vector2f(text.getGlobalBounds().width, text.getGlobalBounds().height));
	}
	if (selectedElement >= 0)
	{
		selectedShape.setFillColor(sf::Color(190, 190, 190, 25));
		selectedShape.setSize(sf::Vector2f(text.getGlobalBounds().width, oneLineHeight));
	}
	else
	{
		selectedShape.setFillColor(sf::Color(190, 190, 190, 0));
		selectedShape.setSize(sf::Vector2f(text.getGlobalBounds().width, oneLineHeight));
	}
}

void LISTVIEW::setVerticalList(bool isVertical)
{
	VerticalList = isVertical;
	update();
}

void LISTVIEW::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// apply the entity's transform -- combine it with the one that was passed by the caller
	states.transform *= getTransform(); // getTransform() is defined by sf::Transformable

										// apply the texture
	states.texture = &listTexture;

	// you may also override states.shader or states.blendMode if you want

	// draw the vertex array
	if (border)
	{
		target.draw(borderShape, states);
	}
	if (selectedElement >= 0)
	{
		target.draw(selectedShape, states);
	}
	target.draw(text, states);
}