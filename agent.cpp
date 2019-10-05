#include <cstdlib>
#include <cstdio>
#include <Windows.h>
#include "agent.h"

Agent::Agent(float update_rate) :_update_rate(update_rate)
{
	FILE* fp = fopen(FilePath, "rb");
	_lookUpTable = new float[DATA_NUM];
	fread(_lookUpTable, sizeof(float), DATA_NUM, fp);
	fclose(fp);
}

void Agent::Learn(int learn_time)
{
	//Board gameboard, transition_state, last_transition_state;
	Board gameboard, laststate;
	long total_steps = 0L;
	DIR dir = 0;
	for (int i = 0; i < learn_time; i++)
	{
		if (!(i % 100))printf("%.1fK games\t%.2fM steps\r", i / 1000.0f, total_steps / 1000000.0f);

		//Queue boardqueue(10);
		gameboard.Clear();
		gameboard.Initialize();
		//boardqueue.EnQueue(gameboard);
		while (!gameboard.GameOver())
		{
			//V5.0.3.0last_transition_state = transition_state;

			laststate = gameboard;
			if (gameboard.Move(dir = _chooseNextMove(gameboard)))
			{
				//transition_state = gameboard;
				gameboard.AddNum();
			}
			else puts("error!");

			//_updateWeight(last_transition_state, transition_state, gameboard.GameOver());
			//_updateWeight(boardqueue, gameboard);
			_updateWeight(laststate, gameboard);
		}
		gameboard.SaveToFile();
		total_steps += gameboard._steps;

		if (!(i % 0x2000))_writefile();
	}
}

void Agent::Play(int play_time)
{
	Board gameboard;
#ifdef TIMER
	long total_steps = 0L;
#endif
	for (int i = 1; i <= play_time; i++)
	{
		gameboard.Clear();
		gameboard.Initialize();
#ifndef TIMER
		//gameboard.Print();
#endif // !TIMER

		while (!gameboard.GameOver())
		{
			if (gameboard.Move(_chooseNextMove(gameboard)))gameboard.AddNum();
#ifdef GUI
			gameboard.Print();
			Sleep(20);
#elif !defined TIMER
		//gameboard.Print();
#endif
			
		}
#ifdef TIMER
		total_steps += gameboard._steps;
		if (!(i % 100))printf("%ld games\t%ld steps\r", i, total_steps);
#endif
		printf("%d\r", i);
		gameboard.SaveToFile();
		gameboard.ShortPrint();
	}
}

float Agent::_getStateValue(Board & board)
{
	float total_value = 0.0F;
	for (loop_control i = 0; i < TUPLE_NUM; i++)
		total_value += *(_lookUpTable + _getLocation(board, i));
	return total_value / TUPLE_NUM;
}

void Agent::_writefile()
{
	FILE* fp = fopen(FilePath, "wb+");
	fwrite(_lookUpTable, sizeof(float), DATA_NUM, fp);
	fclose(fp);
}

DIR Agent::_chooseNextMove(Board & present_state)
{
	Board next_state(present_state);

	loop_control i = 0;

	DIR dir = 1, max_dir = 1;

	float max_score = -INFINITY, score = 0.0f;
	COOR* coor = nullptr;
	for (; dir <= 4; dir++)
	{
		if (next_state.Move(dir))
		{
			coor = next_state._emptyblocklist;
			for (i = 0; i < next_state._emptyblocknum; i++)
			{//V5.0.2.0
				next_state._setValue(coor + i, 1);
				score += 0.9f*_getStateValue(next_state);
				next_state._setValue(coor + i, 2);
				score += 0.1f*_getStateValue(next_state);
				next_state._setValue(coor + i, 0);
			}
			//V5.0.1.2
			score /= next_state._emptyblocknum;
			score += next_state._score - present_state._score;
			if (score > max_score)
			{
				max_score = score;
				max_dir = dir;
			}
			score = 0.0f;
			next_state = present_state;
		}
	}
	return max_dir;
}

void Agent::_updateWeight(Board & last_state, Board & present_state)
{
	float addvalue = _update_rate *	/*5.0.1.9*/
		(present_state.GameOver() ? -present_state._score :
		(present_state._score - last_state._score + _getStateValue(present_state) - _getStateValue(last_state)));

	for (loop_control i = 0; i < TUPLE_NUM; i++)
		*(_lookUpTable + _getLocation(last_state, i)) += addvalue;
}

