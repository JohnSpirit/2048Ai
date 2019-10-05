#pragma once
#include "MDUTD.h"
class Board
{
	friend class Agent;
	friend class QueueNode;
public:
	Board();//生成空棋盘
	Board(const Board& origin);//深拷贝构造函数
	Board(int8(*source)[4]);//根据数组生成棋盘
	~Board();//析构函数
	void operator=(Board& origin);

	void Clear();//清空棋盘，初始化全部数据
	void Initialize();//开局添加两个数
	bool GameOver();//判断游戏是否结束
	bool Move(int8 dir = 0);//移动棋盘
	void AddNum();//添加新数
	
	void Print();//打印棋盘
	void ShortPrint();//打印运行结果
	void SaveToFile();//保存至文件

	int8 GetValue(const COOR* const coor);//获得对数值
	int  GetRealValue(int8 value);//获得实际数值
	int  GetScore();//获取分数
	short GetSteps();//获取步数
	int8(*GetBoard())[4];//获取数组行指针

	static char FilePath[128];
#ifdef GUI
	static int Size;
#endif // GUI

protected://所有成员就地初始化（C++11）
	int8 _board[4][4] = { 0 };//棋盘
	int8 _emptyblocknum = 16;//空格子个数
	COOR* _emptyblocklist = nullptr;//空格子坐标链头指针
	int  _score = 0;//分数
	int8 _max = 0;//最大数，在移动函数中有合成情况下判断修改。
	short _steps = 0;//步数在Move中有效操作情况下修改。

	//具体移动被封装。
	bool _left();
	bool _right();
	bool _up();
	bool _down();
	void _setValue(COOR* const coor, int8 value);//添加新数用，其他情况禁用
};
/*
	在C++类的实现过程中，如果想要将成员函数设置成inline内联函数的话，
	需要在类的头文件.h中定义这个函数，不能在相应的.cpp文件中定义。
	C++编译器只能将源代码嵌入，而没办法从外部库中将机器代码嵌入。
*/
inline void Board::_setValue(COOR* const coor, int8 value)
{
	_board[coor->X][coor->Y] = value;
}
inline int8 Board::GetValue(const COOR* const coor)
{
	return _board[coor->X][coor->Y];
}
inline int  Board::GetRealValue(int8 value)
{
	return value ? 1 << value : 0;
}
inline int  Board::GetScore()
{
	return _score;
}
inline short Board::GetSteps()
{
	return _steps;
}
inline int8(*Board::GetBoard())[4]
{
	return _board;
}
