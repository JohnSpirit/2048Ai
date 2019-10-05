#pragma once
#include <cstdio>
#include "board.h"
//#include "tree.h"
//#include "queue.h"
constexpr int TUPLE_DIMENSION = 4;
constexpr int TUPLE_NUM = 65;
constexpr int PER_TUPLE_DATA_NUM = 1 << (4 * TUPLE_DIMENSION);
constexpr int DATA_NUM = TUPLE_NUM * PER_TUPLE_DATA_NUM;
constexpr float DISCOUNT_RATE = 1.0f;
class Agent
{
public:
	Agent(float update_rate);
	~Agent(){delete _lookUpTable;}

	void Learn(int learn_time);
	void Play(int play_time);

	static char FilePath[128];

private:
	DIR _chooseNextMove(Board& present_state);//选择当前状态最佳动作

	void _updateWeight(Board& last_state, Board& present_state);//更新前一状态权值

	void _updateWeight(Board & last_transition_state, Board & transition_state, bool gameover);

	float _getStateValue(Board& board);//获取某局面状态值函数值
	long _getLocation(Board& board, int8 num);
	//void _updateWeight(Queue & queue, Board & present_state);
	//获取某局面状态值在文件中的位置
	void _writefile();//写入文件
	template <typename T>T _random(T max);
	/***************对Tree的操作***************/
	//float _findmax(int8 tree_depth, TreeNode* root);
	DIR _chooseNextMove(Board& present_state, int8 tree_depth, int16 simulation_time);


	/***************对Queue的操作**************/
	//void _updateWeight(Queue& queue, Board & present_state);





	float _update_rate = 0.0f;
	float* _lookUpTable = nullptr;
};