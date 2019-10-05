#pragma once
#define LEFT 1
#define RIGHT 4
#define UP 2
#define DOWN 3
#define REV(x) (5-x)
#include <cstdlib>
using std::rand;
template<typename T>
constexpr T random(T x) { return ((T)(x*rand()/(0x7fff+1.0F))); }
//constexpr int SR_VER(int main, int sub, int mod, int submod) { return main * 1000000 + sub * 10000 + mod * 100 + submod; }
typedef signed char int8;
typedef short int16;
typedef signed char loop_control;
typedef signed char DIR;
typedef struct
{
	int8 X;//行标
	int8 Y;//列标
}COOR;
typedef struct
{
	int8 value;
	COOR coor;
}COOR_DATA;