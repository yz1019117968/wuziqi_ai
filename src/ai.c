#include "ai.h"
#include "board.h"
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define MAX_DEPTH 4
#define MAX_CANDIDATES 225

static int ai_depth = MAX_DEPTH;
static int g_candidates[MAX_CANDIDATES][2];
static int g_candidate_scores[MAX_CANDIDATES];

void ai_set_depth(int depth) {
    if (depth >= 1 && depth <= 6) {
        ai_depth = depth;
    }
}

int ai_get_depth(void) {
    return ai_depth;
}

// 评估一个方向上的连续棋子数和开放端数
static void count_line(Board* board, int row, int col, int dr, int dc, int player,
                       int* out_count, int* out_open_ends) {
    int count = 1;
    int open_ends = 0;

    // 正方向
    int r = row + dr, c = col + dc;
    while (board_is_valid_pos(r, c) && board->grid[r][c] == player) {
        count++;
        r += dr; c += dc;
    }
    if (board_is_valid_pos(r, c) && board->grid[r][c] == 0) open_ends++;

    // 反方向
    r = row - dr; c = col - dc;
    while (board_is_valid_pos(r, c) && board->grid[r][c] == player) {
        count++;
        r -= dr; c -= dc;
    }
    if (board_is_valid_pos(r, c) && board->grid[r][c] == 0) open_ends++;

    *out_count = count;
    *out_open_ends = open_ends;
}

// 评估棋型得分
static int evaluate_pattern(int count, int open_ends) {
    if (count >= 5) return 100000;
    if (count == 4) return (open_ends == 2) ? 10000 : (open_ends == 1) ? 1000 : 0;
    if (count == 3) return (open_ends == 2) ? 1000  : (open_ends == 1) ? 100  : 0;
    if (count == 2) return (open_ends == 2) ? 100   : (open_ends == 1) ? 10   : 0;
    if (count == 1) return (open_ends == 2) ? 10    : (open_ends == 1) ? 1    : 0;
    return 0;
}

// 评估某个位置对指定玩家的价值（只看这个位置相关的4个方向）
static int evaluate_point(Board* board, int row, int col, int player) {
    int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
    int score = 0;
    // 临时放上棋子评估
    board->grid[row][col] = player;
    for (int d = 0; d < 4; d++) {
        int count, open_ends;
        count_line(board, row, col, directions[d][0], directions[d][1], player, &count, &open_ends);
        score += evaluate_pattern(count, open_ends);
    }
    board->grid[row][col] = 0;
    return score;
}

// 快速评估落子点的"好度"，用于候选排序
static int quick_score(Board* board, int row, int col, int ai_player) {
    int opponent = (ai_player == PLAYER_BLACK) ? PLAYER_WHITE : PLAYER_BLACK;
    int attack = evaluate_point(board, row, col, ai_player);
    int defend = evaluate_point(board, row, col, opponent);
    // 如果AI能在此五连，极高优先；如果对手能，也要高优先（堵）
    if (attack >= 100000) return 200000;
    if (defend >= 100000) return 150000;
    // 攻防兼顾
    return attack * 2 + defend;
}

// 候选位置排序（冒泡，候选数不大所以开销可接受）
static void sort_candidates(Board* board, int count, int ai_player) {
    for (int i = 0; i < count; i++) {
        g_candidate_scores[i] = quick_score(board, g_candidates[i][0], g_candidates[i][1], ai_player);
    }
    // 冒泡排序（按分数降序）
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            if (g_candidate_scores[j] < g_candidate_scores[j + 1]) {
                int ts = g_candidate_scores[j];
                g_candidate_scores[j] = g_candidate_scores[j + 1];
                g_candidate_scores[j + 1] = ts;
                int tr = g_candidates[j][0];
                int tc = g_candidates[j][1];
                g_candidates[j][0] = g_candidates[j + 1][0];
                g_candidates[j][1] = g_candidates[j + 1][1];
                g_candidates[j + 1][0] = tr;
                g_candidates[j + 1][1] = tc;
            }
        }
    }
}

// 获取所有可能的落子位置（已有棋子周围1格，候选更少）
static int get_candidate_moves(Board* board, int moves[][2], int max_moves) {
    int count = 0;
    char has_piece[BOARD_SIZE][BOARD_SIZE];
    memset(has_piece, 0, sizeof(has_piece));

    for (int i = 0; i < BOARD_SIZE && count < max_moves; i++) {
        for (int j = 0; j < BOARD_SIZE && count < max_moves; j++) {
            if (board->grid[i][j] != 0) {
                for (int di = -1; di <= 1 && count < max_moves; di++) {
                    for (int dj = -1; dj <= 1 && count < max_moves; dj++) {
                        int ni = i + di, nj = j + dj;
                        if (board_is_valid_pos(ni, nj) && board->grid[ni][nj] == 0 && !has_piece[ni][nj]) {
                            moves[count][0] = ni;
                            moves[count][1] = nj;
                            has_piece[ni][nj] = 1;
                            count++;
                        }
                    }
                }
            }
        }
    }

    if (count == 0) {
        moves[0][0] = BOARD_SIZE / 2;
        moves[0][1] = BOARD_SIZE / 2;
        count = 1;
    }
    return count;
}

