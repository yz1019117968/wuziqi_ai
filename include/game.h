#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

// 游戏常量
#define BOARD_SIZE 15
#define CELL_SIZE 38
#define MARGIN 50       // 棋盘边距（含坐标标注空间）
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 780
#define PIECE_RADIUS 17

// 游戏模式
typedef enum {
    MODE_PVP,
    MODE_PVE
} GameMode;

// 游戏状态
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_WIN,
    STATE_DRAW,
    STATE_LOAD
} GameState;

// 存档文件信息
#define MAX_SAVE_FILES 50
typedef struct {
    char filename[256];      // 完整文件名
    char display_time[32];   // 显示用的时间字符串
    char mode_text[16];      // PvP / PvE
    int move_count;          // 落子步数
    int game_state;          // 存档时的游戏状态
} SaveFileInfo;

// 存档列表
typedef struct {
    SaveFileInfo files[MAX_SAVE_FILES];
    int count;
    int scroll_offset;       // 滚动偏移
    int selected_index;      // 当前选中的存档索引（-1 表示无选中）
} SaveFileList;

// 玩家颜色
#define PLAYER_BLACK 1
#define PLAYER_WHITE 2

// 棋盘结构体
typedef struct {
    int grid[BOARD_SIZE][BOARD_SIZE];
    int current_player;
    int move_history[BOARD_SIZE * BOARD_SIZE][2];
    int move_count;
} Board;

// 按钮结构体
typedef struct {
    SDL_Rect rect;
    const char* text;
    int is_hovered;
} Button;

// 游戏上下文结构体
typedef struct {
    Board board;
    GameMode mode;
    GameState state;
    int winner;
    int ai_thinking;

    // SDL 渲染相关
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* black_piece;
    SDL_Texture* white_piece;
    SDL_Texture* board_bg;

    // UI 按钮
    Button btn_new_game;
    Button btn_undo;
    Button btn_save;
    Button btn_load;
    Button btn_mode_pvp;
    Button btn_mode_pve;
    Button btn_back_to_menu;

    // 存档列表
    SaveFileList save_list;
} GameContext;

// 初始化游戏
void game_init(GameContext* ctx);
void game_cleanup(GameContext* ctx);
void game_reset(GameContext* ctx);

// 切换玩家
void switch_player(Board* board);

#endif // GAME_H
