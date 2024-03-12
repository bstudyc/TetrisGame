/*开发日志
* 1.创建项目
* 2.素材导入
* 3.c++开发
* 4.设计c++项目的模块
	设计类：方块类Block，游戏类Tetris
* 5.设计各个模块的主要接口
* 6.启动游戏，写主函数

*/
#include"Tetris.h"
#include<iostream>

using namespace std;

int main(void) {
	
	Tetris game(20,10,236,78,32);
	game.play();

	return 0;
}