long Agent::_getLocation(Board & board, int8 num)
{
	long a;
	switch (num)
	{
	case 0:a = board._board[0][0] * 4096 + board._board[0][1] * 256 + board._board[0][2] * 16 + board._board[0][3]; break;
	case 1:a = board._board[1][0] * 4096 + board._board[1][1] * 256 + board._board[1][2] * 16 + board._board[1][3]; break;
	case 2:a = board._board[2][0] * 4096 + board._board[2][1] * 256 + board._board[2][2] * 16 + board._board[2][3]; break;
	case 3:a = board._board[3][0] * 4096 + board._board[3][1] * 256 + board._board[3][2] * 16 + board._board[3][3]; break;

	case 4:a = board._board[0][0] * 4096 + board._board[1][0] * 256 + board._board[2][0] * 16 + board._board[3][0]; break;
	case 5:a = board._board[0][1] * 4096 + board._board[1][1] * 256 + board._board[2][1] * 16 + board._board[3][1]; break;
	case 6:a = board._board[0][2] * 4096 + board._board[1][2] * 256 + board._board[2][2] * 16 + board._board[3][2]; break;
	case 7:a = board._board[0][3] * 4096 + board._board[1][3] * 256 + board._board[2][3] * 16 + board._board[3][3]; break;

	case 8:a = board._board[0][0] * 4096 + board._board[0][1] * 256 + board._board[1][1] * 16 + board._board[1][0]; break;
	case 9:a = board._board[0][1] * 4096 + board._board[0][2] * 256 + board._board[1][2] * 16 + board._board[1][1]; break;
	case 10:a = board._board[0][2] * 4096 + board._board[0][3] * 256 + board._board[1][3] * 16 + board._board[1][2]; break;

	case 11:a = board._board[1][0] * 4096 + board._board[1][1] * 256 + board._board[2][1] * 16 + board._board[2][0]; break;
	case 12:a = board._board[1][1] * 4096 + board._board[1][2] * 256 + board._board[2][2] * 16 + board._board[2][1]; break;
	case 13:a = board._board[1][2] * 4096 + board._board[1][3] * 256 + board._board[2][3] * 16 + board._board[2][2]; break;

	case 14:a = board._board[2][0] * 4096 + board._board[2][1] * 256 + board._board[3][1] * 16 + board._board[3][0]; break;
	case 15:a = board._board[2][1] * 4096 + board._board[2][2] * 256 + board._board[3][2] * 16 + board._board[3][1]; break;
	case 16:a = board._board[2][2] * 4096 + board._board[2][3] * 256 + board._board[3][3] * 16 + board._board[3][2]; break;

	case 17:a = board._board[0][0] * 4096 + board._board[0][1] * 256 + board._board[0][2] * 16 + board._board[1][0]; break;
	case 18:a = board._board[0][1] * 4096 + board._board[0][2] * 256 + board._board[0][3] * 16 + board._board[1][1]; break;
	case 19:a = board._board[1][0] * 4096 + board._board[1][1] * 256 + board._board[1][2] * 16 + board._board[2][0]; break;
	case 20:a = board._board[1][1] * 4096 + board._board[1][2] * 256 + board._board[1][3] * 16 + board._board[2][1]; break;
	case 21:a = board._board[2][0] * 4096 + board._board[2][1] * 256 + board._board[2][2] * 16 + board._board[3][0]; break;
	case 22:a = board._board[2][1] * 4096 + board._board[2][2] * 256 + board._board[2][3] * 16 + board._board[3][1]; break;

	case 23:a = board._board[0][0] * 4096 + board._board[0][1] * 256 + board._board[0][2] * 16 + board._board[1][2]; break;
	case 24:a = board._board[0][1] * 4096 + board._board[0][2] * 256 + board._board[0][3] * 16 + board._board[1][3]; break;
	case 25:a = board._board[1][0] * 4096 + board._board[1][1] * 256 + board._board[1][2] * 16 + board._board[2][2]; break;
	case 26:a = board._board[1][1] * 4096 + board._board[1][2] * 256 + board._board[1][3] * 16 + board._board[2][3]; break;
	case 27:a = board._board[2][0] * 4096 + board._board[2][1] * 256 + board._board[2][2] * 16 + board._board[3][2]; break;
	case 28:a = board._board[2][1] * 4096 + board._board[2][2] * 256 + board._board[2][3] * 16 + board._board[3][3]; break;

	case 29:a = board._board[1][0] * 4096 + board._board[1][1] * 256 + board._board[1][2] * 16 + board._board[0][0]; break;
	case 30:a = board._board[1][1] * 4096 + board._board[1][2] * 256 + board._board[1][3] * 16 + board._board[0][1]; break;
	case 31:a = board._board[2][0] * 4096 + board._board[2][1] * 256 + board._board[2][2] * 16 + board._board[1][0]; break;
	case 32:a = board._board[2][1] * 4096 + board._board[2][2] * 256 + board._board[2][3] * 16 + board._board[1][1]; break;
	case 33:a = board._board[3][0] * 4096 + board._board[3][1] * 256 + board._board[3][2] * 16 + board._board[2][0]; break;
	case 34:a = board._board[3][1] * 4096 + board._board[3][2] * 256 + board._board[3][3] * 16 + board._board[2][1]; break;

	case 35:a = board._board[1][0] * 4096 + board._board[1][1] * 256 + board._board[1][2] * 16 + board._board[0][2]; break;
	case 36:a = board._board[1][1] * 4096 + board._board[1][2] * 256 + board._board[1][3] * 16 + board._board[0][3]; break;
	case 37:a = board._board[2][0] * 4096 + board._board[2][1] * 256 + board._board[2][2] * 16 + board._board[1][2]; break;
	case 38:a = board._board[2][1] * 4096 + board._board[2][2] * 256 + board._board[2][3] * 16 + board._board[1][3]; break;
	case 39:a = board._board[3][0] * 4096 + board._board[3][1] * 256 + board._board[3][2] * 16 + board._board[2][2]; break;
	case 40:a = board._board[3][1] * 4096 + board._board[3][2] * 256 + board._board[3][3] * 16 + board._board[2][3]; break;

	case 41:a = board._board[0][0] * 4096 + board._board[1][0] * 256 + board._board[2][0] * 16 + board._board[0][1]; break;
	case 42:a = board._board[1][0] * 4096 + board._board[2][0] * 256 + board._board[3][0] * 16 + board._board[1][1]; break;
	case 43:a = board._board[0][1] * 4096 + board._board[1][1] * 256 + board._board[2][1] * 16 + board._board[0][2]; break;
	case 44:a = board._board[1][1] * 4096 + board._board[2][1] * 256 + board._board[3][1] * 16 + board._board[1][2]; break;
	case 45:a = board._board[0][2] * 4096 + board._board[1][2] * 256 + board._board[2][2] * 16 + board._board[0][3]; break;
	case 46:a = board._board[1][2] * 4096 + board._board[2][2] * 256 + board._board[3][2] * 16 + board._board[1][3]; break;

	case 47:a = board._board[0][0] * 4096 + board._board[1][0] * 256 + board._board[2][0] * 16 + board._board[2][1]; break;
	case 48:a = board._board[1][0] * 4096 + board._board[2][0] * 256 + board._board[3][0] * 16 + board._board[3][1]; break;
	case 49:a = board._board[0][1] * 4096 + board._board[1][1] * 256 + board._board[2][1] * 16 + board._board[2][2]; break;
	case 50:a = board._board[1][1] * 4096 + board._board[2][1] * 256 + board._board[3][1] * 16 + board._board[3][2]; break;
	case 51:a = board._board[0][2] * 4096 + board._board[1][2] * 256 + board._board[2][2] * 16 + board._board[2][3]; break;
	case 52:a = board._board[1][2] * 4096 + board._board[2][2] * 256 + board._board[3][2] * 16 + board._board[3][3]; break;

	case 53:a = board._board[0][1] * 4096 + board._board[1][1] * 256 + board._board[2][1] * 16 + board._board[0][0]; break;
	case 54:a = board._board[1][1] * 4096 + board._board[2][1] * 256 + board._board[3][1] * 16 + board._board[1][0]; break;
	case 55:a = board._board[0][2] * 4096 + board._board[1][2] * 256 + board._board[2][2] * 16 + board._board[0][1]; break;
	case 56:a = board._board[1][2] * 4096 + board._board[2][2] * 256 + board._board[3][2] * 16 + board._board[1][1]; break;
	case 57:a = board._board[0][3] * 4096 + board._board[1][3] * 256 + board._board[2][3] * 16 + board._board[0][2]; break;
	case 58:a = board._board[1][3] * 4096 + board._board[2][3] * 256 + board._board[3][3] * 16 + board._board[1][2]; break;

	case 59:a = board._board[0][1] * 4096 + board._board[1][1] * 256 + board._board[2][1] * 16 + board._board[2][0]; break;
	case 60:a = board._board[1][1] * 4096 + board._board[2][1] * 256 + board._board[3][1] * 16 + board._board[3][0]; break;
	case 61:a = board._board[0][2] * 4096 + board._board[1][2] * 256 + board._board[2][2] * 16 + board._board[2][1]; break;
	case 62:a = board._board[1][2] * 4096 + board._board[2][2] * 256 + board._board[3][2] * 16 + board._board[3][1]; break;
	case 63:a = board._board[0][3] * 4096 + board._board[1][3] * 256 + board._board[2][3] * 16 + board._board[2][2]; break;
	case 64:a = board._board[1][3] * 4096 + board._board[2][3] * 256 + board._board[3][3] * 16 + board._board[3][2]; break;

	/*case 65:a = board._board[0][0] * 4096 + board._board[1][0] * 256 + board._board[1][1] * 16 + board._board[2][1]; break;
	case 66:a = board._board[0][1] * 4096 + board._board[1][1] * 256 + board._board[1][2] * 16 + board._board[2][2]; break;
	case 67:a = board._board[0][2] * 4096 + board._board[1][2] * 256 + board._board[1][3] * 16 + board._board[2][3]; break;
	case 68:a = board._board[1][0] * 4096 + board._board[2][0] * 256 + board._board[2][1] * 16 + board._board[3][1]; break;
	case 69:a = board._board[1][1] * 4096 + board._board[2][1] * 256 + board._board[2][2] * 16 + board._board[3][2]; break;
	case 70:a = board._board[1][2] * 4096 + board._board[2][2] * 256 + board._board[2][3] * 16 + board._board[3][3]; break;

	case 71:a = board._board[2][0] * 4096 + board._board[1][0] * 256 + board._board[1][1] * 16 + board._board[0][1]; break;
	case 72:a = board._board[2][1] * 4096 + board._board[1][1] * 256 + board._board[1][2] * 16 + board._board[0][2]; break;
	case 73:a = board._board[2][2] * 4096 + board._board[1][2] * 256 + board._board[1][3] * 16 + board._board[0][3]; break;
	case 74:a = board._board[3][0] * 4096 + board._board[2][0] * 256 + board._board[2][1] * 16 + board._board[1][1]; break;
	case 75:a = board._board[3][1] * 4096 + board._board[2][1] * 256 + board._board[2][2] * 16 + board._board[1][2]; break;
	case 76:a = board._board[3][2] * 4096 + board._board[2][2] * 256 + board._board[2][3] * 16 + board._board[1][3]; break;

	case 77:a = board._board[0][0] * 4096 + board._board[0][1] * 256 + board._board[1][1] * 16 + board._board[1][2]; break;
	case 78:a = board._board[1][0] * 4096 + board._board[1][1] * 256 + board._board[2][1] * 16 + board._board[2][2]; break;
	case 79:a = board._board[2][0] * 4096 + board._board[2][1] * 256 + board._board[3][1] * 16 + board._board[3][2]; break;
	case 80:a = board._board[0][1] * 4096 + board._board[0][2] * 256 + board._board[1][2] * 16 + board._board[1][3]; break;
	case 81:a = board._board[1][1] * 4096 + board._board[1][2] * 256 + board._board[2][2] * 16 + board._board[2][3]; break;
	case 82:a = board._board[2][1] * 4096 + board._board[2][2] * 256 + board._board[3][2] * 16 + board._board[3][3]; break;

	case 83:a = board._board[0][2] * 4096 + board._board[0][1] * 256 + board._board[1][1] * 16 + board._board[1][0]; break;
	case 84:a = board._board[1][2] * 4096 + board._board[1][1] * 256 + board._board[2][1] * 16 + board._board[2][0]; break;
	case 85:a = board._board[2][2] * 4096 + board._board[2][1] * 256 + board._board[3][1] * 16 + board._board[3][0]; break;
	case 86:a = board._board[0][3] * 4096 + board._board[0][2] * 256 + board._board[1][2] * 16 + board._board[1][1]; break;
	case 87:a = board._board[1][3] * 4096 + board._board[1][2] * 256 + board._board[2][2] * 16 + board._board[2][1]; break;
	case 88:a = board._board[2][3] * 4096 + board._board[2][2] * 256 + board._board[3][2] * 16 + board._board[3][1]; break;*/

	}

	return a + num * PER_TUPLE_DATA_NUM;

}

