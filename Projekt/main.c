#include "chess.h"
#include "vld.h"
#include <vld.h>

int main(int argc, char **argv) { // 1 arg,GET_SAVE - 1, I_PLAYER - 2, II_PLAYERS - 3, II_COMPUTERS - 4

	VLDGetOptions();
	runChess(argc,argv);
//	system("pause");
	return 0;
}