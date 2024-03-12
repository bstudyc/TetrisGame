#include "Tetris.h"
#include"Block.h"
#include<ctime>
#include<stdlib.h>
#include<fstream>
#include<vector>
#include<iostream>
#include<conio.h>
#include<mmsystem.h>
#pragma comment(lib,"WINMM.LIB")

#define MAX_LEVEL 5//最难关卡是5

using namespace std;

const int SPEED_NORMAL[MAX_LEVEL] = { 500,300,200,150,100 };
const int SPEED_QUICK = 50;//ms

//游戏页面初始化，行、列、左边界、上边界、每个小方块多长
Tetris::Tetris(int rows, int cols, int left, int top, int blockSize)
{
	this->rows = rows;
	this->cols = cols;
	this->leftMargin = left;
	this->topMargin = top;
	this->blocksize = blockSize;

	for (int i = 0; i < this->rows; i++) {
		vector<int> mapRow;
		for (int j = 0; j < this->cols; j++) {
			mapRow.push_back(0);
		}
		map.push_back(mapRow);
	}


	//初始化最高分,读文件
	ifstream file("record_score.txt");
	if (!file.is_open()) {
		//第一次打开文件会失败，因为没有创建，也没有历史分数
		maxScore = 0;
	}
	else {
		//打开成功，读取里面的分数
		file >> maxScore;
	}
	//初始化分数后，关闭文件
	file.close();
}

void Tetris::init()
{
	//打开背景音乐
	mciSendString("play res/bg_music.mp3 repeat", 0, 0, 0);

	//正常速度下降
	delay = SPEED_NORMAL[0];
	update = false;

	//配置随机种子，为方块随机选择做准备
	srand((unsigned int)time(NULL));

	//创建游戏窗口,831、748
	initgraph(831,748);

	//加载背景图片
	loadimage(&img_Bg, "res/background.png");

	//加载游戏结束后的界面
	loadimage(&imgWin, "res/success.png");
	loadimage(&imgOver, "res/failure.png");

	//初始化游戏区的数据
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			map[i][j] = 0;
		}
	}

	score = 0;
	lineCount = 0;
	level = 1;
	gameOver = false;
}

void Tetris::play()
{
	//初始化
	init();

	//先生成第一个方块赋值给当前方块，再生产新的下一个方块作为预告
	nextBlock = new Block();
	curBlock = nextBlock;
	nextBlock = new Block();


	//定时器
	int time = 0;
	
	//开玩
	while (1) {
		//接收用户输入
		keyEvent();

		time += getDelay();
		
		//每隔一段时间就下降
		if (time > delay ) {
			time = 0;
			drop();	//每隔delay时间下降一块
			update = true;
		}

		if (update) {
			//更新游戏画面,没必要每次循环都更新,开销大,更新的原因一般要么是过了delay时间（也是下坠时间），要么是方块的变化
			//update可以是因为时间到变为true，也可以是因为方块的变化（旋转、平移）等时变化为true
			//下降一次或者是对俄罗斯方块进行操作就会将update设为true，并进入该if语句中，更新画面并判断是否会出现消行现象
			update = false;
			updateWindow();
			//如果一行排在一起，就需要消行，更新数据的判断
			cleanLine();
		}

		if (gameOver) {
			//保存分数(最高才写入存储分数的文件)
			saveScore();

			//游戏结束界面
			overShow();

			system("pause");
			init();  //重新开局
		}
		
	}
}

void Tetris::keyEvent()
{
	unsigned char ch=' ';//0 - 255
	bool rotateFlag = false;//旋转标记
	int dx = 0;//偏移量，用这个偏移量值决定左移还是右移

	//不能用getchar()，如果用这个，那么玩家按了方向键后还需要再按回车才算真的输入
	//按一个按键直接输入了，免回车,问题是如果玩家始终没有按键，就会一直等待，出现阻塞，无法继续接下来的操作
	if (_kbhit()) {
		//有按键输入就读
		ch=_getch();
		//如果按下方向键（功能按键），会自动返回两个字符
		//向上:224 72(旋转)  向下(加速):224 80  向左(左移):224 75  向右(右移):224 77
		if (ch == 224) {
			ch = _getch();
			switch (ch) {
			case 72:
				rotateFlag = true;
				break;
			case 80:
				delay = SPEED_QUICK;
				break;
			case 75:
				dx = -1;
				break;
			case 77:
				dx = 1;
				break;
			default:
				break;
			}
		}
	}

	if (rotateFlag) {
		//旋转俄罗斯方块的实现
		rotate();
		update = true;
	}

	if (dx != 0) {
		moveLeftRight(dx);//实现左右移动
		update = true;	//操作后要更新页面
	}

}

void Tetris::updateWindow()
{
	//绘制背景图片
	putimage(0, 0, &img_Bg);

	//绘制完所有后一次性渲染，就不会有方块一闪闪的感觉
	//利用BeginBatchDraw,EndBatchDraw
	BeginBatchDraw();

	//底部固定方块的渲染
	IMAGE** imgs = Block::getImages();
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (map[i][j] == 0) continue;

			int x = leftMargin + j * blocksize;
			int y = topMargin + i * blocksize;
			putimage(x, y, imgs[map[i][j]-1]); //map中方块种类是从1算起，0表示的是空，但是imgs中种类从0算起
		}
	}

	//可操作方块、预告方块的渲染
	curBlock->draw(leftMargin, topMargin);
	nextBlock->draw(650,200);

	drawScore();//绘制分数

	EndBatchDraw();
}


