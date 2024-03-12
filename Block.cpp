#include "Block.h"
#include<stdlib.h>

IMAGE* Block::imgs[7] = { NULL };
int Block::size = 32;

//ÿ�δ���һ��Block���󶼻��ʼ����Ҳ����˵ÿ�δ�������󶼻�������һϵ�в���
//������Щ�����󣬶�ȷ���˵�ǰ���������������˹�������ɫ����״
Block::Block()
{
	if (imgs[0] == NULL) {
		//7�ֶ���˹�����ʼ������ͼƬ�и�
		IMAGE imgTmp;
		loadimage(&imgTmp, "res/tiles.png");

		//�����и����
		SetWorkingImage(&imgTmp);
		for (int i = 0; i < 7; i++) {
			imgs[i] = new IMAGE;
			getimage(imgs[i], i * size,0,size,size);	//imgs[]������7�ֲ�ͬ��ɫ��С����
		}
		SetWorkingImage();//�ָ�������
	}

	//�����Ķ���˹�����ʼ����
	int block[7][4] = {
		1,3,5,7,	//I
		2,4,5,7,	//Z 1��
		3,5,4,6,	//Z 2��
		3,5,4,7,	//T
		2,3,5,7,	//L
		3,5,7,6,	//J
		2,3,4,5,	//��
	};

	//���ѡ��һ�ֶ���˹��������,1-7
	blockType = rand() % 7 + 1;

	//��ʼ��smallBlocks��Ҳ���Ƕ���˹����ճ��ֵ�λ��
	//��Ϸ������У�����20��10�У���Ϸ��ʼʱ�����ݲ�ͬ���͵Ķ���˹���������ʼλ��
	for (int i = 0; i < 4; i++) {
		//���εõ����ɶ���˹�����Ӧ������ֵ��Ҳ����arr[4][2]�е�λ�ö�Ӧ��ֵ��
		int value = block[blockType - 1][i];

		smallBlocks[i].row = value / 2;//���λ�õ���->����λ�õ���
		smallBlocks[i].col = value % 2;//���λ�õ���->����λ�õ���
	}

	//�����·�����ɫ�����Կ���blockType������block����������һ�ַ������ͣ�Ҳ������imgѡ�񼴾���������ɫ��
	//��ʼ������ͬ���͵Ķ���˹����Ҳ������ͬ����ɫ��img��7����ɫ��С�����е�һ��
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
	//�Ĵ�ѭ��,ÿһ��ѭ���ҵ����Ƕ���˹�����е�ĳһС���λ�ã�����֪����λ�õ�x��y�����䷽��ͼƬimg����
	for (int i = 0; i < 4; i++) {
		//��߽�(��Ϸ��߿�ͼƬ��߿�)����(С�������λ�ö�����*С���鳤��Ҳ����С��������Ϸ�߿�೤)
		//�õ����Ƿ�����ͼ�еľ���λ��
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
	//����ֵ�������Ͳ����в���
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
	//����λ�úϷ��ԣ����ܳ�����Ϸ���У�Ҳ�������Ѿ���С����ĵط�
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
		//�̻���Ӧλ��
		map[smallBlocks[i].row][smallBlocks[i].col] = blockType;
	}
}

int Block::getBlockType()
{
	return blockType;
}
