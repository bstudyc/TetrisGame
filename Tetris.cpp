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

#define MAX_LEVEL 5//���ѹؿ���5

using namespace std;

const int SPEED_NORMAL[MAX_LEVEL] = { 500,300,200,150,100 };
const int SPEED_QUICK = 50;//ms

//��Ϸҳ���ʼ�����С��С���߽硢�ϱ߽硢ÿ��С����೤
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


	//��ʼ����߷�,���ļ�
	ifstream file("record_score.txt");
	if (!file.is_open()) {
		//��һ�δ��ļ���ʧ�ܣ���Ϊû�д�����Ҳû����ʷ����
		maxScore = 0;
	}
	else {
		//�򿪳ɹ�����ȡ����ķ���
		file >> maxScore;
	}
	//��ʼ�������󣬹ر��ļ�
	file.close();
}

void Tetris::init()
{
	//�򿪱�������
	mciSendString("play res/bg_music.mp3 repeat", 0, 0, 0);

	//�����ٶ��½�
	delay = SPEED_NORMAL[0];
	update = false;

	//����������ӣ�Ϊ�������ѡ����׼��
	srand((unsigned int)time(NULL));

	//������Ϸ����,831��748
	initgraph(831,748);

	//���ر���ͼƬ
	loadimage(&img_Bg, "res/background.png");

	//������Ϸ������Ľ���
	loadimage(&imgWin, "res/success.png");
	loadimage(&imgOver, "res/failure.png");

	//��ʼ����Ϸ��������
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
	//��ʼ��
	init();

	//�����ɵ�һ�����鸳ֵ����ǰ���飬�������µ���һ��������ΪԤ��
	nextBlock = new Block();
	curBlock = nextBlock;
	nextBlock = new Block();


	//��ʱ��
	int time = 0;
	
	//����
	while (1) {
		//�����û�����
		keyEvent();

		time += getDelay();
		
		//ÿ��һ��ʱ����½�
		if (time > delay ) {
			time = 0;
			drop();	//ÿ��delayʱ���½�һ��
			update = true;
		}

		if (update) {
			//������Ϸ����,û��Ҫÿ��ѭ��������,������,���µ�ԭ��һ��Ҫô�ǹ���delayʱ�䣨Ҳ����׹ʱ�䣩��Ҫô�Ƿ���ı仯
			//update��������Ϊʱ�䵽��Ϊtrue��Ҳ��������Ϊ����ı仯����ת��ƽ�ƣ���ʱ�仯Ϊtrue
			//�½�һ�λ����ǶԶ���˹������в����ͻὫupdate��Ϊtrue���������if����У����»��沢�ж��Ƿ�������������
			update = false;
			updateWindow();
			//���һ������һ�𣬾���Ҫ���У��������ݵ��ж�
			cleanLine();
		}

		if (gameOver) {
			//�������(��߲�д��洢�������ļ�)
			saveScore();

			//��Ϸ��������
			overShow();

			system("pause");
			init();  //���¿���
		}
		
	}
}

void Tetris::keyEvent()
{
	unsigned char ch=' ';//0 - 255
	bool rotateFlag = false;//��ת���
	int dx = 0;//ƫ�����������ƫ����ֵ�������ƻ�������

	//������getchar()��������������ô��Ұ��˷��������Ҫ�ٰ��س������������
	//��һ������ֱ�������ˣ���س�,������������ʼ��û�а������ͻ�һֱ�ȴ��������������޷������������Ĳ���
	if (_kbhit()) {
		//�а�������Ͷ�
		ch=_getch();
		//������·���������ܰ����������Զ����������ַ�
		//����:224 72(��ת)  ����(����):224 80  ����(����):224 75  ����(����):224 77
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
		//��ת����˹�����ʵ��
		rotate();
		update = true;
	}

	if (dx != 0) {
		moveLeftRight(dx);//ʵ�������ƶ�
		update = true;	//������Ҫ����ҳ��
	}

}

void Tetris::updateWindow()
{
	//���Ʊ���ͼƬ
	putimage(0, 0, &img_Bg);

	//���������к�һ������Ⱦ���Ͳ����з���һ�����ĸо�
	//����BeginBatchDraw,EndBatchDraw
	BeginBatchDraw();

	//�ײ��̶��������Ⱦ
	IMAGE** imgs = Block::getImages();
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (map[i][j] == 0) continue;

			int x = leftMargin + j * blocksize;
			int y = topMargin + i * blocksize;
			putimage(x, y, imgs[map[i][j]-1]); //map�з��������Ǵ�1����0��ʾ���ǿգ�����imgs�������0����
		}
	}

	//�ɲ������顢Ԥ�淽�����Ⱦ
	curBlock->draw(leftMargin, topMargin);
	nextBlock->draw(650,200);

	drawScore();//���Ʒ���

	EndBatchDraw();
}


