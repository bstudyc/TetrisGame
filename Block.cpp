#include "Block.h"
#include<stdlib.h>

IMAGE* Block::imgs[7] = { NULL };
int Block::size = 32;

//每次创建一个Block对象都会初始化，也就是说每次创建对象后都会有下面一系列操作
//经过这些操作后，都确定了当前对象代表的这个俄罗斯方块的颜色、形状
Block::Block()
{
	if (imgs[0] == NULL) {
		//7种俄罗斯方块初始化，对图片切割
		IMAGE imgTmp;
		loadimage(&imgTmp, "res/tiles.png");

		//设置切割对象
		SetWorkingImage(&imgTmp);
		for (int i = 0; i < 7; i++) {
			imgs[i] = new IMAGE;
			getimage(imgs[i], i * size,0,size,size);	//imgs[]里面是7种不同颜色的小方块
		}
		SetWorkingImage();//恢复工作区
	}

	//基本的俄罗斯方块初始类型
	int block[7][4] = {
		1,3,5,7,	//I
		2,4,5,7,	//Z 1型
		3,5,4,6,	//Z 2型
		3,5,4,7,	//T
		2,3,5,7,	//L
		3,5,7,6,	//J
		2,3,4,5,	//填
	};

	//随机选择一种俄罗斯方块生成,1-7
	blockType = rand() % 7 + 1;

	//初始化smallBlocks，也就是俄罗斯方块刚出现的位置
	//游戏框的行列，比如20行10列，游戏开始时，根据不同类型的俄罗斯方块决定初始位置
	for (int i = 0; i < 4; i++) {
		//依次得到生成俄罗斯方块对应的数组值（也就是arr[4][2]中的位置对应的值）
		int value = block[blockType - 1][i];

		smallBlocks[i].row = value / 2;//相对位置的行->绝对位置的行
		smallBlocks[i].col = value % 2;//相对位置的列->绝对位置的列
	}

	//配置新方块颜色，可以看出blockType决定了block即决定是哪一种方块类型，也决定了img选择即决定方块颜色，
	//初始化是相同类型的俄罗斯方块也有着相同的颜色，img是7种颜色的小方块中的一种
	img = imgs[blockType - 1];

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
	Point p = smallBlocks[1];
	for (int i = 0; i < 4; i++) {
		Point temp = smallBlocks[i];
		smallBlocks[i].col = p.col - temp.row + p.row;
		smallBlocks[i].row = p.row + temp.col - p.col;
	}
}

void Block::draw(int leftMargin, int topMargin)
{
	//四次循环,每一次循环找到的是俄罗斯方块中的某一小块的位置，并且知道该位置的x、y轴后填充方块图片img即可
	for (int i = 0; i < 4; i++) {
		//左边界(游戏左边框到图片左边框)加上(小方块绝对位置多少列*小方块长，也就是小方块离游戏边框多长)
		//得到的是方块在图中的绝对位置
		int x = leftMargin + smallBlocks[i].col * size;
		int y = topMargin + smallBlocks[i].row * size;
		putimage(x, y, img);
	}
}

IMAGE** Block::getImages()
{
	return imgs;
}

Block& Block::operator=(const Block& block)
{
	//本身赋值给本身，就不进行操作
	if (this == &block) return *this;

	this->blockType = block.blockType;
	for (int i = 0; i < 4; i++) {
		this->smallBlocks[i] = block.smallBlocks[i];
	}
	return *this;
}

bool Block::blockInMap(const vector<vector<int>>& map)
{
	int rows = map.size();
	int cols = map[0].size();
	//方块位置合法性，不能超出游戏行列，也不能在已经有小方块的地方
	for (int i = 0; i < 4; i++) {
		if (smallBlocks[i].row<0 ||smallBlocks[i].row>=rows ||
			smallBlocks[i].col<0 ||smallBlocks[i].col>=cols ||
			map[smallBlocks[i].row][smallBlocks[i].col]!=0) {
			return false;
		}
	}
	return true;
}

void Block::solidify(vector<vector<int>>& map)
{
	for (int i = 0; i < 4; i++) {
		//固化相应位置
		map[smallBlocks[i].row][smallBlocks[i].col] = blockType;
	}
}

int Block::getBlockType()
{
	return blockType;
}
