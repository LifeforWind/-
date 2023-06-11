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
	//���캯����������Ϸ�������������������߽�Ͷ����߽�ľ��룬����Ĵ�С
	Tetris(int rows, int cols, int left, int top, int blockSize);
	//��ʼ������һЩ��Ҫ�Ĳ������ú�
	void init();
	//��Ϸ�����߼�������������ִ��
	void game();
	




private:
	int delay;//����ˢ�µ�ʱ��
	bool update;//�ж��Ƿ�ˢ��
	void keyEvent();//�ж��û�����
	int getDelay();//������һ�ε��øú��������˶೤ʱ�䣬��һ�ε��÷���0
	void drop();//����Block���½��߼�
	void updateWindow();//ˢ�´��ڣ���Ⱦ��Ϸ����
	void clearLine();//����
	void moveLeftRight(int offset);//�����ƶ�
	void rotate();//��ת
	void drawScore();//���Ƶ�ǰ����
	void checkOver();//�ж���Ϸ����
	void saveScore();//������߷�
	void displayOver();//���½�������
	void beingPaused();//��ͣ״̬�µİ�������
	void saveProgress(const string& filename);//�浵
	void loadProgress(const string& filename);//����

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
	Block bakBlock;//��ǰ���齵��ʱ����������һ���Ϸ�λ�ã�ע�����ﲻ��ָ�룬��ҪŪ���

	int score;//��ǰ����
	int level;//��ǰ�ؿ�
	int lineCount;//��ǰ����������
	int highestScore;//��߷�
	bool gameOver;//��Ϸ������״̬��־
	bool pause;//�ж���Ϸ��ͣ״̬
	bool saveKey;//����浵����
	bool loadKey;//������ذ���
};

