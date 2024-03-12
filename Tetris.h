#pragma once
#include<vector>
#include<graphics.h>
#include"Block.h"
using namespace std;

class Tetris
{
public:
	Tetris(){}
	Tetris(int rows, int cols, int left, int top, int blockSize);
	void init();//初始化
	void play();//开始游戏

	//内部用，就用私有的
private:
	void keyEvent();		
	void updateWindow();	//渲染游戏画面

	//返回距离上一次调用该函数，间隔了多少时间(ms)
	//第一次调用该函数，返回0
	int getDelay();
	void drop();
	void cleanLine();
	void moveLeftRight(int offset);//左右移动
	void rotate();//旋转
	void drawScore();//显示分数
	void checkOver();//检查游戏是否结束
	void saveScore();//如果是最高分保存分数
	void overShow();//游戏结束界面展示

private:
	int delay; //渲染延时，而且这个延时同时也是俄罗斯方块降落的时间
	bool update; //是否更新（方块旋转、平移等就需要马上渲染而不是等待delay时间）

	//int map[][]
	//二维数组可以看做数组的每个成员都是一维素组，vector是动态数组（长度可以变化的数组）
	//0:空白，没有任何方块	5：第5种类型的俄罗斯方块，其他同理
	vector<vector<int>> map;

	//游戏多少行、列
	int rows;
	int cols;

	//左侧边界、上边界、方块大小
	int leftMargin;
	int topMargin;
	int blocksize;

	//背景图片
	IMAGE img_Bg;

	//预告方块
	Block* nextBlock;
	//当前方块
	Block* curBlock;
	//当前方块降落过程中，用来备份上一个合法位置的
	Block bakBlock;

	int score;//游戏分数
	int level;//当前关卡
	int lineCount;//消除了的行总数
	int maxScore;//最高分

	bool gameOver;//游戏是否结束的状态

	IMAGE imgOver;//失败界面
	IMAGE imgWin;//胜利界面
};

