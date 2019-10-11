#include <iostream>
#include <sstream>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <filesystem>
#include "LISTVIEW.h"
#include "HIP8.h"
#include "BUTTON.h"

using namespace std;

std::vector<pair<string, string>> ROMS; // Name, Path
int selectedROM;

namespace fs = std::experimental::filesystem;

sf::Font defaultFont;


#define KEY_MAP_0 
#define KEY_MAP_1
#define KEY_MAP_2
#define KEY_MAP_3
#define KEY_MAP_4
#define KEY_MAP_5
#define KEY_MAP_6
#define KEY_MAP_7
#define KEY_MAP_8
#define KEY_MAP_9
#define KEY_MAP_A
#define KEY_MAP_B
#define KEY_MAP_C
#define KEY_MAP_D
#define KEY_MAP_E
#define KEY_MAP_F

sf::Uint8 sfpixel[DISPLAY_WIDTH*DISPLAY_HEIGHT * 4];

LISTVIEW MemList;
LISTVIEW StackList;
LISTVIEW RegistryList;
sf::RectangleShape rs_bg;

enum EXECUTION_TYPE
{
	PAUSED,
	SINGLE_STEP,
	CONTINIOUS
};

char * EXECUTION_TYPE_TXT[] = {
	"PAUSED" ,
	"SINGLE_STEP" ,
	"CONTINIOUS"
};

int exectype;
bool SingleStepExecuted;

sf::Text instructionText;
sf::Text ciText;
unsigned int ci;

BUTTON btn_step(10, 240, 25, 25);
BUTTON btn_back(40, 240, 25, 25);
BUTTON btn_more(70, 240, 25, 25);
BUTTON btn_less(100, 240, 25, 25);
BUTTON btn_reset(130, 240, 25, 25);


BUTTON btn_PAUSED_execution(10, 270, 50, 25);
BUTTON btn_SINGLE_STEP_execution(10, 300, 75, 25);
BUTTON btn_CONTINIOUS_execution(10, 330, 75, 25);

sf::Text ipsText;
unsigned int ips;

sf::Text execText;

CHIP8 chip8;
sf::Clock clk;
float zoom;

void updateExecutionType(int selectedType)
{
	switch (selectedType)
	{
	case(PAUSED) :
		exectype = PAUSED;
		btn_PAUSED_execution.bshape.setOutlineColor(sf::Color(0, 220, 0));
		btn_SINGLE_STEP_execution.bshape.setOutlineColor(sf::Color(200, 30, 0));
		btn_CONTINIOUS_execution.bshape.setOutlineColor(sf::Color(200, 30, 0));
		break;
	case(SINGLE_STEP) :
		exectype = SINGLE_STEP;
		btn_PAUSED_execution.bshape.setOutlineColor(sf::Color(200, 30, 0));
		btn_SINGLE_STEP_execution.bshape.setOutlineColor(sf::Color(0, 220, 0));
		btn_CONTINIOUS_execution.bshape.setOutlineColor(sf::Color(200, 30, 0));
		break;
	case(CONTINIOUS) :
		exectype = CONTINIOUS;
		btn_PAUSED_execution.bshape.setOutlineColor(sf::Color(200, 30, 0));
		btn_SINGLE_STEP_execution.bshape.setOutlineColor(sf::Color(200, 30, 0));
		btn_CONTINIOUS_execution.bshape.setOutlineColor(sf::Color(0, 220, 0));
		break;
	}

	execText.setString("Status: " + std::string(EXECUTION_TYPE_TXT[exectype]));


}