template <typename T>
inline T Agent::_random(T max)
{
	return max * rand() / (RAND_MAX + 1.0);
}

/*************************************************************************************************/

#if 0
void Agent::_updateWeight(Board & last_transition_state, Board & transition_state, bool gameover)
{
	float addvalue = _update_rate *
		(gameover ? _getStateValue(transition_state) - 2 * _getStateValue(last_transition_state) :
			//5.0.3.0,present_state->transition_state
		(transition_state._score - last_transition_state._score +
			_getStateValue(transition_state) - _getStateValue(last_transition_state)));

	for (loop_control i = 0; i < TUPLE_NUM; i++)
		*(_lookUpTable + _getLocation(last_transition_state, i)) += addvalue;
}
void Agent::_updateWeight(Queue& queue, Board & present_state)
{
	//*queue._queue_rear==last_state
	float addvalue = 0.0f, TDerror = present_state.GameOver() ?
		_getStateValue(present_state) - 2 * _getStateValue(*queue._queue_rear) :
		(present_state._score - queue._queue_rear->_score +
			_getStateValue(present_state) - _getStateValue(*queue._queue_rear));

	QueueNode* ptr = queue._queue_rear;
	for (loop_control i = 0; i < queue._present_queue_length; i++, ptr = ptr->_front)
	{
		addvalue = _update_rate * queue._eligibility[i] * TDerror;
		for (loop_control j = 0; j < TUPLE_NUM; j++)
			*(_lookUpTable + _getLocation(*ptr, j)) += addvalue;
	}

	queue.EnQueue(present_state);
}
void Agent::_updateWeight(Board & last_state, DIR dir)//V5.0.2
{
	float addvalue = 0.0f;
	//所有局面奖励的均值+立即奖励值

	Board transition_state(last_state);
	transition_state.Move(dir);
	COOR* coor = transition_state._emptyblocklist;

	for (loop_control i = 0; i < transition_state._emptyblocknum; i++)
	{//V5.0.2.1
		transition_state._setValue(coor + i, 1);
		transition_state._emptyblocknum--;
		addvalue += 0.9f*(transition_state.GameOver() ? _getStateValue(transition_state) - 2 * _getStateValue(last_state) :
			(_getStateValue(transition_state) - _getStateValue(last_state)));
		transition_state._setValue(coor + i, 2);
		addvalue += 0.1f*(transition_state.GameOver() ? _getStateValue(transition_state) - 2 * _getStateValue(last_state) :
			(_getStateValue(transition_state) - _getStateValue(last_state)));
		transition_state._setValue(coor + i, 0);
		transition_state._emptyblocknum++;
	}
	//V5.0.1.2
	addvalue /= transition_state._emptyblocknum;
	addvalue += transition_state._score - last_state._score;
	addvalue *= _update_rate;

	for (loop_control i = 0; i < TUPLE_NUM; i++)
		*(_lookUpTable + _getLocation(last_state, i)) += addvalue;
}
void Agent::Learn(int learn_time)
{
	Board gameboard, laststate;
	long total_steps = 0L;
	DIR dir = 0;
	for (int i = 0; i < learn_time; i++)
	{
		if (!(i % 100))printf("%.1fK games\t%.2fM steps\r", i / 1000.0f, total_steps / 1000000.0f);
		gameboard.Clear();
		gameboard.Initialize();
		while (!gameboard.GameOver())
		{
			laststate = gameboard;
			if (gameboard.Move(dir = _chooseNextMove(gameboard)))gameboard.AddNum();
			else puts("error!");
			_updateWeight(laststate, gameboard);
			//_updateWeight(laststate, dir);
		}
		gameboard.SaveToFile();
		total_steps += gameboard._steps;

		if (!(i % 1000))_writefile();
	}
}