//��һ�ε��ã�����0
//������ã����ؾ�����һ�ε��ã�����˶���ms
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

	//�½������м�齵��λ��
	if (!curBlock->blockInMap(map)) {
		//��ʱ��λ�ò��Ϸ����Ѿ����˵ײ�����Ҫ��������й̻�
		//��Ҫ�̻���λ��Ӧ���ǺϷ�λ�ã�������˹�������һ��λ��,Ҳ���Ǳ��ݺõ�bakBlock
		bakBlock.solidify(map);

		//�̻���ɺ󣬲�����Ҫ�������˹���顣curBlock��ָ�룬ָ��new�����Ķ������ݣ���ֹ�ڴ�й©,ɾ����ָ��ָ����ڴ�
		delete curBlock;

		//��ǰ����ָ��ָ��Ԥ�淽��׼���õ��ڶ���new�����ķ���
		curBlock = nextBlock;

		//Ԥ�淽�����
		nextBlock = new Block();

		//�����Ϸ�Ƿ����
		checkOver();
	}
	//�ڼ����½�����֮����Ҫ���ٶȻ�ԭ����Ȼ����ַ���һֱ�����½�����ԭ����ʵ�ְ�һ�����¼��ͼ���һ��
	delay = SPEED_NORMAL[level-1];

}

void Tetris::cleanLine()
{
	int lines = 0;//ͳ������������
	int k = rows - 1;//�洢���ݵ�����
	for (int i = rows - 1; i >= 0; i--) {
		int count = 0;
		for (int j = 0; j < cols; j++) {
			//����i���Ƿ�����
				if (map[i][j] != 0) {
					count++;
				}
				map[k][j] = map[i][j];//һ��ɨ��һ�ߴ洢
		}
		if (count < cols) {
			//������
			k--;
		}
		else {
			//k���䣬��ô��һ��ѭ��һ��ɨ��һ�߸��Ǿͻ��������Ͻ��в���
			lines++;
		}
	}

	if (lines > 0) {
		//����÷�
		lineCount = lineCount + lines;
		score = score + lines * 10;
		mciSendString("play res/clear_music.wav", 0, 0, 0);
		update = true;


		//ÿ100��һ������0-100��һ�أ�101-200�ڶ���
		level = (score + 99) / 100;//�����if���˵�����������еģ�������������0
		if (level > MAX_LEVEL) {
			//ͨ���������Ϸʤ����Ҳ�����
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
	//��������ת����
	if (curBlock->getBlockType() == 7) return;

	bakBlock = *curBlock;
	curBlock->rotate();

	if (curBlock->blockInMap(map) == false) {
		*curBlock = bakBlock;
	}
}

void Tetris::drawScore()
{
	//��ʾ����
	char scoreText[32];
	sprintf_s(scoreText, sizeof(scoreText), "%d", score);

	LOGFONT f;
	gettextstyle(&f);//��ȡ��ǰ����
	f.lfHeight = 20;//����߶�40����
	f.lfWidth = 20;//������40����
	f.lfQuality = ANTIALIASED_QUALITY;//�����Ч�����߽��Բ��
	settextstyle(&f);//���õ�ǰ����

	setbkmode(TRANSPARENT);//���屳��͸��Ч��

	outtextxy(600, 550, scoreText);//��ʾ

	//��ʾ����
	sprintf_s(scoreText, sizeof(scoreText), "%d", lineCount);
	outtextxy(150, 630, scoreText);

	//��ʾ��Ϸ�ؿ�
	sprintf_s(scoreText, sizeof(scoreText), "%d", level);
	outtextxy(150, 550, scoreText);

	//��ʾ��ʷ��߷���
	sprintf_s(scoreText, sizeof(scoreText), "%d", maxScore);
	outtextxy(600, 630, scoreText);
}

void Tetris::checkOver()
{
	//����λ�ò�����
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

	//ʤ����������ʧ�ܽ���
	if (level <= MAX_LEVEL) {
		putimage(150, 100, &imgOver);
		mciSendString("play res/over.wav", 0, 0, 0);
	}
	else {
		putimage(150, 100, &imgWin);
		mciSendString("play res/pass.ogg", 0, 0, 0);
	}
}
