#pragma once
#include<graphics.h>	//�谲װeasyxͼ�ο�
#include<vector>
using namespace std;

struct Point {
	int row;		//ÿ������������Ϸ��ĵڼ��еڼ���
	int col;
};

class Block
{
public:
	Block();
	void drop();//��׹
	void moveLeftRight(int offset);//����or����
	void rotate();//��ת
	void draw(int leftMargin, int topMargin);//���ƶ���˹����
	static IMAGE** getImages();
	Block& operator=(const Block& block);

	//�жϷ�������Ϸ��λ���Ƿ�Ϸ�
	//λ�õĺϷ��ж�����Ϸ������ϢϢ��أ�������Ҫ�������
	bool blockInMap(const vector<vector<int>> &map);
	
	void solidify(vector<vector<int>>& map);//�̻����飬Ҫ��map�е�ֵ���и���

	int getBlockType();//��ȡ����˹��������

private:
	//����˹��������
	int blockType;
	//һ�����͵Ķ���˹���飬���ĸ�С���飬����Ҫ֪��ÿ��С�������λ��(��Ϸ���ھ���λ��)
	Point smallBlocks[4];	
	IMAGE *img;	//��Ϸ�п��ܴ��ڶ����ͬ�Ķ���˹���飬7��С����7����ɫ��imgָ������һ����ɫ

	//int arr[7],��Ҫ��������ķ���ֵ��int* p��p=arr���������int* arr[]����Ҫ���վ���int** p
	static IMAGE* imgs[7];	//7�ֻ����Ķ���˹���飬��̬���ԣ�����������Ƕ���

	static int size;
};