void setupBtnText()
{
	defaultFont.loadFromFile("C:/Windows/Fonts/arial.ttf");
	SingleStepExecuted = true;
	ips = 10;
	btn_step.text.setString("STEP");
	btn_back.text.setString("BACK");
	btn_more.text.setString("MORE");
	btn_less.text.setString("LESS");
	btn_reset.text.setString("RESET");

	btn_PAUSED_execution.text.setString("PAUSE");
	btn_SINGLE_STEP_execution.text.setString("SINGLE_STEP");
	btn_CONTINIOUS_execution.text.setString("CONTINIOUS");

	ipsText.setPosition(btn_step.bshape.getPosition().x, btn_step.bshape.getPosition().y - 60);
	cout << "POSITON: " << ipsText.getPosition().y << endl;
	ipsText.setFont(defaultFont);
	execText.setFont(defaultFont);

	ipsText.setStyle(sf::Text::Regular);
	ipsText.setFillColor(sf::Color::Green);
	ipsText.setCharacterSize(12);
	ipsText.setString("ips: " + ips);

	instructionText.setPosition(10,210);
	instructionText.setFont(defaultFont);

	instructionText.setStyle(sf::Text::Regular);
	instructionText.setFillColor(sf::Color::Green);
	instructionText.setCharacterSize(12);
	instructionText.setString("A - Next ROM, D - Prev. ROM");
	exectype = PAUSED;
	btn_PAUSED_execution.bshape.setOutlineColor(sf::Color(0, 220, 0));
	btn_SINGLE_STEP_execution.bshape.setOutlineColor(sf::Color(200, 30, 0));
	btn_CONTINIOUS_execution.bshape.setOutlineColor(sf::Color(200, 30, 0));
	execText.setStyle(sf::Text::Regular);
	execText.setFillColor(sf::Color::Green);
	execText.setCharacterSize(12);
	execText.setString("Status: " + std::string(EXECUTION_TYPE_TXT[exectype]));
	execText.setPosition(btn_PAUSED_execution.bshape.getPosition().x + 67, btn_PAUSED_execution.bshape.getPosition().y + 3);
}

void readFile(string filename, unsigned char memory8[], unsigned short PROGRAM_ADDRESS)
{
	string line;
	ifstream myfile;
	stringstream ss;
	unsigned short opcode = 0x00;
	myfile.open(filename, ifstream::binary | ifstream::in);
	myfile.seekg(0, std::ios::end);
	std::streamsize size = myfile.tellg();
	myfile.seekg(0, std::ios::beg);

	std::vector<char> rom;
	std::vector<char>().swap(rom);
	rom.resize(size);
	std::cout << std::endl << "ROM: " << filename << std::endl;
	if (myfile.read(&rom.front(), size)) {
		
		myfile.close();
	}

	memcpy(&memory8[0+PROGRAM_ADDRESS], &rom.front(), rom.size());
	//std::cout << "OPCODE:" << std::endl;
	for (int n = 0; n < rom.size(); n+=2)
	{
		opcode = memory8[PROGRAM_ADDRESS+n] << 8 | memory8[PROGRAM_ADDRESS + n + 1];
		//std::cout << "[" << std::dec<<n/2 << "]:" << "0x" <<
		//	std::uppercase << std::setfill('0')
		//	<< std::setw(4)<< std::hex << opcode << std::endl;
		
		ss << "Addr: " << "[" << std::hex << n + PROGRAM_ADDRESS << "]: " << "0x" << 
			std::uppercase << std::setfill('0')
			<< std::setw(4) << std::hex << opcode << "\n";
		MemList.getElements().push_back(ss.str());
		ss.str("");
	}
	//std::cout << std::endl << "ROM: " << filename << std::endl;


}

bool loadRoms(string directoryPath)
{
	string fileName;
	string filePath;
	selectedROM = 0;
	for (const auto& d : fs::directory_iterator(directoryPath))
	{
		string elem = d.path().string();
		if (elem.substr(elem.find_last_of(".") + 1) == "rom")
		{
			fileName = elem.substr(elem.find_last_of("\\") + 1, elem.find_last_of(".") + 1);
			filePath = elem;
			ROMS.push_back(make_pair(fileName, filePath));
		}
		else if (elem.substr(elem.find_last_of(".") + 1) == "ch8")
		{
			fileName = elem.substr(elem.find_last_of("\\") + 1, elem.find_last_of(".") + 1);
			filePath = elem;
			ROMS.push_back(make_pair(fileName, filePath));
		}
		else
		{
		}
	}
	if (ROMS.empty())
	{
		return false;
	}

	return true;
}

