#define VER 5000109
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <easyx.h>
#include "agent.h"

//#define CLEAR

char Agent::FilePath[128] = "D:\\program files\\2048\\SR5\\SuperRun5\\data.dat";
char Board::FilePath[128] = "D:\\program files\\2048\\SR5\\rundata\\rundata.xls";

#ifdef GUI
int Board::Size = 30;
void InitGUI()
{
	setbkcolor(RGB(245, 245, 245));
	cleardevice();
	setbkmode(TRANSPARENT);
	setfillcolor(RGB(200, 200, 200));
	solidrectangle(1 * Board::Size, 1 * Board::Size, 8 * Board::Size, 5 * Board::Size);
	solidrectangle(10 * Board::Size, 1 * Board::Size, 17 * Board::Size, 5 * Board::Size);
	solidrectangle(1 * Board::Size, 7 * Board::Size, 17 * Board::Size, 23 * Board::Size);
	RECT pRect1 = { 1 * Board::Size, 1 * Board::Size, 8 * Board::Size, 3 * Board::Size };
	RECT pRect2 = { 10 * Board::Size, 1 * Board::Size, 17 * Board::Size, 3 * Board::Size };
	RECT pRect3 = { 0, 23.5 * Board::Size, 18 * Board::Size, 25 * Board::Size };
	settextcolor(RGB(245, 245, 245));
	LOGFONT font;
	gettextstyle(&font);
	strcpy(font.lfFaceName, _T("ºÚÌå"));
	font.lfHeight = Board::Size;
	font.lfWidth = 0;
	font.lfQuality = PROOF_QUALITY;
	settextstyle(&font);
	drawtext("steps", &pRect1, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	drawtext("score", &pRect2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	font.lfHeight = 0.5 * Board::Size;
	settextstyle(&font);
	settextcolor(RGB(200, 200, 200));
	drawtext("2048SuperRun V5.0.1.7\nCopyright@ JSY 2019.8", &pRect3, DT_CENTER | DT_VCENTER);
}
#endif
int main(void)
{
#ifdef CLEAR

	FILE* fp = fopen(Agent::FilePath, "wb+");
	float a = 0.0F;
	for (int i = 0; i < DATA_NUM; i++)
	{
		fwrite(&a, 4, 1, fp);
	}
	fclose(fp);
	return 0;
#endif // CLEAR
#ifdef SHOW

	FILE* fp = fopen(Agent::FilePath, "rb");
	float a = 0.0F;
	int counter = 0;
	for (int i = 0; i < DATA_NUM; i++)
	{
		fread(&a, 4, 1, fp);
		//printf("%f\n", a);
		if (a)counter++;
	}
	printf("%d\n", counter);
	system("pause");
	fclose(fp);
	return 0;
#endif // SHOW
//-----------------------------------------------------------------

	srand(clock() + time(0));

	Agent SR5_Ai(0.1f);

#ifdef GUI
	initgraph(18 * Board::Board::Size, 25 * Board::Board::Size);
	InitGUI();
	SR5_Ai.Play(60*0x2000/*8192*/);
	closegraph();
#elif defined TIMER
	clock_t start = clock();
	SR5_Ai.Play(1000);
	clock_t end = clock();
	printf("\n%.1lf s\n", (end - start) / (double)CLK_TCK);
	std::system("pause");
#elif 0
	for (int i = 1; i <= 32; i++)
	{
		FILE* fp = fopen(Agent::FilePath, "wb+");
		float a = 0.0F;
		for (int j = 0; j < DATA_NUM; j++)fwrite(&a, 4, 1, fp);
		fclose(fp);
		Agent SR5_Ai(0.1f);
		SR5_Ai.Learn(30000);
	}
#else
	//SR5_Ai.Learn(60*0x2000/*8192*/);
	SR5_Ai.Play(1000);
#endif
//-----------------------------------------------------------------
	return 0;
}