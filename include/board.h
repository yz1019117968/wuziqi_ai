#ifndef BOARD_H
#define BOARD_H

#include "game.h"

// 初始化棋盘
void board_init(Board* board);

// 在指定位置落子，返回1成功，0失败（位置已有棋子）
int board_place_piece(Board* board, int row, int col, int player);

// 悔棋，撤回上一步，返回1成功，0失败（无棋可悔）
int board_undo(Board* board);

// 判断是否有玩家获胜，返回获胜玩家编号，0表示无人获胜
int board_check_win(Board* board, int row, int col);

// 判断棋盘是否已满（平局）
int board_is_full(Board* board);

// 判断位置是否在棋盘范围内
int board_is_valid_pos(int row, int col);

// 获取棋盘上指定位置的棋子
int board_get_piece(Board* board, int row, int col);

// 复制棋盘状态
void board_copy(Board* dest, Board* src);

#endif // BOARD_H