void updateLists()
{
	stringstream ss;
	RegistryList.maxElementsHeight = DATA_REGISTRY_SIZE;
	StackList.maxElementsHeight = STACK_SIZE;
	for (int n = 0; n < STACK_SIZE; n++)
	{
		ss << "Stack[" << n << "]: " << "0x" << std::uppercase << std::hex << chip8.getStack8()[n] << std::dec << '\n';
		StackList.getElements()[n] = ss.str();
		ss.str("");
	}
	for (int n = 0; n < DATA_REGISTRY_SIZE; n++)
	{
		ss << "R[" << n << "]: " << "0x" << std::uppercase << std::hex << chip8.getData_registers8()[n] << std::dec << '\n';
		RegistryList.getElements()[n] = ss.str();
		ss.str("");
	}

	RegistryList.update();
	StackList.update();
}

void setup8(CHIP8& c)
{
	c.Reset();

	MemList.clear();
	RegistryList.clear();
	StackList.clear();
	RegistryList.getElements().resize(DATA_REGISTRY_SIZE);
	StackList.getElements().resize(STACK_SIZE);
	RegistryList.maxElementsHeight = DATA_REGISTRY_SIZE;
	StackList.maxElementsHeight = STACK_SIZE;
	readFile(ROMS[selectedROM].second, c.getMemory8(), c.getPROGRAM_ADDRESS());
	
	MemList.update();
	updateLists();


}

void nextROM(CHIP8& c)
{
	selectedROM++;
	if (selectedROM >= ROMS.size())
	{
		selectedROM--;
		cout << " ROM: " << ROMS[selectedROM].first << endl;
	}
	setup8(c);
}

void previousROM(CHIP8& c)
{
	selectedROM--;
	if (selectedROM < 0)
	{
		selectedROM++;
		cout << " ROM: " << ROMS[selectedROM].first << endl;
	}
	setup8(c);
}

void ChipDraw(CHIP8& c)
{
	for (unsigned int x = 0; x < DISPLAY_HEIGHT; x++)
	{
		for (unsigned int y = 0; y < DISPLAY_WIDTH; y++)
		{
			sfpixel[((x * 4)*DISPLAY_WIDTH + (y * 4))] = c.getDisplay8(x, y) * 255;
			sfpixel[((x * 4)*DISPLAY_WIDTH + (y * 4) + 1)] = c.getDisplay8(x, y) * 255;
			sfpixel[((x * 4)*DISPLAY_WIDTH + (y * 4) + 2)] = c.getDisplay8(x, y) * 255;
			sfpixel[((x * 4)*DISPLAY_WIDTH + (y * 4) + 3)] = c.getDisplay8(x, y) * 255;
		}
	}
}


void Resized(sf::View &view)
{
	updateLists();
	cout << rs_bg.getGlobalBounds().width << " l " << rs_bg.getLocalBounds().width << endl;
	MemList.setPosition(rs_bg.getLocalBounds().width + 3, rs_bg.getLocalBounds().top + 3);
	RegistryList.setPosition(MemList.getPosition().x + MemList.getRectangleShape().getGlobalBounds().width + 1, MemList.getPosition().y);
	StackList.setPosition(RegistryList.getPosition().x + RegistryList.getRectangleShape().getLocalBounds().width + 1, StackList.getPosition().y);
}


using namespace std;