//第一次调用，返回0
//后面调用，返回距离上一次调用，间隔了多少ms
int Tetris::getDelay()
{
	static unsigned long long lastTime = 0;
	unsigned long long currentTime = GetTickCount();
	if (lastTime == 0) {
		lastTime = currentTime;
		return 0;
	}
	else {
		int ret = currentTime - lastTime;
		lastTime = currentTime;
		return ret;
	}
}

void Tetris::drop()
{
	bakBlock = *curBlock;
	curBlock->drop();

	//下降过程中检查降落位置
	if (!curBlock->blockInMap(map)) {
		//这时候位置不合法，已经到了底部，需要将方块进行固化
		//需要固化的位置应该是合法位置，即俄罗斯方块的上一个位置,也就是备份好的bakBlock
		bakBlock.solidify(map);

		//固化完成后，不再需要这个俄罗斯方块。curBlock是指针，指向new出来的堆区数据，防止内存泄漏,删除该指针指向的内存
		delete curBlock;

		//当前方块指针指向预告方块准备好的在堆区new出来的方块
		curBlock = nextBlock;

		//预告方块更新
		nextBlock = new Block();

		//检查游戏是否结束
		checkOver();
	}
	//在加速下降操作之后需要将速度还原，不然会出现方块一直快速下降，还原后能实现按一下向下键就加速一下
	delay = SPEED_NORMAL[level-1];

}

void Tetris::cleanLine()
{
	int lines = 0;//统计消掉的行数
	int k = rows - 1;//存储数据的行数
	for (int i = rows - 1; i >= 0; i--) {
		int count = 0;
		for (int j = 0; j < cols; j++) {
			//检查第i行是否满行
				if (map[i][j] != 0) {
					count++;
				}
				map[k][j] = map[i][j];//一边扫描一边存储
		}
		if (count < cols) {
			//不满行
			k--;
		}
		else {
			//k不变，那么下一次循环一边扫描一边覆盖就会在满行上进行操作
			lines++;
		}
	}

	if (lines > 0) {
		//计算得分
		lineCount = lineCount + lines;
		score = score + lines * 10;
		mciSendString("play res/clear_music.wav", 0, 0, 0);
		update = true;


		//每100分一个级别，0-100第一关，101-200第二关
		level = (score + 99) / 100;//进入该if语句说明是消除了行的，分数不可能是0
		if (level > MAX_LEVEL) {
			//通过第五关游戏胜利，也算结束
			gameOver = true;
		}
	}
}

void Tetris::moveLeftRight(int offset)
{
	bakBlock = *curBlock;
	curBlock->moveLeftRight(offset);
	
	if (curBlock->blockInMap(map)==false) {
		*curBlock = bakBlock;
	}

}

void Tetris::rotate()
{
	//田字型旋转不变
	if (curBlock->getBlockType() == 7) return;

	bakBlock = *curBlock;
	curBlock->rotate();

	if (curBlock->blockInMap(map) == false) {
		*curBlock = bakBlock;
	}
}

void Tetris::drawScore()
{
	//显示分数
	char scoreText[32];
	sprintf_s(scoreText, sizeof(scoreText), "%d", score);

	LOGFONT f;
	gettextstyle(&f);//获取当前字体
	f.lfHeight = 20;//字体高度40像素
	f.lfWidth = 20;//字体宽度40像素
	f.lfQuality = ANTIALIASED_QUALITY;//抗锯齿效果，边界更圆润
	settextstyle(&f);//设置当前字体

	setbkmode(TRANSPARENT);//字体背景透明效果

	outtextxy(600, 550, scoreText);//显示

	//显示行数
	sprintf_s(scoreText, sizeof(scoreText), "%d", lineCount);
	outtextxy(150, 630, scoreText);

	//显示游戏关卡
	sprintf_s(scoreText, sizeof(scoreText), "%d", level);
	outtextxy(150, 550, scoreText);

	//显示历史最高分数
	sprintf_s(scoreText, sizeof(scoreText), "%d", maxScore);
	outtextxy(600, 630, scoreText);
}

void Tetris::checkOver()
{
	//方块位置不合理
	if (curBlock->blockInMap(map) == false) {
		gameOver = true;
	}
}

void Tetris::saveScore()
{
	if (score > maxScore) {
		maxScore = score;
		ofstream file("record_score.txt");
		file << maxScore;
		file.close();
	}
}

void Tetris::overShow()
{
	mciSendString("stop res/bg_music.mp3",0,0,0);

	//胜利结束还是失败结束
	if (level <= MAX_LEVEL) {
		putimage(150, 100, &imgOver);
		mciSendString("play res/over.wav", 0, 0, 0);
	}
	else {
		putimage(150, 100, &imgWin);
		mciSendString("play res/pass.ogg", 0, 0, 0);
	}
}
