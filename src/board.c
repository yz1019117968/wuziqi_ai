#include "board.h"
#include <string.h>

// 初始化棋盘
void board_init(Board* board) {
    memset(board->grid, 0, sizeof(board->grid));
    board->current_player = PLAYER_BLACK;  // 黑子先手
    memset(board->move_history, -1, sizeof(board->move_history));
    board->move_count = 0;
}

// 判断位置是否在棋盘范围内
int board_is_valid_pos(int row, int col) {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

// 获取棋盘上指定位置的棋子
int board_get_piece(Board* board, int row, int col) {
    if (!board_is_valid_pos(row, col)) {
        return -1;
    }
    return board->grid[row][col];
}

// 在指定位置落子，返回1成功，0失败
int board_place_piece(Board* board, int row, int col, int player) {
    if (!board_is_valid_pos(row, col)) {
        return 0;
    }
    if (board->grid[row][col] != 0) {
        return 0;  // 位置已有棋子
    }
    
    board->grid[row][col] = player;
    board->move_history[board->move_count][0] = row;
    board->move_history[board->move_count][1] = col;
    board->move_count++;
    
    return 1;
}

// 悔棋，撤回上一步，返回1成功，0失败
int board_undo(Board* board) {
    if (board->move_count <= 0) {
        return 0;  // 无棋可悔
    }
    
    board->move_count--;
    int row = board->move_history[board->move_count][0];
    int col = board->move_history[board->move_count][1];
    board->grid[row][col] = 0;
    board->move_history[board->move_count][0] = -1;
    board->move_history[board->move_count][1] = -1;
    
    // 切换回上一个玩家
    board->current_player = (board->current_player == PLAYER_BLACK) ? PLAYER_WHITE : PLAYER_BLACK;
    
    return 1;
}

// 从指定位置向指定方向统计连续棋子数
static int count_direction(Board* board, int row, int col, int dr, int dc) {
    int player = board->grid[row][col];
    if (player == 0) {
        return 0;
    }
    
    int count = 1;  // 包含当前位置
    
    // 正方向
    int r = row + dr;
    int c = col + dc;
    while (board_is_valid_pos(r, c) && board->grid[r][c] == player) {
        count++;
        r += dr;
        c += dc;
    }
    
    // 反方向
    r = row - dr;
    c = col - dc;
    while (board_is_valid_pos(r, c) && board->grid[r][c] == player) {
        count++;
        r -= dr;
        c -= dc;
    }
    
    return count;
}

// 判断是否有玩家获胜，返回获胜玩家编号，0表示无人获胜
int board_check_win(Board* board, int row, int col) {
    if (!board_is_valid_pos(row, col)) {
        return 0;
    }
    
    int player = board->grid[row][col];
    if (player == 0) {
        return 0;
    }
    
    // 四个方向：水平、垂直、左上-右下对角线、右上-左下对角线
    int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
    
    for (int i = 0; i < 4; i++) {
        int count = count_direction(board, row, col, directions[i][0], directions[i][1]);
        if (count >= 5) {
            return player;
        }
    }
    
    return 0;  // 无人获胜
}

// 判断棋盘是否已满（平局）
int board_is_full(Board* board) {
    return board->move_count >= BOARD_SIZE * BOARD_SIZE;
}

// 复制棋盘状态
void board_copy(Board* dest, Board* src) {
    memcpy(dest->grid, src->grid, sizeof(src->grid));
    dest->current_player = src->current_player;
    memcpy(dest->move_history, src->move_history, sizeof(src->move_history));
    dest->move_count = src->move_count;
}