int main()
{
	sf::Texture texture;
	sf::RenderWindow window(sf::VideoMode(400, 400), "SFML works!");
	sf::View view = window.getDefaultView();
	sf::Sprite sprite;
	exectype = PAUSED;
	window.setView(view);
	texture.create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	texture.update(sfpixel);
	sprite.setPosition(2, 2);
	sprite.setTexture(texture);
	sprite.setScale(2,2);
	rs_bg.setPosition(sprite.getPosition());
	rs_bg.setSize(sf::Vector2f(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height));
	rs_bg.setOutlineColor(sf::Color::Green);
	rs_bg.setOutlineThickness(1);
	rs_bg.setFillColor(sf::Color(0, 0, 0, 255));
	loadRoms("./Roms");
	setupBtnText();
	setup8(chip8);
	zoom = 1.0;

	Resized(view);

	while (window.isOpen())
	{
		sf::Event event;
		static float Time = clk.getElapsedTime().asSeconds();
		while (window.pollEvent(event))
		{
			window.setTitle(ROMS[selectedROM].second);
			if (event.type == sf::Event::Closed)
				window.close();
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
				window.close();
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad0)) {
				chip8.setKey(255, 0x00);
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad1)) {
				chip8.setKey(255, 0x01);
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad2)) {
				chip8.setKey(255, 0x02);
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad3)) {
				chip8.setKey(255, 0x03);
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad4)) {
				chip8.setKey(255, 0x04);
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad5)) {
				chip8.setKey(255, 0x05);
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad6)) {
				chip8.setKey(255, 0x06);
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad7)) {
				chip8.setKey(255, 0x07);
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad8)) {
				chip8.setKey(255, 0x08);
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad9)) {
				chip8.setKey(255, 0x09);
			}


			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad0)) {
				chip8.setKey(0, 0x00);
			}

			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad1)) {
				chip8.setKey(0, 0x01);
			}

			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad2)) {
				chip8.setKey(0, 0x02);
			}

			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad3)) {
				chip8.setKey(0, 0x03);
			}

			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad4)) {
				chip8.setKey(0, 0x04);
			}

			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad5)) {
				chip8.setKey(0, 0x05);
			}

			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad6)) {
				chip8.setKey(0, 0x06);
			}

			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad7)) {
				chip8.setKey(0, 0x07);
			}

			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad8)) {
				chip8.setKey(0, 0x08);
			}

			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad9)) {
				chip8.setKey(0, 0x09);
			}


			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::A)) {
				nextROM(chip8);
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::C)) {}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::D)) {
				previousROM(chip8);
			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::E)) {}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::F)) {}
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::U)) {
				MemList.nextElement();
			}
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::J)) {
				MemList.previousElement();
			}
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::H)) {
				MemList.moreElements();
			}
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::K)) {
				MemList.lessElements();
			}
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::I)) {
				MemList.selectElement(MemList.selectedElement++);
			}
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Y)) {
				MemList.selectElement(MemList.selectedElement--);

			}

			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Z)) {
				chip8.setKey(255, 0x0A);
			}
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::X)) {
				chip8.setKey(255, 0x0B);
			}
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::C)) {
				chip8.setKey(255, 0x0C);
			}
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::V)) {
				chip8.setKey(255, 0x0D);
			}
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::B)) {
				chip8.setKey(255, 0x0E);
			}
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::N)) {
				chip8.setKey(255, 0x0F);
			}

			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Z)) {
				chip8.setKey(0, 0x0A);
			}
			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::X)) {
				chip8.setKey(0, 0x0B);
			}
			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::C)) {
				chip8.setKey(0, 0x0C);
			}
			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::V)) {
				chip8.setKey(0, 0x0D);
			}
			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::B)) {
				chip8.setKey(0, 0x0E);
			}
			if ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::N)) {
				chip8.setKey(0, 0x0F);
			}

			if (event.type == sf::Event::Resized)
			{
				view.setSize(event.size.width, event.size.height);
				window.setView(view);
				Resized(view);
			}
			if (event.type == sf::Event::MouseMoved)
			{
				sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
				sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
				if (btn_step.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_step.bshape.setFillColor(sf::Color::Blue);
					btn_step.hovered = true;

				}
				else
				{
					btn_step.bshape.setFillColor(sf::Color(190, 190, 190, 8));
					btn_step.hovered = false;

				}
				if (btn_back.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_back.bshape.setFillColor(sf::Color::Blue);
					btn_back.hovered = true;

				}
				else
				{
					btn_back.bshape.setFillColor(sf::Color(190, 190, 190, 8));
					btn_back.hovered = false;

				}
				if (btn_more.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_more.bshape.setFillColor(sf::Color::Blue);
					btn_more.hovered = true;

				}
				else
				{
					btn_more.bshape.setFillColor(sf::Color(190, 190, 190, 8));
					btn_more.hovered = false;

				}
				if (btn_less.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_less.bshape.setFillColor(sf::Color::Blue);
					btn_less.hovered = true;

				}
				else
				{
					btn_less.bshape.setFillColor(sf::Color(190, 190, 190, 8));
					btn_less.hovered = false;

				}
				if (btn_reset.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_reset.bshape.setFillColor(sf::Color::Blue);
					btn_reset.hovered = true;

				}
				else
				{
					btn_reset.bshape.setFillColor(sf::Color(190, 190, 190, 8));
					btn_reset.hovered = false;
				}
				if (btn_PAUSED_execution.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_PAUSED_execution.bshape.setFillColor(sf::Color::Blue);
					btn_PAUSED_execution.hovered = true;

				}
				else
				{
					btn_PAUSED_execution.bshape.setFillColor(sf::Color(190, 190, 190, 8));
					btn_PAUSED_execution.hovered = false;

				}
				if (btn_SINGLE_STEP_execution.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_SINGLE_STEP_execution.bshape.setFillColor(sf::Color::Blue);
					btn_SINGLE_STEP_execution.hovered = true;

				}
				else
				{
					btn_SINGLE_STEP_execution.bshape.setFillColor(sf::Color(190, 190, 190, 8));
					btn_SINGLE_STEP_execution.hovered = false;

				}
				if (btn_CONTINIOUS_execution.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_CONTINIOUS_execution.bshape.setFillColor(sf::Color::Blue);
					btn_CONTINIOUS_execution.hovered = true;

				}
				else
				{
					btn_CONTINIOUS_execution.bshape.setFillColor(sf::Color(190, 190, 190, 8));
					btn_CONTINIOUS_execution.hovered = false;

				}
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
				sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
				if (btn_step.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_step.text.setFillColor(sf::Color::Red);
					btn_step.pressed = true;
					btn_step.released = false;
				}
				if (btn_back.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_back.text.setFillColor(sf::Color::Red);
					btn_back.pressed = true;
					btn_back.released = false;

				}
				if (btn_more.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_more.text.setFillColor(sf::Color::Red);
					btn_more.pressed = true;
					btn_more.released = false;

				}
				if (btn_less.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_less.text.setFillColor(sf::Color::Red);
					btn_less.pressed = true;
					btn_less.released = false;
				}
				if (btn_reset.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_reset.text.setFillColor(sf::Color::Red);
					btn_reset.pressed = true;
					btn_reset.released = false;
				}
				if (btn_PAUSED_execution.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_PAUSED_execution.text.setFillColor(sf::Color::Red);
					btn_PAUSED_execution.pressed = true;
					btn_PAUSED_execution.released = false;

				}
				if (btn_SINGLE_STEP_execution.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_SINGLE_STEP_execution.text.setFillColor(sf::Color::Red);
					btn_SINGLE_STEP_execution.pressed = true;
					btn_SINGLE_STEP_execution.released = false;

				}
				if (btn_CONTINIOUS_execution.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_CONTINIOUS_execution.text.setFillColor(sf::Color::Red);
					btn_CONTINIOUS_execution.pressed = true;
					btn_CONTINIOUS_execution.released = false;

				}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
				sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
				if (btn_step.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_step.text.setFillColor(sf::Color::Green);
					btn_step.pressed = false;
					btn_step.released = true;
					SingleStepExecuted = false;
				}
				else
				{
					btn_step.text.setFillColor(sf::Color::Green);
				}
				if (btn_back.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_back.text.setFillColor(sf::Color::Green);
					btn_back.pressed = false;
					btn_back.released = true;

				}
				else
				{
					btn_back.text.setFillColor(sf::Color::Green);
				}
				if (btn_more.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_more.text.setFillColor(sf::Color::Green);
					btn_more.pressed = false;
					btn_more.released = true;

				}
				else
				{
					btn_more.text.setFillColor(sf::Color::Green);
				}
				if (btn_less.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_less.text.setFillColor(sf::Color::Green);
					btn_less.pressed = false;
					btn_less.released = true;

				}
				else
				{
					btn_less.text.setFillColor(sf::Color::Green);
				}

				if (btn_reset.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_reset.text.setFillColor(sf::Color::Green);
					btn_reset.pressed = false;
					btn_reset.released = true;
					setup8(chip8);
				}
				else
				{
					btn_reset.text.setFillColor(sf::Color::Green);
				}

				
				if (btn_PAUSED_execution.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_PAUSED_execution.text.setFillColor(sf::Color::Green);
					btn_PAUSED_execution.pressed = false;
					btn_PAUSED_execution.released = true;
					updateExecutionType(PAUSED);

				}
				else
				{
					btn_PAUSED_execution.text.setFillColor(sf::Color::Green);
				}
				if (btn_SINGLE_STEP_execution.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_SINGLE_STEP_execution.text.setFillColor(sf::Color::Green);
					btn_SINGLE_STEP_execution.pressed = false;
					btn_SINGLE_STEP_execution.released = true;
					updateExecutionType(SINGLE_STEP);

				}
				else
				{
					btn_SINGLE_STEP_execution.text.setFillColor(sf::Color::Green);
				}
				if (btn_CONTINIOUS_execution.bshape.getGlobalBounds().contains(worldPos))
				{
					btn_CONTINIOUS_execution.text.setFillColor(sf::Color::Green);
					btn_CONTINIOUS_execution.pressed = false;
					btn_CONTINIOUS_execution.released = true;
					updateExecutionType(CONTINIOUS);

				}
				else
				{
					btn_CONTINIOUS_execution.text.setFillColor(sf::Color::Green);
				}
			}
			if (event.type == sf::Event::MouseWheelScrolled)
			{
				sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
				sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
				if (event.mouseWheelScroll.delta <= -1)
				{
					zoom = std::min(2.f, zoom + .1f);
					if (MemList.getTransform().transformRect(MemList.getRectangleShape().getLocalBounds()).contains(worldPos))
					{
						MemList.moreElements();
						cout << " MORE " << endl;
					}
				}
				else if (event.mouseWheelScroll.delta >= 1)
				{
					zoom = std::max(.1f, zoom - .1f);
					if (MemList.getTransform().transformRect(MemList.getRectangleShape().getLocalBounds()).contains(worldPos))
					{
						MemList.lessElements();
						cout << " LESS " << endl;
					}
				}
				cout << " WP: " << worldPos.x << ":" << worldPos.y << endl;

				//view.setSize(window.getDefaultView().getSize());
				//view.zoom(zoom);
				window.setView(view);
			}
		}

		window.clear();
		if (exectype == CONTINIOUS)
		{
			Time = clk.getElapsedTime().asSeconds();
			if (Time > 1.0f / 60)
			{
				chip8.Chip8UpdateTimers();
				clk.restart();

			}
			for (int i = 0; i < ips; i++)
			{
				chip8.Chip8Execute(true);
				if (chip8.getDrawFlag() == 1)
				{
					ChipDraw(chip8);
					texture.update(sfpixel);

					chip8.getDrawFlag() = 0;
				}
				updateLists();
			}
		}
		else if (exectype == PAUSED)
		{

		}
		else if (exectype == SINGLE_STEP)
		{
			Time = clk.getElapsedTime().asSeconds();
			if (SingleStepExecuted == false)
			{
				if (Time > 1.0f / 60)
				{
					chip8.Chip8UpdateTimers();
					clk.restart();

				}

				chip8.Chip8Execute(true);
				if (chip8.getDrawFlag() == 1)
				{
					ChipDraw(chip8);
					texture.update(sfpixel);

					chip8.getDrawFlag() = 0;
				}
				updateLists();

				SingleStepExecuted = true;
			}
		}
		//window.draw(commandList);
		window.draw(rs_bg);
		window.draw(MemList);
		window.draw(RegistryList);
		window.draw(StackList);
		window.draw(sprite);
		window.draw(btn_step);
		window.draw(btn_back);
		window.draw(btn_more);
		window.draw(btn_less);
		window.draw(btn_reset);
		window.draw(btn_PAUSED_execution);
		window.draw(btn_SINGLE_STEP_execution);
		window.draw(btn_CONTINIOUS_execution);
		//window.draw(ipsText);
		window.draw(execText);
		window.draw(instructionText);
		window.display();
	}

	return 0;
}