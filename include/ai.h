#ifndef AI_H
#define AI_H

#include "game.h"

// AI执行落子，返回最佳落子坐标
void ai_make_move(Board* board, int* row, int* col);

// 设置AI搜索深度（默认4层）
void ai_set_depth(int depth);

// 获取AI搜索深度
int ai_get_depth(void);

// 评估指定玩家在棋盘上的优势值
int ai_evaluate(Board* board, int player);

#endif // AI_H
