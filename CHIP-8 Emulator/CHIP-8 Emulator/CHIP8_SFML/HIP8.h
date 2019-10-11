#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

#define LSHIFT2(param) ((param & 0x00F0) >> 4)
#define LSHIFT3(param) ((param & 0x0F00) >> 8)

#define V0 data_registers8[0]
#define V1 data_registers8[1]
#define V2 data_registers8[2]
#define V3 data_registers8[3]
#define V4 data_registers8[4]
#define V5 data_registers8[5]
#define V6 data_registers8[6]
#define V7 data_registers8[7]
#define V8 data_registers8[8]
#define V9 data_registers8[9]
#define VA data_registers8[10]
#define VB data_registers8[11]
#define VC data_registers8[12]
#define VD data_registers8[13]
#define VE data_registers8[14]
#define VF data_registers8[15]

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define STACK_SIZE 16
#define MEMORY8_SIZE 4096
#define DATA_REGISTRY_SIZE 16
#define FONT_SIZE 80
class CHIP8
{
public:
	CHIP8();
	~CHIP8();
	void Reset();

private:
	unsigned char memory8[4096];
	unsigned char data_registers8[DATA_REGISTRY_SIZE];
	unsigned char key[16];
	unsigned char display8[DISPLAY_HEIGHT][DISPLAY_WIDTH];
	unsigned short opcode_8;
	unsigned short index_register8;
	unsigned short pc_register8;

	unsigned short stack8[STACK_SIZE];
	unsigned short stack_pointer8;

	unsigned short PROGRAM_ADDRESS = 0x200;

	unsigned char delay_timer;
	unsigned char sound_timer;

	unsigned char font_8[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	bool drawFlag = 0;

public:
	bool& getDrawFlag();
	unsigned char & Key();
	unsigned char getDelay_timer;
	unsigned char getDisplay8(unsigned char column, unsigned char row);
	unsigned char getKey();
	unsigned char getSound_timer;
	unsigned char* getData_registers8();
	unsigned char* getDisplay8();
	unsigned char* getFont_8();
	unsigned char* getMemory8();
	unsigned short& getIndex_register8();
	unsigned short& getOpcode_8();
	unsigned short& getPC_register8();
	unsigned short& getPROGRAM_ADDRESS();
	unsigned short& getStack_pointer8();
	unsigned short* getStack8();
	void Chip8UpdateTimers();
	void Chip8ClearScreen();
	void Chip8Execute(bool start);
	void loadRom8(int addr, int size, unsigned char & data);
	void setDisplay8(unsigned char column, unsigned char row, unsigned char val);
	void setKey(unsigned char val, unsigned int index);
};

/*
NNN: address
NN: 8-bit constant
N: 4-bit constant
X and Y: 4-bit register identifier
PC : Program Counter
I : 16bit register (For memory address) (Similar to void pointer)

0NNN 	Call 		Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
00E0 	Display 	disp_clear() 	Clears the screen.
00EE 	Flow 	return; 	Returns from a subroutine.
1NNN 	Flow 	goto NNN; 	Jumps to address NNN.
2NNN 	Flow 	*(0xNNN)() 	Calls subroutine at NNN.
3XNN 	Cond 	if(Vx==NN) 	Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
4XNN 	Cond 	if(Vx!=NN) 	Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
5XY0 	Cond 	if(Vx==Vy) 	Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
6XNN 	Const 	Vx = NN 	Sets VX to NN.
7XNN 	Const 	Vx += NN 	Adds NN to VX. (Carry flag is not changed)
8XY0 	Assign 	Vx=Vy 	Sets VX to the value of VY.
8XY1 	BitOp 	Vx=Vx|Vy 	Sets VX to VX or VY. (Bitwise OR operation)
8XY2 	BitOp 	Vx=Vx&Vy 	Sets VX to VX and VY. (Bitwise AND operation)
8XY3 	BitOp 	Vx=Vx^Vy 	Sets VX to VX xor VY.
8XY4 	Math 	Vx += Vy 	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
8XY5 	Math 	Vx -= Vy 	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
8XY6 	BitOp 	Vx>>=1 	Stores the least significant bit of VX in VF and then shifts VX to the right by 1.[2]
8XY7 	Math 	Vx=Vy-Vx 	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
8XYE 	BitOp 	Vx<<=1 	Stores the most significant bit of VX in VF and then shifts VX to the left by 1.[3]
9XY0 	Cond 	if(Vx!=Vy) 	Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
ANNN 	MEM 	I = NNN 	Sets I to the address NNN.
BNNN 	Flow 	PC=V0+NNN 	Jumps to the address NNN plus V0.
CXNN 	Rand 	Vx=rand()&NN 	Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
DXYN 	Disp 	draw(Vx,Vy,N) 	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
EX9E 	KeyOp 	if(key()==Vx) 	Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
EXA1 	KeyOp 	if(key()!=Vx) 	Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
FX07 	Timer 	Vx = get_delay() 	Sets VX to the value of the delay timer.
FX0A 	KeyOp 	Vx = get_key() 	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
FX15 	Timer 	delay_timer(Vx) 	Sets the delay timer to VX.
FX18 	Sound 	sound_timer(Vx) 	Sets the sound timer to VX.
FX1E 	MEM 	I +=Vx 	Adds VX to I.[4]
FX29 	MEM 	I=sprite_addr[Vx] 	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
FX33 	BCD 	set_BCD(Vx);

*(I+0)=BCD(3);

*(I+1)=BCD(2);

*(I+2)=BCD(1);
Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
FX55 	MEM 	reg_dump(Vx,&I) 	Stores V0 to VX (including VX) in memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
FX65 	MEM 	reg_load(Vx,&I) 	Fills V0 to VX (including VX) with values from memory starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.
*/

/*
เภข$๐3๒e๐)` c ะ5๑)`ะ5๒)`
ะ5๐
20E0B980E0B8A0EFA388E0B88224E0B99033E0B99265E0B9902960206320E0B8B035E0B991296005E0B8B035E0B99229600AE0B8B035E0B9900A1220
*/