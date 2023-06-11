#pragma once
#include<cstdlib>
#include<graphics.h>
#include<vector>
using namespace std;
//用于小方块的坐标
struct Point {
	int row;
	int col;
};

class Block
{
public:
	//构造函数 
	Block();
	//实现方块的自动下落，这里会有碰撞逻辑
	void drop();
	//实现方块左右运动，传入offset，为1代表向右，为-1代表向左，这里会有碰撞逻辑
	void moveLeftRight(int offset);
	//实现方块的旋转逻辑，需要算法解释
	void rotate(); 
	//实现方块的绘制，在界面上的显示
	void draw(int leftMargin, int topMargin);
	//返回方块的图
	static IMAGE** getImages();
	//赋值构造函数
	Block& operator=(const Block& r);
	//方块固化
	void solidify(vector<vector<int>>& map);
	//判断方块位置是否合法，便于实现碰撞逻辑
	bool blockInMap(const vector<vector<int>>&map);
	//返回方块类型的接口
	int getBlocktype();
	//返回小方块的位置信息
	int* getBlockPos();
	//设置方块类型（读档）
	void setBlocktype(int a);
	//设置位置信息
	void setBlockPos(int *a);

private:
	int blockType;//方块类型，7种
	Point smallBlocks[4];//每种方块都是由4个小方块组成
	IMAGE *img;//由于小方块是一样的，图片都是一样的，而且数量众多，从而这里使用指针，可以更加节省空间

	static IMAGE* imgs[7];//这里使用静态变量，因为图片并不是某一个block对象所私有的，这个是每个都可以使用的
	static int size;//图片大小，整个类的图片大小都相同
};

