#pragma once
#include<graphics.h>	//需安装easyx图形库
#include<vector>
using namespace std;

struct Point {
	int row;		//每个方块所在游戏框的第几行第几列
	int col;
};

class Block
{
public:
	Block();
	void drop();//下坠
	void moveLeftRight(int offset);//左移or右移
	void rotate();//旋转
	void draw(int leftMargin, int topMargin);//绘制俄罗斯方块
	static IMAGE** getImages();
	Block& operator=(const Block& block);

	//判断方块在游戏中位置是否合法
	//位置的合法判断与游戏的行列息息相关，所以需要这个参数
	bool blockInMap(const vector<vector<int>> &map);
	
	void solidify(vector<vector<int>>& map);//固化方块，要对map中的值进行更改

	int getBlockType();//获取俄罗斯方块类型

private:
	//俄罗斯方块类型
	int blockType;
	//一个类型的俄罗斯方块，有四个小方块，且需要知道每个小方块绝对位置(游戏框内绝对位置)
	Point smallBlocks[4];	
	IMAGE *img;	//游戏中可能存在多个相同的俄罗斯方块，7种小方块7种颜色，img指向其中一种颜色

	//int arr[7],想要接收数组的返回值就int* p，p=arr，那如果是int* arr[]，想要接收就是int** p
	static IMAGE* imgs[7];	//7种基础的俄罗斯方块，静态属性，属于类而不是对象

	static int size;
};

