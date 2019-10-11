#include "HIP8.h"

CHIP8::CHIP8()
{
	pc_register8 = PROGRAM_ADDRESS;
	opcode_8 = 0;
	index_register8 = 0;
	stack_pointer8 = 0;

	memset(stack8, 0x00, STACK_SIZE);
	memset(memory8, 0x00, MEMORY8_SIZE);
	memset(data_registers8, 0x00, DATA_REGISTRY_SIZE);
	Chip8ClearScreen();
	delay_timer = 0;
	sound_timer = 0;
	for (int n = 0; n < FONT_SIZE; n++)
	{
		memory8[n] = font_8[n];
	}
}

CHIP8::~CHIP8()
{
}

void CHIP8::Reset()
{
	pc_register8 = PROGRAM_ADDRESS;
	opcode_8 = 0;
	index_register8 = 0;
	stack_pointer8 = 0;

	memset(stack8, 0x00, STACK_SIZE);
	memset(memory8, 0x00, MEMORY8_SIZE);
	memset(data_registers8, 0x00, DATA_REGISTRY_SIZE);
	memset(key, 0x00, FONT_SIZE);
	Chip8ClearScreen();
	delay_timer = 0;
	sound_timer = 0;
	for (int n = 0; n < FONT_SIZE; n++)
	{
		memory8[n] = font_8[n];
	}
}

unsigned char* CHIP8::getMemory8()
{
	return &memory8[0];
}

unsigned short * CHIP8::getStack8()
{
	return &stack8[0];
}

void CHIP8::Chip8UpdateTimers()
{
	if (delay_timer > 0)
	{
		delay_timer--;
	}
	if (sound_timer > 0)
	{
		sound_timer--;
	}
}

unsigned short & CHIP8::getStack_pointer8()
{
	// TODO: insert return statement here
	return stack_pointer8;
}

unsigned char * CHIP8::getFont_8()
{
	return nullptr;
}

unsigned char * CHIP8::getDisplay8()
{
	return &display8[0][0];
}

unsigned char CHIP8::getDisplay8(unsigned char row, unsigned char column)
{
	return display8[row][column];
}

void CHIP8::setDisplay8(unsigned char row, unsigned char column, unsigned char val)
{
	display8[row][column] = val;
}

bool & CHIP8::getDrawFlag()
{
	return drawFlag;
}

unsigned char CHIP8::getKey()
{
	return 0;
}

void CHIP8::setKey(unsigned char val, unsigned int index)
{
	key[index] = val;
}

unsigned char& CHIP8::Key()
{
	return key[0];
}

unsigned char* CHIP8::getData_registers8()
{
	return data_registers8;
}

unsigned short & CHIP8::getPROGRAM_ADDRESS()
{
	// TODO: insert return statement here
	return PROGRAM_ADDRESS;
}

unsigned short & CHIP8::getOpcode_8()
{
	return opcode_8;
}

unsigned short & CHIP8::getIndex_register8()
{
	return index_register8;
}

unsigned short & CHIP8::getPC_register8()
{
	return pc_register8;
}

void CHIP8::loadRom8(int addr, int size, unsigned char &data)
{
	memcpy(&memory8[addr], &data, sizeof(unsigned char) * size);
}

void CHIP8::Chip8ClearScreen()
{
	for (int x = 0; x < DISPLAY_HEIGHT; x++)
	{
		for (int y = 0; y < DISPLAY_WIDTH; y++)
		{
			display8[x][y] = 0;
		}
	}
}

