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
	DIR _chooseNextMove(Board& present_state);//ѡ��ǰ״̬��Ѷ���

	void _updateWeight(Board& last_state, Board& present_state);//����ǰһ״̬Ȩֵ

	void _updateWeight(Board & last_transition_state, Board & transition_state, bool gameover);

	float _getStateValue(Board& board);//��ȡĳ����״ֵ̬����ֵ
	long _getLocation(Board& board, int8 num);
	//void _updateWeight(Queue & queue, Board & present_state);
	//��ȡĳ����״ֵ̬���ļ��е�λ��
	void _writefile();//д���ļ�
	template <typename T>T _random(T max);
	/***************��Tree�Ĳ���***************/
	//float _findmax(int8 tree_depth, TreeNode* root);
	DIR _chooseNextMove(Board& present_state, int8 tree_depth, int16 simulation_time);


	/***************��Queue�Ĳ���**************/
	//void _updateWeight(Queue& queue, Board & present_state);





	float _update_rate = 0.0f;
	float* _lookUpTable = nullptr;
};