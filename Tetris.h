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
	void init();//��ʼ��
	void play();//��ʼ��Ϸ

	//�ڲ��ã�����˽�е�
private:
	void keyEvent();		
	void updateWindow();	//��Ⱦ��Ϸ����

	//���ؾ�����һ�ε��øú���������˶���ʱ��(ms)
	//��һ�ε��øú���������0
	int getDelay();
	void drop();
	void cleanLine();
	void moveLeftRight(int offset);//�����ƶ�
	void rotate();//��ת
	void drawScore();//��ʾ����
	void checkOver();//�����Ϸ�Ƿ����
	void saveScore();//�������߷ֱ������
	void overShow();//��Ϸ��������չʾ

private:
	int delay; //��Ⱦ��ʱ�����������ʱͬʱҲ�Ƕ���˹���齵���ʱ��
	bool update; //�Ƿ���£�������ת��ƽ�ƵȾ���Ҫ������Ⱦ�����ǵȴ�delayʱ�䣩

	//int map[][]
	//��ά������Կ��������ÿ����Ա����һά���飬vector�Ƕ�̬���飨���ȿ��Ա仯�����飩
	//0:�հף�û���κη���	5����5�����͵Ķ���˹���飬����ͬ��
	vector<vector<int>> map;

	//��Ϸ�����С���
	int rows;
	int cols;

	//���߽硢�ϱ߽硢�����С
	int leftMargin;
	int topMargin;
	int blocksize;

	//����ͼƬ
	IMAGE img_Bg;

	//Ԥ�淽��
	Block* nextBlock;
	//��ǰ����
	Block* curBlock;
	//��ǰ���齵������У�����������һ���Ϸ�λ�õ�
	Block bakBlock;

	int score;//��Ϸ����
	int level;//��ǰ�ؿ�
	int lineCount;//�����˵�������
	int maxScore;//��߷�

	bool gameOver;//��Ϸ�Ƿ������״̬

	IMAGE imgOver;//ʧ�ܽ���
	IMAGE imgWin;//ʤ������
};