void CHIP8::Chip8Execute(bool start)
{
 	opcode_8 = memory8[pc_register8] << 8 | memory8[pc_register8 + 1];
	//std::cout << "OPCODE: " << std::hex << opcode_8  << " PC: " << std::hex << pc_register8 << " I: " << std::hex << index_register8 << " SP: " << std::hex << stack_pointer8 << std::endl;
	switch (opcode_8 & 0xF000)
	{
	case 0x0000:		//0NNN Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
		switch (opcode_8 & 0x000F)
		{
		case 0x0000:
			Chip8ClearScreen();
			drawFlag = true;
			pc_register8 += 2;
			break;
		case 0x000E:		//Returns from a subroutine.
			--stack_pointer8;
			pc_register8 = stack8[stack_pointer8];
			pc_register8 += 2;
			break;
		}
		break;
	case 0x1000:		//Jumps to address NNN.
		pc_register8 = (opcode_8 & 0x0FFF);
		break;
	case 0x2000:		//Calls subroutine at NNN.
		stack8[stack_pointer8] = pc_register8;
		++stack_pointer8;
		pc_register8 = (opcode_8 & 0x0FFF);
		break;
	case 0x3000:		//Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
		if (data_registers8[LSHIFT3(opcode_8)] == (opcode_8 & 0x00FF))
		{
			pc_register8 += 4;
		}
		else
		{
			pc_register8 += 2;
		}
		break;
	case 0x4000:		//Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
		if (data_registers8[LSHIFT3(opcode_8)] != (opcode_8 & 0x00FF))
		{
			pc_register8 += 4;
		}
		else
		{
			pc_register8 += 2;
		}
		break;
	case 0x5000:		//Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
		if (data_registers8[LSHIFT3(opcode_8)] == data_registers8[LSHIFT2(opcode_8)])
		{
			pc_register8 += 4;
		}
		else
		{
			pc_register8 += 2;
		}
		break;
	case 0x6000:		//Sets VX to NN.
		data_registers8[LSHIFT3(opcode_8)] = (opcode_8 & 0x00FF);
		pc_register8 += 2;
		break;
	case 0x7000:		//Adds NN to VX. (Carry flag is not changed)
		data_registers8[LSHIFT3(opcode_8)] += (opcode_8 & 0x00FF);
		pc_register8 += 2;
		break;
	case 0x8000:
		switch (opcode_8 & 0x000F)
		{
		case 0x0000:	//Sets VX to the value of VY.
			data_registers8[LSHIFT3(opcode_8)] = data_registers8[LSHIFT2(opcode_8)];
			pc_register8 += 2;
			break;
		case 0x0001:		//Sets VX to VX or VY. (Bitwise OR operation)
			data_registers8[LSHIFT3(opcode_8)] |= data_registers8[LSHIFT2(opcode_8)];
			pc_register8 += 2;
			break;
		case 0x0002:		//Sets VX to VX and VY. (Bitwise AND operation)
			data_registers8[LSHIFT3(opcode_8)] &= data_registers8[LSHIFT2(opcode_8)];
			pc_register8 += 2;
			break;
		case 0x0003:		//Sets VX to VX xor VY.
			data_registers8[LSHIFT3(opcode_8)] ^= data_registers8[LSHIFT2(opcode_8)];
			pc_register8 += 2;
			break;
		case 0x0004:		//Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
			data_registers8[LSHIFT3(opcode_8)] += data_registers8[LSHIFT2(opcode_8)];
			if (data_registers8[LSHIFT2(opcode_8)] > (0xFF - data_registers8[LSHIFT3(opcode_8)]))
			{
				VF = 1;
			}
			else
			{
				VF = 0;
			}
			pc_register8 += 2;
			break;
		case 0x0005:		//VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
			if (data_registers8[LSHIFT2(opcode_8)] < data_registers8[LSHIFT3(opcode_8)])
			{
				VF = 1;
			}
			else
			{
				VF = 0;
			}
			data_registers8[LSHIFT3(opcode_8)] -= data_registers8[LSHIFT2(opcode_8)];
			pc_register8 += 2;
			break;
		case 0x0006:		//Stores the least significant bit of VX in VF and then shifts VX to the right by 1.[2]
			VF = data_registers8[LSHIFT3(opcode_8)] & 0x1;
			data_registers8[LSHIFT3(opcode_8)] = data_registers8[LSHIFT3(opcode_8)] >> 1;
			pc_register8 += 2;
			break;
		case 0x0007:		//Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
			if (data_registers8[LSHIFT3(opcode_8)] > data_registers8[LSHIFT2(opcode_8)])
			{
				VF = 0;
			}
			else
			{
				VF = 1;
			}
			data_registers8[LSHIFT3(opcode_8)] = data_registers8[LSHIFT2(opcode_8)] - data_registers8[LSHIFT3(opcode_8)];
			pc_register8 += 2;
			break;
		case 0x000E:		//Stores the most significant bit of VX in VF and then shifts VX to the left by 1.[3]
			VF = data_registers8[LSHIFT3(opcode_8)] >> 7;
			data_registers8[LSHIFT3(opcode_8)] = data_registers8[LSHIFT3(opcode_8)] << 1;
			pc_register8 += 2;
			break;
		default:
			std::cout << "UNKNOWN OPCODE: " << std::hex << opcode_8 << std::endl;
			break;
		}
		break;

	case 0x9000:		//Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
		if (data_registers8[LSHIFT3(opcode_8)] != data_registers8[LSHIFT2(opcode_8)])
		{
			pc_register8 += 4;
		}
		else
		{
			pc_register8 += 2;
		}
		break;
	case 0xA000:		//Sets I to the address NNN.
		index_register8 = (opcode_8 & 0x0FFF);
		pc_register8 += 2;
		break;
	case 0xB000:		//Jumps to the address NNN plus V0.
		pc_register8 = (opcode_8 & 0x0FFF) + data_registers8[V0];
		break;
	case 0xC000:		//Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
		data_registers8[LSHIFT3(opcode_8)] = (rand() % 0xFF) & (opcode_8 & 0x00FF);
		pc_register8 += 2;
		break;
	case 0xD000:		//Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
	{
		unsigned short dVX = data_registers8[LSHIFT3(opcode_8)];
		unsigned short dVY = data_registers8[LSHIFT2(opcode_8)];
		unsigned short height = opcode_8 & 0x000F;
		unsigned short row;
		VF = 0;
		for (int y = 0; y < height; y++)
		{
			row = memory8[(index_register8 + y)];
			for (int x = 0; x < 8; x++)
			{
				if ((row & (0x80 >> x)) != 0)
				{
					if (display8[dVY + y][dVX + x] == 1)
					{
						VF = 1;
					}
					display8[dVY + y][dVX + x] ^= 1;
				}
			}
		}
		drawFlag = true;
		pc_register8 += 2;
	}
	break;
	case 0xE000:
		switch (opcode_8 & 0x00FF)
		{
		case 0x009E:		//Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
			if (key[data_registers8[LSHIFT3(opcode_8)]] != 0)
			{
				pc_register8 += 4;
			}
			else
			{
				pc_register8 += 2;
			}
			break;
		case 0x00A1:		//Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
			if (key[data_registers8[LSHIFT3(opcode_8)]] == 0)
			{
				pc_register8 += 4;
			}
			else
			{
				pc_register8 += 2;
			}
			break;
		default:
			std::cout << "WRONG OPCODE: " << std::hex << opcode_8 << std::endl;
			break;
		}
		break;
	case 0xF000:
		switch (opcode_8 & 0x00FF)
		{
		case 0x0007:		//Sets VX to the value of the delay timer.
			data_registers8[LSHIFT3(opcode_8)] = delay_timer;
			pc_register8 += 2;
			break;
		case 0x000A:		//A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
		{
			bool keyPress = false;

			for (int i = 0; i < 16; ++i)
			{
				if (key[i] != 0)
				{
					data_registers8[LSHIFT3(opcode_8)] = i;
					keyPress = true;
				}
			}

			if (!keyPress)
				return;

			pc_register8 += 2;
		}
		break;
		case 0x0015:		//Sets the delay timer to VX.
			delay_timer = data_registers8[LSHIFT3(opcode_8)];
			pc_register8 += 2;
			break;
		case 0x0018:		//Sets the sound timer to VX.
			sound_timer = data_registers8[LSHIFT3(opcode_8)];
			pc_register8 += 2;
			break;
		case 0x001E:		//Adds VX to I.[4]
			index_register8 += data_registers8[LSHIFT3(opcode_8)];
			pc_register8 += 2;
			break;
		case 0x0029:		//Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
			index_register8 = data_registers8[LSHIFT3(opcode_8)] * 0x5;
			pc_register8 += 2;
			break;
		case 0x0033:		//Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
			memory8[index_register8] = (data_registers8[(LSHIFT3(opcode_8))] / 100);
			memory8[index_register8 + 1] = ((data_registers8[(LSHIFT3(opcode_8))] / 10) % 10);
			memory8[index_register8 + 2] = ((data_registers8[(LSHIFT3(opcode_8))] % 100) % 10);
			pc_register8 += 2;
			break;
		case 0x0055:		//Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
			for (unsigned int n = 0; n <= LSHIFT3(opcode_8); ++n)
			{
				memory8[index_register8 + n] = data_registers8[n];
			}
			pc_register8 += 2;
			break;
		case 0x0065:		//Fills V0 to VX (including VX) with values from memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
			for (unsigned int n = 0; n <= LSHIFT3(opcode_8); ++n)
			{
				data_registers8[n] = memory8[index_register8 + n];
			}
			pc_register8 += 2;
			break;
		default:
			std::cout << "WRONG OPCODE: " << std::hex << opcode_8 << std::endl;
			break;
		}
		break;

	default:
		std::cout << "WRONG OPCODE: " << std::hex << opcode_8 << std::endl;
		start = false;
		break;
	}
}