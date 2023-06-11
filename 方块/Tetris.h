#pragma once
#include<cstdlib>
#include<iostream>
#include<conio.h>
#include<ctime>
#include<graphics.h>
#include<fstream>
#include <vector>
#include"Block.h"
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")

using namespace std;

class Tetris
{
public:
	//构造函数，传入游戏区域的行数，列数，左边界和顶部边界的距离，方块的大小
	Tetris(int rows, int cols, int left, int top, int blockSize);
	//初始化，将一些必要的参数设置好
	void init();
	//游戏主体逻辑，功能在这里执行
	void game();
	




private:
	int delay;//设置刷新的时间
	bool update;//判断是否刷新
	void keyEvent();//判断用户输入
	int getDelay();//距离上一次调用该函数经历了多长时间，第一次调用返回0
	void drop();//调用Block的下降逻辑
	void updateWindow();//刷新窗口，渲染游戏画面
	void clearLine();//清行
	void moveLeftRight(int offset);//左右移动
	void rotate();//旋转
	void drawScore();//绘制当前分数
	void checkOver();//判断游戏结束
	void saveScore();//保存最高分
	void displayOver();//更新结束画面
	void beingPaused();//暂停状态下的按键操作
	void saveProgress(const string& filename);//存档
	void loadProgress(const string& filename);//读档

	vector<vector<int>>map;
	int rows;
	int cols;
	int leftMargin;
	int topMargin;
	int blockSize;
	IMAGE imgBk;
	IMAGE imgOver;
	IMAGE imgWin;

	Block* curBlock;
	Block* nextBlock;
	Block bakBlock;//当前方块降落时用来备份上一个合法位置，注意这里不用指针，需要弄清楚

	int score;//当前分数
	int level;//当前关卡
	int lineCount;//当前消除的行数
	int highestScore;//最高分
	bool gameOver;//游戏结束的状态标志
	bool pause;//判断游戏暂停状态
	bool saveKey;//加入存档按键
	bool loadKey;//加入加载按键
};