// 增量式局面评估 —— 只评估落子位置周围，不全盘扫描
static int evaluate_player(Board* board, int player) {
    int total_score = 0;
    int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board->grid[i][j] == player) {
                for (int d = 0; d < 4; d++) {
                    int count, open_ends;
                    count_line(board, i, j, directions[d][0], directions[d][1], player, &count, &open_ends);
                    total_score += evaluate_pattern(count, open_ends);
                }
            }
        }
    }
    return total_score;
}

// 评估棋盘局面
int ai_evaluate(Board* board, int ai_player) {
    int opponent = (ai_player == PLAYER_BLACK) ? PLAYER_WHITE : PLAYER_BLACK;
    return evaluate_player(board, ai_player) - evaluate_player(board, opponent);
}

// 模拟落子
static void do_move(Board* board, int row, int col, int player) {
    board->grid[row][col] = player;
    board->move_history[board->move_count][0] = row;
    board->move_history[board->move_count][1] = col;
    board->move_count++;
}

// 撤销模拟落子
static void undo_move(Board* board, int row, int col) {
    board->move_count--;
    board->grid[row][col] = 0;
}

// Minimax + Alpha-Beta，使用候选排序提升剪枝效率
static int minimax(Board* board, int depth, int alpha, int beta, int is_maximizing, int ai_player) {
    int opponent = (ai_player == PLAYER_BLACK) ? PLAYER_WHITE : PLAYER_BLACK;

    // 获胜检测
    if (board->move_count > 0) {
        int last_row = board->move_history[board->move_count - 1][0];
        int last_col = board->move_history[board->move_count - 1][1];
        int winner = board_check_win(board, last_row, last_col);
        if (winner == ai_player) return 100000 + depth;
        if (winner == opponent) return -100000 - depth;
    }

    if (depth == 0 || board_is_full(board)) {
        return ai_evaluate(board, ai_player);
    }

    int candidate_count = get_candidate_moves(board, g_candidates, MAX_CANDIDATES);
    if (candidate_count == 0) return 0;

    // 对候选位置排序（启发式搜索，最大化剪枝效果）
    sort_candidates(board, candidate_count, ai_player);

    if (is_maximizing) {
        int max_eval = INT_MIN;
        for (int i = 0; i < candidate_count; i++) {
            int row = g_candidates[i][0];
            int col = g_candidates[i][1];

            do_move(board, row, col, ai_player);
            int eval = minimax(board, depth - 1, alpha, beta, 0, ai_player);
            undo_move(board, row, col);

            if (eval > max_eval) max_eval = eval;
            if (eval > alpha) alpha = eval;
            if (beta <= alpha) break;
        }
        return max_eval;
    } else {
        int min_eval = INT_MAX;
        for (int i = 0; i < candidate_count; i++) {
            int row = g_candidates[i][0];
            int col = g_candidates[i][1];

            do_move(board, row, col, opponent);
            int eval = minimax(board, depth - 1, alpha, beta, 1, ai_player);
            undo_move(board, row, col);

            if (eval < min_eval) min_eval = eval;
            if (eval < beta) beta = eval;
            if (beta <= alpha) break;
        }
        return min_eval;
    }
}

// AI执行落子
void ai_make_move(Board* board, int* row, int* col) {
    int ai_player = board->current_player;

    int candidate_count = get_candidate_moves(board, g_candidates, MAX_CANDIDATES);
    if (candidate_count == 0) return;

    // 对顶层候选排序
    sort_candidates(board, candidate_count, ai_player);

    int best_score = INT_MIN;
    int best_row = g_candidates[0][0];
    int best_col = g_candidates[0][1];

    for (int i = 0; i < candidate_count; i++) {
        int r = g_candidates[i][0];
        int c = g_candidates[i][1];

        do_move(board, r, c, ai_player);
        int score = minimax(board, ai_depth - 1, INT_MIN, INT_MAX, 0, ai_player);
        undo_move(board, r, c);

        if (score > best_score) {
            best_score = score;
            best_row = r;
            best_col = c;
        }
    }

    *row = best_row;
    *col = best_col;
}