DIR Agent::_chooseNextMove(Board & present_state, int8 tree_depth, int16 simulation_time)
{
	//寻找所有终端节点的最大值，并返回该节点的根方向。
	loop_control i = 0, j = 0;

	float evaltotalscore[4] = { 0.0f }, maxevalscore = -INFINITY;
	int16 evaltimes[4] = { 0 };

	TreeNode treeroot(present_state);
	for (int16 k = 0; k < simulation_time; k++)
	{
		Tree tree(tree_depth, &treeroot);
		for (i = 0; i < 4; i++)
			if (treeroot._child[i] != nullptr)
			{
				evaltotalscore[i] += _findmax(tree_depth, treeroot._child[i]);
				evaltimes[i]++;
			}
	}
	for (i = 0; i < 4; i++)evaltotalscore[i] = evaltotalscore[i] / evaltimes[i] ? evaltimes[i] : -INFINITY;

	for (i = 1, j = 0, maxevalscore = evaltotalscore[0]; i < 4; i++)
		if (evaltotalscore[i] > maxevalscore)
		{
			maxevalscore = evaltotalscore[i];
			j = i;
		}

	return j + 1;
}

float Agent::_findmax(int8 tree_depth, TreeNode * root)
{
	loop_control i = 0;
	float result = 0.0F;
	float statevalue = 0.0F;
	if (root->_depth == tree_depth - 1)
	{
		for (i = 0; i < 4; i++)
			if (root->_child[i] != nullptr)
				if (result < (statevalue = _getStateValue(*root->_child[i])))result = statevalue;
		return result;
	}
	else
	{
		for (i = 0; i < 4; i++)
			if (root->_child[i] != nullptr)
				if (result < (statevalue = _findmax(tree_depth, root->_child[i])))result = statevalue;
		return result;
	}
}
#if TUPLE_DIMENSION==2
long Agent::_getLocation(Board & board, int num)
{
	long a;
	switch (num)
	{
	case 0:a = board._board[0][0] * 16 + board._board[0][1]; break;
	case 1:a = board._board[0][1] * 16 + board._board[0][2]; break;
	case 2:a = board._board[0][2] * 16 + board._board[0][3]; break;
	case 3:a = board._board[1][0] * 16 + board._board[1][1]; break;
	case 4:a = board._board[1][1] * 16 + board._board[1][2]; break;
	case 5:a = board._board[1][2] * 16 + board._board[1][3]; break;
	case 6:a = board._board[2][0] * 16 + board._board[2][1]; break;
	case 7:a = board._board[2][1] * 16 + board._board[2][2]; break;
	case 8:a = board._board[2][2] * 16 + board._board[2][3]; break;
	case 9:a = board._board[3][0] * 16 + board._board[3][1]; break;
	case 10:a = board._board[3][1] * 16 + board._board[3][2]; break;
	case 11:a = board._board[3][2] * 16 + board._board[3][3]; break;

	case 12:a = board._board[0][0] * 16 + board._board[1][0]; break;
	case 13:a = board._board[1][0] * 16 + board._board[2][0]; break;
	case 14:a = board._board[2][0] * 16 + board._board[3][0]; break;
	case 15:a = board._board[0][1] * 16 + board._board[1][1]; break;
	case 16:a = board._board[1][1] * 16 + board._board[2][1]; break;
	case 17:a = board._board[2][1] * 16 + board._board[3][1]; break;
	case 18:a = board._board[0][2] * 16 + board._board[1][2]; break;
	case 19:a = board._board[1][2] * 16 + board._board[2][2]; break;
	case 20:a = board._board[2][2] * 16 + board._board[3][2]; break;
	case 21:a = board._board[0][3] * 16 + board._board[1][3]; break;
	case 22:a = board._board[1][3] * 16 + board._board[2][3]; break;
	case 23:a = board._board[2][3] * 16 + board._board[3][3]; break;
	}

	return a + num * PER_TUPLE_DATA_NUM;

}
#elif TUPLE_DIMENSION==3
long Agent::_getLocation(Board & board, int num)
{
	long a;
	switch (num)
	{
	case 0:a = board._board[0][0] * 256 + board._board[0][1] * 16 + board._board[0][2]; break;
	case 1:a = board._board[0][1] * 256 + board._board[0][2] * 16 + board._board[0][3]; break;

	case 2:a = board._board[1][0] * 256 + board._board[1][1] * 16 + board._board[1][2]; break;
	case 3:a = board._board[1][1] * 256 + board._board[1][2] * 16 + board._board[1][3]; break;

	case 4:a = board._board[2][0] * 256 + board._board[2][1] * 16 + board._board[2][2]; break;
	case 5:a = board._board[2][1] * 256 + board._board[2][2] * 16 + board._board[2][3]; break;

	case 6:a = board._board[3][0] * 256 + board._board[3][1] * 16 + board._board[3][2]; break;
	case 7:a = board._board[3][1] * 256 + board._board[3][2] * 16 + board._board[3][3]; break;

	case 8:a = board._board[0][0] * 256 + board._board[0][1] * 16 + board._board[0][2]; break;
	case 9:a = board._board[0][1] * 256 + board._board[0][2] * 16 + board._board[0][3]; break;

	case 10:a = board._board[1][0] * 256 + board._board[1][1] * 16 + board._board[1][2]; break;
	case 11:a = board._board[1][1] * 256 + board._board[1][2] * 16 + board._board[1][3]; break;

	case 12:a = board._board[2][0] * 256 + board._board[2][1] * 16 + board._board[2][2]; break;
	case 13:a = board._board[2][1] * 256 + board._board[2][2] * 16 + board._board[2][3]; break;

	case 14:a = board._board[3][0] * 256 + board._board[3][1] * 16 + board._board[3][2]; break;
	case 15:a = board._board[3][1] * 256 + board._board[3][2] * 16 + board._board[3][3]; break;

	case 16:a = board._board[0][0] * 256 + board._board[0][1] * 16 + board._board[1][0]; break;
	case 17:a = board._board[0][1] * 256 + board._board[0][2] * 16 + board._board[1][1]; break;
	case 18:a = board._board[0][2] * 256 + board._board[0][3] * 16 + board._board[1][2]; break;

	case 19:a = board._board[1][0] * 256 + board._board[1][1] * 16 + board._board[2][0]; break;
	case 20:a = board._board[1][1] * 256 + board._board[1][2] * 16 + board._board[2][1]; break;
	case 21:a = board._board[1][2] * 256 + board._board[1][3] * 16 + board._board[2][2]; break;

	case 22:a = board._board[2][0] * 256 + board._board[2][1] * 16 + board._board[3][0]; break;
	case 23:a = board._board[2][1] * 256 + board._board[2][2] * 16 + board._board[3][1]; break;
	case 24:a = board._board[2][2] * 256 + board._board[2][3] * 16 + board._board[3][2]; break;

	case 25:a = board._board[1][1] * 256 + board._board[1][0] * 16 + board._board[0][1]; break;
	case 26:a = board._board[1][2] * 256 + board._board[1][1] * 16 + board._board[0][2]; break;
	case 27:a = board._board[1][3] * 256 + board._board[1][2] * 16 + board._board[0][3]; break;

	case 28:a = board._board[2][1] * 256 + board._board[2][0] * 16 + board._board[1][1]; break;
	case 29:a = board._board[2][2] * 256 + board._board[2][1] * 16 + board._board[1][2]; break;
	case 30:a = board._board[2][3] * 256 + board._board[2][2] * 16 + board._board[1][3]; break;

	case 31:a = board._board[3][1] * 256 + board._board[3][0] * 16 + board._board[2][1]; break;
	case 32:a = board._board[3][2] * 256 + board._board[3][1] * 16 + board._board[2][2]; break;
	case 33:a = board._board[3][3] * 256 + board._board[3][2] * 16 + board._board[2][3]; break;
	}

	return a + num * PER_TUPLE_DATA_NUM;

}
#endif
#endif
/*************************************************************************************************/
