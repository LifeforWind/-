#pragma once
#include<cstdlib>
#include<graphics.h>
#include<vector>
using namespace std;
//����С���������
struct Point {
	int row;
	int col;
};

class Block
{
public:
	//���캯�� 
	Block();
	//ʵ�ַ�����Զ����䣬���������ײ�߼�
	void drop();
	//ʵ�ַ��������˶�������offset��Ϊ1�������ң�Ϊ-1�����������������ײ�߼�
	void moveLeftRight(int offset);
	//ʵ�ַ������ת�߼�����Ҫ�㷨����
	void rotate(); 
	//ʵ�ַ���Ļ��ƣ��ڽ����ϵ���ʾ
	void draw(int leftMargin, int topMargin);
	//���ط����ͼ
	static IMAGE** getImages();
	//��ֵ���캯��
	Block& operator=(const Block& r);
	//����̻�
	void solidify(vector<vector<int>>& map);
	//�жϷ���λ���Ƿ�Ϸ�������ʵ����ײ�߼�
	bool blockInMap(const vector<vector<int>>&map);
	//���ط������͵Ľӿ�
	int getBlocktype();
	//����С�����λ����Ϣ
	int* getBlockPos();
	//���÷������ͣ�������
	void setBlocktype(int a);
	//����λ����Ϣ
	void setBlockPos(int *a);

private:
	int blockType;//�������ͣ�7��
	Point smallBlocks[4];//ÿ�ַ��鶼����4��С�������
	IMAGE *img;//����С������һ���ģ�ͼƬ����һ���ģ����������ڶ࣬�Ӷ�����ʹ��ָ�룬���Ը��ӽ�ʡ�ռ�

	static IMAGE* imgs[7];//����ʹ�þ�̬��������ΪͼƬ������ĳһ��block������˽�еģ������ÿ��������ʹ�õ�
	static int size;//ͼƬ��С���������ͼƬ��С����ͬ
};

