#include "Block.h"

IMAGE* Block::imgs[7] = { NULL };
int Block::size = 36;

Block::Block()
{
	if (imgs[0] == NULL) {
		//切割我们提供的素材图片，并赋给这个图片数组
		IMAGE imgTmp;
		loadimage(&imgTmp, "tiles.png");

		SetWorkingImage(&imgTmp);
		for (int i = 0; i < 7; i++) {
			imgs[i] = new IMAGE;
			getimage(imgs[i], i * size, 0, size, size);
		}
		SetWorkingImage();//恢复工作区
	}

	int blocks[7][4] = {
		1,3,5,7,//长条形
		2,4,5,7,//Z1
		3,5,4,6,//Z2
		3,5,4,7,//T
		2,3,5,7,//L1
		3,5,7,6,//L2
		2,3,4,5,//田
	};
	//随机生成一种俄罗斯方块，这里先就这样，到后面可以考虑更改
	blockType = rand() % 7+1;
	
	//初始化Point结构数组
	for (int i = 0; i < 4; i++) {
		int v = blocks[blockType-1][i];
		smallBlocks[i].row = v / 2;
		smallBlocks[i].col = v % 2;
	}
	img = imgs[0];//这里可以留一个接口，到时候方便进行图片更换的目的
}

void Block::drop()
{
	for (int i = 0; i < 4; i++) {
		smallBlocks[i].row++;
	}
}

void Block::moveLeftRight(int offset)
{
	for (int i = 0; i < 4; i++) {
		smallBlocks[i].col += offset;
	}
}

void Block::rotate()
{
	//这里我们打算采用旋转公式来实现
	Point p = smallBlocks[1];
	//需要在报告中研究推导
	for (int i = 0; i < 4; i++) {
		Point tmp = smallBlocks[i];
		smallBlocks[i].col = p.col - tmp.row + p.row;
		smallBlocks[i].row = p.row + tmp.col - p.col;
	}
}

void Block::draw(int leftMargin, int topMargin)
{
	for (int i = 0; i < 4; i++) {
		int x = leftMargin + smallBlocks[i].col * size;
		int y = topMargin + smallBlocks[i].row * size;
		putimage(x, y, img);
	}
}

IMAGE** Block::getImages()
{
	return imgs;
}

Block& Block::operator=(const Block& r)
{
	if (this == &r) return *this;

	this->blockType = r.blockType;
	for (int i = 0; i < 4; i++) {
		this->smallBlocks[i] = r.smallBlocks[i];
	}
	return *this;
}

void Block::solidify(vector<vector<int>>& map)
{
	for (int i = 0; i < 4; i++) {
		map[smallBlocks[i].row][smallBlocks[i].col] = blockType;//把这个位置设定为第几种方块的类型
	}
}


bool Block::blockInMap(const vector<vector<int>>& map)
{
	int rows = map.size();
	int cols = map[0].size();
	for (int i = 0; i < 4; i++) {
		//这里是碰撞逻辑，注意难点是判断当前位置有没有已经落下的小方块
		if (smallBlocks[i].col < 0 || smallBlocks[i].col >= cols ||
			smallBlocks[i].row < 0 || smallBlocks[i].row >= rows ||
			map[smallBlocks[i].row][smallBlocks[i].col] != 0)return false;
	}
	return true;
}

int Block::getBlocktype()
{
	return blockType;
}

int* Block::getBlockPos()
{
	int x[8];
	for (int i = 0; i < 4; i++) {
		x[i] = smallBlocks[i].col;
		x[i + 4] = smallBlocks[i].row;
	}
	return x;
}

void Block::setBlocktype(int a)
{
	blockType = a;
}

void Block::setBlockPos(int* a)
{
	for (int i = 0; i < 4; i++) {
		smallBlocks[i].col = a[i];
		smallBlocks[i].row = a[i + 4];
	}
}
