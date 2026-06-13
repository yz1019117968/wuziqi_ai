#include "render.h"
#include "board.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===== 颜色定义 - 深色木纹风格 =====
static const SDL_Color COLOR_BG_DARK      = {45,  30,  20,  255};   // 深棕背景
static const SDL_Color COLOR_BOARD_BG     = {218, 175, 100, 255};   // 木纹棋盘底色
static const SDL_Color COLOR_BOARD_EDGE   = {185, 145,  80, 255};   // 棋盘边缘深色
static const SDL_Color COLOR_LINE         = {80,  50,  20,  180};   // 网格线
static const SDL_Color COLOR_COORD_TEXT   = {60,  40,  20,  200};   // 坐标文字
static const SDL_Color COLOR_BLACK        = {30,  30,  30,  255};   // 黑子
static const SDL_Color COLOR_WHITE        = {240, 240, 240, 255};   // 白子
static const SDL_Color COLOR_BTN          = {180, 140,  70, 255};   // 金黄按钮
static const SDL_Color COLOR_BTN_HOVER    = {210, 170,  90, 255};   // 按钮悬停
static const SDL_Color COLOR_BTN_BORDER   = {150, 110,  50, 255};   // 按钮边框
static const SDL_Color COLOR_TEXT         = {240, 220, 180, 255};   // 浅色文字
static const SDL_Color COLOR_TEXT_LIGHT   = {255, 245, 230, 255};   // 标题白色
static const SDL_Color COLOR_STATUS_BG    = {60,  45,  30,  255};   // 状态栏背景
static const SDL_Color COLOR_STATUS_BORDER= {100,  75,  45,  255};  // 状态栏边框

// ===== 简易5x7位图字体 =====
// 支持 ASCII 32-126 的字符
static const unsigned char font_5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // 32 space
    {0x00,0x00,0x5F,0x00,0x00}, // 33 !
    {0x00,0x07,0x00,0x07,0x00}, // 34 "
    {0x14,0x7F,0x14,0x7F,0x14}, // 35 #
    {0x24,0x2A,0x7F,0x2A,0x12}, // 36 $
    {0x23,0x13,0x08,0x64,0x62}, // 37 %
    {0x36,0x49,0x55,0x22,0x50}, // 38 &
    {0x00,0x05,0x03,0x00,0x00}, // 39 '
    {0x00,0x1C,0x22,0x41,0x00}, // 40 (
    {0x00,0x41,0x22,0x1C,0x00}, // 41 )
    {0x08,0x2A,0x1C,0x2A,0x08}, // 42 *
    {0x08,0x08,0x3E,0x08,0x08}, // 43 +
    {0x00,0x50,0x30,0x00,0x00}, // 44 ,
    {0x08,0x08,0x08,0x08,0x08}, // 45 -
    {0x00,0x60,0x60,0x00,0x00}, // 46 .
    {0x20,0x10,0x08,0x04,0x02}, // 47 /
    {0x3E,0x51,0x49,0x45,0x3E}, // 48 0
    {0x00,0x42,0x7F,0x40,0x00}, // 49 1
    {0x42,0x61,0x51,0x49,0x46}, // 50 2
    {0x21,0x41,0x45,0x4B,0x31}, // 51 3
    {0x18,0x14,0x12,0x7F,0x10}, // 52 4
    {0x27,0x45,0x45,0x45,0x39}, // 53 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 54 6
    {0x01,0x71,0x09,0x05,0x03}, // 55 7
    {0x36,0x49,0x49,0x49,0x36}, // 56 8
    {0x06,0x49,0x49,0x29,0x1E}, // 57 9
    {0x00,0x36,0x36,0x00,0x00}, // 58 :
    {0x00,0x56,0x36,0x00,0x00}, // 59 ;
    {0x00,0x08,0x14,0x22,0x41}, // 60 <
    {0x14,0x14,0x14,0x14,0x14}, // 61 =
    {0x41,0x22,0x14,0x08,0x00}, // 62 >
    {0x02,0x01,0x51,0x09,0x06}, // 63 ?
    {0x32,0x49,0x79,0x41,0x3E}, // 64 @
    {0x7E,0x11,0x11,0x11,0x7E}, // 65 A
    {0x7F,0x49,0x49,0x49,0x36}, // 66 B
    {0x3E,0x41,0x41,0x41,0x22}, // 67 C
    {0x7F,0x41,0x41,0x22,0x1C}, // 68 D
    {0x7F,0x49,0x49,0x49,0x41}, // 69 E
    {0x7F,0x09,0x09,0x01,0x01}, // 70 F
    {0x3E,0x41,0x41,0x51,0x32}, // 71 G
    {0x7F,0x08,0x08,0x08,0x7F}, // 72 H
    {0x00,0x41,0x7F,0x41,0x00}, // 73 I
    {0x20,0x40,0x41,0x3F,0x01}, // 74 J
    {0x7F,0x08,0x14,0x22,0x41}, // 75 K
    {0x7F,0x40,0x40,0x40,0x40}, // 76 L
    {0x7F,0x02,0x04,0x02,0x7F}, // 77 M
    {0x7F,0x04,0x08,0x10,0x7F}, // 78 N
    {0x3E,0x41,0x41,0x41,0x3E}, // 79 O
    {0x7F,0x09,0x09,0x09,0x06}, // 80 P
    {0x3E,0x41,0x51,0x21,0x5E}, // 81 Q
    {0x7F,0x09,0x19,0x29,0x46}, // 82 R
    {0x46,0x49,0x49,0x49,0x31}, // 83 S
    {0x01,0x01,0x7F,0x01,0x01}, // 84 T
    {0x3F,0x40,0x40,0x40,0x3F}, // 85 U
    {0x1F,0x20,0x40,0x20,0x1F}, // 86 V
    {0x7F,0x20,0x18,0x20,0x7F}, // 87 W
    {0x63,0x14,0x08,0x14,0x63}, // 88 X
    {0x03,0x04,0x78,0x04,0x03}, // 89 Y
    {0x61,0x51,0x49,0x45,0x43}, // 90 Z
    {0x00,0x00,0x7F,0x41,0x41}, // 91 [
    {0x02,0x04,0x08,0x10,0x20}, // 92 backslash
    {0x41,0x41,0x7F,0x00,0x00}, // 93 ]
    {0x04,0x02,0x01,0x02,0x04}, // 94 ^
    {0x40,0x40,0x40,0x40,0x40}, // 95 _
    {0x00,0x01,0x02,0x04,0x00}, // 96 `
    {0x20,0x54,0x54,0x54,0x78}, // 97 a
    {0x7F,0x48,0x44,0x44,0x38}, // 98 b
    {0x38,0x44,0x44,0x44,0x20}, // 99 c
    {0x38,0x44,0x44,0x48,0x7F}, // 100 d
    {0x38,0x54,0x54,0x54,0x18}, // 101 e
    {0x08,0x7E,0x09,0x01,0x02}, // 102 f
    {0x08,0x14,0x54,0x54,0x3C}, // 103 g
    {0x7F,0x08,0x04,0x04,0x78}, // 104 h
    {0x00,0x44,0x7D,0x40,0x00}, // 105 i
    {0x20,0x40,0x44,0x3D,0x00}, // 106 j
    {0x00,0x7F,0x10,0x28,0x44}, // 107 k
    {0x00,0x41,0x7F,0x40,0x00}, // 108 l
    {0x7C,0x04,0x18,0x04,0x78}, // 109 m
    {0x7C,0x08,0x04,0x04,0x78}, // 110 n
    {0x38,0x44,0x44,0x44,0x38}, // 111 o
    {0x7C,0x14,0x14,0x14,0x08}, // 112 p
    {0x08,0x14,0x14,0x18,0x7C}, // 113 q
    {0x7C,0x08,0x04,0x04,0x08}, // 114 r
    {0x48,0x54,0x54,0x54,0x20}, // 115 s
    {0x04,0x3F,0x44,0x40,0x20}, // 116 t
    {0x3C,0x40,0x40,0x20,0x7C}, // 117 u
    {0x1C,0x20,0x40,0x20,0x1C}, // 118 v
    {0x3C,0x40,0x30,0x40,0x3C}, // 119 w
    {0x44,0x28,0x10,0x28,0x44}, // 120 x
    {0x0C,0x50,0x50,0x50,0x3C}, // 121 y
    {0x44,0x64,0x54,0x4C,0x44}, // 122 z
    {0x00,0x08,0x36,0x41,0x00}, // 123 {
    {0x00,0x00,0x7F,0x00,0x00}, // 124 |
    {0x00,0x41,0x36,0x08,0x00}, // 125 }
    {0x08,0x08,0x2A,0x1C,0x08}, // 126 ~
};

static void draw_char(SDL_Renderer* renderer, char c, int x, int y, SDL_Color color, float scale) {
    if (c < 32 || c > 126) return;
    const unsigned char* glyph = font_5x7[c - 32];
    int s = (int)(scale + 0.5f);
    if (s < 1) s = 1;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 5; col++) {
            if (glyph[col] & (1 << row)) {
                SDL_Rect pixel = {x + col * s, y + row * s, s, s};
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }
}

void draw_text(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color, float scale) {
    if (!text) return;
    int s = (int)(scale + 0.5f);
    if (s < 1) s = 1;
    int offset = 0;
    while (*text) {
        draw_char(renderer, *text, x + offset, y, color, scale);
        offset += 6 * s; // 字符间距
        text++;
    }
}

int get_text_width(const char* text, float scale) {
    if (!text) return 0;
    int s = (int)(scale + 0.5f);
    if (s < 1) s = 1;
    return (int)(strlen(text) * 6 * s);
}

// ===== 渲染初始化 =====
int render_init(GameContext* ctx) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 0;
    }

    ctx->window = SDL_CreateWindow("Wuziqi AI",
                                   SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED,
                                   WINDOW_WIDTH, WINDOW_HEIGHT,
                                   SDL_WINDOW_SHOWN);
    if (!ctx->window) {
        SDL_Quit();
        return 0;
    }

    ctx->renderer = SDL_CreateRenderer(ctx->window, -1,
                                       SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ctx->renderer) {
        SDL_DestroyWindow(ctx->window);
        SDL_Quit();
        return 0;
    }

    // 创建棋子纹理
    ctx->black_piece = create_piece_texture(ctx->renderer, 1);
    ctx->white_piece = create_piece_texture(ctx->renderer, 0);

    // 初始化按钮 —— 全部一字排列在底部
    int btn_y = WINDOW_HEIGHT - 52;
    int btn_h = 36;
    int gap = 12;

    // 计算各按钮宽度
    int w_pvp = 70;
    int w_pve = 70;
    int w_menu = 75;
    int w_restart = 85;
    int w_undo = 65;
    int w_save = 65;
    int w_load = 65;

    int total_w = w_pvp + gap + w_pve + gap + w_menu + gap + w_restart + gap + w_undo + gap + w_save + gap + w_load;
    int start_x = (WINDOW_WIDTH - total_w) / 2;
    int x = start_x;

    ctx->btn_mode_pvp.rect = (SDL_Rect){x, btn_y, w_pvp, btn_h};
    ctx->btn_mode_pvp.text = "PvP";
    ctx->btn_mode_pvp.is_hovered = 0;
    x += w_pvp + gap;

    ctx->btn_mode_pve.rect = (SDL_Rect){x, btn_y, w_pve, btn_h};
    ctx->btn_mode_pve.text = "PvE";
    ctx->btn_mode_pve.is_hovered = 0;
    x += w_pve + gap;

    ctx->btn_back_to_menu.rect = (SDL_Rect){x, btn_y, w_menu, btn_h};
    ctx->btn_back_to_menu.text = "Menu";
    ctx->btn_back_to_menu.is_hovered = 0;
    x += w_menu + gap;

    ctx->btn_new_game.rect = (SDL_Rect){x, btn_y, w_restart, btn_h};
    ctx->btn_new_game.text = "Restart";
    ctx->btn_new_game.is_hovered = 0;
    x += w_restart + gap;

    ctx->btn_undo.rect = (SDL_Rect){x, btn_y, w_undo, btn_h};
    ctx->btn_undo.text = "Undo";
    ctx->btn_undo.is_hovered = 0;
    x += w_undo + gap;

    ctx->btn_save.rect = (SDL_Rect){x, btn_y, w_save, btn_h};
    ctx->btn_save.text = "Save";
    ctx->btn_save.is_hovered = 0;
    x += w_save + gap;

    ctx->btn_load.rect = (SDL_Rect){x, btn_y, w_load, btn_h};
    ctx->btn_load.text = "Load";
    ctx->btn_load.is_hovered = 0;

    return 1;
}

void render_cleanup(GameContext* ctx) {
    if (ctx->black_piece) SDL_DestroyTexture(ctx->black_piece);
    if (ctx->white_piece) SDL_DestroyTexture(ctx->white_piece);
    if (ctx->board_bg) SDL_DestroyTexture(ctx->board_bg);
    if (ctx->renderer) SDL_DestroyRenderer(ctx->renderer);
    if (ctx->window) SDL_DestroyWindow(ctx->window);
    SDL_Quit();
}

// ===== 棋子纹理（带渐变光泽效果）=====
SDL_Texture* create_piece_texture(SDL_Renderer* renderer, int is_black) {
    int size = PIECE_RADIUS * 2 + 4;
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                             SDL_TEXTUREACCESS_TARGET, size, size);
    if (!texture) return NULL;

    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    int center = size / 2;
    int radius = PIECE_RADIUS;

    // 多层绘制，从外到内，产生立体感
    for (int r = radius; r >= 1; r--) {
        float ratio = (float)r / radius;
        
        SDL_Color c;
        if (is_black) {
            // 黑子: 外圈深黑，内圈稍亮
            int base = (int)(25 + (1 - ratio) * 35);
            c = (SDL_Color){base, base, base, 255};
        } else {
            // 白子: 外圈浅灰，内圈纯白
            int base = (int)(210 + (1 - ratio) * 45);
            c = (SDL_Color){base, base, base, 255};
        }

        SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
        for (int dy = -r; dy <= r; dy++) {
            for (int dx = -r; dx <= r; dx++) {
                if ((long)dx * dx + (long)dy * dy <= (long)r * r) {
                    SDL_RenderDrawPoint(renderer, center + dx, center + dy);
                }
            }
        }
    }

    // 高光效果 - 左上角
    int hl_radius = (is_black) ? 5 : 6;
    SDL_SetRenderDrawColor(renderer, is_black ? 120 : 255, is_black ? 120 : 255, is_black ? 120 : 255,
                           is_black ? 100 : 160);
    for (int dy = -hl_radius; dy <= -2; dy++) {
        for (int dx = -hl_radius; dx <= -2; dx++) {
            if ((long)dx * dx + (long)dy * dy <= (long)hl_radius * hl_radius) {
                SDL_RenderDrawPoint(renderer, center + dx, center + dy);
            }
        }
    }

    SDL_SetRenderTarget(renderer, NULL);
    return texture;
}

// ===== 圆角矩形填充 =====
void draw_rounded_rect(SDL_Renderer* renderer, SDL_Rect* rect, int r, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // 中间矩形区域
    SDL_Rect inner;
    inner = (SDL_Rect){rect->x + r, rect->y, rect->w - 2 * r, rect->h};
    SDL_RenderFillRect(renderer, &inner);
    inner = (SDL_Rect){rect->x, rect->y + r, rect->w, rect->h - 2 * r};
    SDL_RenderFillRect(renderer, &inner);

    // 四个圆角
    for (int angle = 0; angle <= 90; angle++) {
        float rad = (float)angle * 3.14159f / 180.0f;
        int dx = (int)((float)r * cosf(rad));
        int dy = (int)((float)r * sinf(rad));

        SDL_RenderDrawPoint(renderer, rect->x + r - dx, rect->y + r - dy);
        SDL_RenderDrawPoint(renderer, rect->x + rect->w - r + dx, rect->y + r - dy);
        SDL_RenderDrawPoint(renderer, rect->x + r - dx, rect->y + rect->h - r + dy);
        SDL_RenderDrawPoint(renderer, rect->x + rect->w - r + dx, rect->y + rect->h - r + dy);
    }
}

// ===== 圆角矩形描边 =====
static void draw_rounded_rect_outline(SDL_Renderer* renderer, SDL_Rect* rect, int r, int thickness, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // 四条边的直线部分
    // 上边
    for (int i = 0; i < thickness; i++)
        SDL_RenderDrawLine(renderer, rect->x + r, rect->y + i, rect->x + rect->w - r - 1, rect->y + i);
    // 下边
    for (int i = 0; i < thickness; i++)
        SDL_RenderDrawLine(renderer, rect->x + r, rect->y + rect->h - 1 - i, rect->x + rect->w - r - 1, rect->y + rect->h - 1 - i);
    // 左边
    for (int i = 0; i < thickness; i++)
        SDL_RenderDrawLine(renderer, rect->x + i, rect->y + r, rect->x + i, rect->y + rect->h - r - 1);
    // 右边
    for (int i = 0; i < thickness; i++)
        SDL_RenderDrawLine(renderer, rect->x + rect->w - 1 - i, rect->y + r, rect->x + rect->w - 1 - i, rect->y + rect->h - r - 1);

    // 四个圆角
    for (int t = 0; t < thickness; t++) {
        for (int angle = 0; angle <= 90; angle++) {
            float rad = (float)angle * 3.14159f / 180.0f;
            int rr = r - t;
            if (rr < 0) rr = 0;
            int dx = (int)((float)rr * cosf(rad));
            int dy = (int)((float)rr * sinf(rad));

            SDL_RenderDrawPoint(renderer, rect->x + r - dx, rect->y + r - dy);
            SDL_RenderDrawPoint(renderer, rect->x + rect->w - r + dx, rect->y + r - dy);
            SDL_RenderDrawPoint(renderer, rect->x + r - dx, rect->y + rect->h - r + dy);
            SDL_RenderDrawPoint(renderer, rect->x + rect->w - r + dx, rect->y + rect->h - r + dy);
        }
    }
}

// ===== 棋盘渲染 - 木纹风格 =====
void render_board(GameContext* ctx) {
    // ---- 整体深色背景 ----
    SDL_SetRenderDrawColor(ctx->renderer, COLOR_BG_DARK.r, COLOR_BG_DARK.g, COLOR_BG_DARK.b, COLOR_BG_DARK.a);
    SDL_RenderClear(ctx->renderer);

    // ---- 计算棋盘位置 ----
    int board_pixel_size = (BOARD_SIZE - 1) * CELL_SIZE;
    int coord_space = 18; // 坐标标注空间
    int board_x = (WINDOW_WIDTH - board_pixel_size) / 2;
    int board_y = MARGIN + coord_space + 5; // 预留给标题区

    // ---- 木纹棋盘底板（带圆角）----
    int padding = 28; // 棋盘网格到木板的边缘距离
    SDL_Rect board_bg_rect = {
        board_x - padding,
        board_y - padding,
        board_pixel_size + padding * 2,
        board_pixel_size + padding * 2
    };
    
    // 木板主体
    draw_rounded_rect(ctx->renderer, &board_bg_rect, 12, COLOR_BOARD_BG);
    // 木板边缘阴影效果（内嵌）
    draw_rounded_rect_outline(ctx->renderer, &board_bg_rect, 12, 2, COLOR_BOARD_EDGE);

    // ---- 坐标标注 ----
    // 列标 A-O (顶部)
    for (int i = 0; i < BOARD_SIZE; i++) {
        char label[2] = {'A' + i, '\0'};
        int lx = board_x + i * CELL_SIZE - get_text_width(label, 1.0f) / 2;
        draw_text(ctx->renderer, label, lx, board_y - padding + 6, COLOR_COORD_TEXT, 1.0f);
    }
    // 行标 1-15 (左侧)
    for (int i = 0; i < BOARD_SIZE; i++) {
        char label[4];
        snprintf(label, sizeof(label), "%d", BOARD_SIZE - i);
        int ly = board_y + i * CELL_SIZE - 3;
        draw_text(ctx->renderer, label, board_x - padding + 6, ly, COLOR_COORD_TEXT, 1.0f);
    }

    // ---- 网格线 ----
    SDL_SetRenderDrawColor(ctx->renderer, COLOR_LINE.r, COLOR_LINE.g, COLOR_LINE.b, COLOR_LINE.a);
    for (int i = 0; i < BOARD_SIZE; i++) {
        // 竖线
        SDL_RenderDrawLine(ctx->renderer,
                          board_x + i * CELL_SIZE, board_y,
                          board_x + i * CELL_SIZE, board_y + board_pixel_size);
        // 横线
        SDL_RenderDrawLine(ctx->renderer,
                          board_x, board_y + i * CELL_SIZE,
                          board_x + board_pixel_size, board_y + i * CELL_SIZE);
    }

    // ---- 星位（天元和四个角星）----
    int star_points[5][2] = {{3, 3}, {3, 11}, {7, 7}, {11, 3}, {11, 11}};
    SDL_SetRenderDrawColor(ctx->renderer, COLOR_LINE.r, COLOR_LINE.g, COLOR_LINE.b, 255);
    for (int i = 0; i < 5; i++) {
        int sx = board_x + star_points[i][0] * CELL_SIZE;
        int sy = board_y + star_points[i][1] * CELL_SIZE;
        // 绘制实心圆点作为星位
        for (int dy = -3; dy <= 3; dy++) {
            for (int dx = -3; dx <= 3; dx++) {
                if (dx * dx + dy * dy <= 9) {
                    SDL_RenderDrawPoint(ctx->renderer, sx + dx, sy + dy);
                }
            }
        }
    }
}

// ===== 棋子渲染 =====
void render_pieces(GameContext* ctx) {
    int board_pixel_size = (BOARD_SIZE - 1) * CELL_SIZE;
    int board_x = (WINDOW_WIDTH - board_pixel_size) / 2;
    int coord_space = 18;
    int board_y = MARGIN + coord_space + 5;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int piece = ctx->board.grid[i][j];
            if (piece == 0) continue;

            int px = board_x + j * CELL_SIZE;
            int py = board_y + i * CELL_SIZE;

            SDL_Rect dst = {px - PIECE_RADIUS - 2, py - PIECE_RADIUS - 2,
                            PIECE_RADIUS * 2 + 4, PIECE_RADIUS * 2 + 4};
            SDL_Texture* tex = (piece == PLAYER_BLACK) ? ctx->black_piece : ctx->white_piece;
            if (tex) {
                SDL_RenderCopy(ctx->renderer, tex, NULL, &dst);
            }

            // 最后落子标记（红色小圆点）
            if (ctx->board.move_count > 0 &&
                ctx->board.move_history[ctx->board.move_count - 1][0] == i &&
                ctx->board.move_history[ctx->board.move_count - 1][1] == j) {
                SDL_SetRenderDrawColor(ctx->renderer, 220, 50, 50, 200);
                for (int dy = -3; dy <= 3; dy++) {
                    for (int dx = -3; dx <= 3; dx++) {
                        if (dx * dx + dy * dy <= 9) {
                            SDL_RenderDrawPoint(ctx->renderer, px + dx, py + dy);
                        }
                    }
                }
            }
        }
    }
}

// ===== 按钮绘制（金黄色圆角按钮风格）=====
void draw_button(SDL_Renderer* renderer, Button* btn) {
    SDL_Color bg_color = btn->is_hovered ? COLOR_BTN_HOVER : COLOR_BTN;
    
    // 按钮背景
    draw_rounded_rect(renderer, &btn->rect, 8, bg_color);
    
    // 按钮边框
    draw_rounded_rect_outline(renderer, &btn->rect, 8, 1, COLOR_BTN_BORDER);

    if (btn->text) {
        float text_scale = (btn->rect.h >= 45) ? 2.5f : ((btn->rect.h >= 35) ? 1.6f : 1.2f);
        int tw = get_text_width(btn->text, text_scale);
        int tx = btn->rect.x + (btn->rect.w - tw) / 2;
        int ty = btn->rect.y + (btn->rect.h - 7 * (int)(text_scale + 0.5f)) / 2;
        // 深色文字
        draw_text(renderer, btn->text, tx, ty, (SDL_Color){40, 25, 10, 255}, text_scale);
    }
}

// ===== UI按钮 =====
void render_buttons(GameContext* ctx) {
    draw_button(ctx->renderer, &ctx->btn_new_game);
    draw_button(ctx->renderer, &ctx->btn_undo);
    draw_button(ctx->renderer, &ctx->btn_save);
    draw_button(ctx->renderer, &ctx->btn_load);
    draw_button(ctx->renderer, &ctx->btn_mode_pvp);
    draw_button(ctx->renderer, &ctx->btn_mode_pve);
    draw_button(ctx->renderer, &ctx->btn_back_to_menu);
}

// ===== 状态信息栏（胶囊形）=====
void render_status(GameContext* ctx) {
    int status_y = MARGIN - 2;
    
    // 状态栏背景（圆角胶囊形）
    SDL_Rect status_bar = {WINDOW_WIDTH/2 - 140, status_y, 280, 34};
    draw_rounded_rect(ctx->renderer, &status_bar, 17, COLOR_STATUS_BG);
    draw_rounded_rect_outline(ctx->renderer, &status_bar, 17, 1, COLOR_STATUS_BORDER);

    // 当前玩家信息
    int text_x = status_bar.x + 15;
    int text_y = status_bar.y + 11;

    // 绘制一个小棋子图标
    int icon_r = 8;
    SDL_SetRenderDrawColor(ctx->renderer, COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b, COLOR_BLACK.a);
    if (ctx->state != STATE_WIN && ctx->state != STATE_DRAW) {
        if (ctx->board.current_player == PLAYER_WHITE) {
            SDL_SetRenderDrawColor(ctx->renderer, 220, 220, 220, 255);
        }
        for (int dy = -icon_r; dy <= icon_r; dy++) {
            for (int dx = -icon_r; dx <= icon_r; dx++) {
                if (dx * dx + dy * dy <= icon_r * icon_r) {
                    SDL_RenderDrawPoint(ctx->renderer, text_x + icon_r + dx, text_y - 1 + dy);
                }
            }
        }
    }

    // 文本信息
    text_x += icon_r * 2 + 10;

    if (ctx->state == STATE_WIN) {
        const char* winner_text = (ctx->winner == PLAYER_BLACK) ? "Black Win!" : "White Win!";
        draw_text(ctx->renderer, winner_text, text_x, text_y, COLOR_TEXT, 1.4f);
    } else if (ctx->state == STATE_DRAW) {
        draw_text(ctx->renderer, "Draw!", text_x, text_y, COLOR_TEXT, 1.4f);
    } else {
        // 显示当前落子和手数
        const char* player_text = (ctx->board.current_player == PLAYER_BLACK) ? "Black" : "White";
        draw_text(ctx->renderer, player_text, text_x, text_y, COLOR_TEXT, 1.4f);
        text_x += get_text_width(player_text, 1.4f) + 8;
        
        // 手数
        char move_str[16];
        snprintf(move_str, sizeof(move_str), "#%d", ctx->board.move_count);
        draw_text(ctx->renderer, move_str, text_x, text_y, (SDL_Color){160, 140, 100, 255}, 1.4f);

        if (ctx->mode == MODE_PVE && ctx->board.current_player == PLAYER_WHITE) {
            int ai_w = get_text_width("AI...", 1.4f);
            draw_text(ctx->renderer, "AI...", WINDOW_WIDTH/2 - ai_w/2, status_bar.y + 11, (SDL_Color){200, 170, 100, 255}, 1.4f);
        }
    }

    // 更新窗口标题
    if (ctx->state == STATE_WIN) {
        SDL_SetWindowTitle(ctx->window, (ctx->winner == PLAYER_BLACK) ? "Wuziqi AI - Black Win!" : "Wuziqi AI - White Win!");
    } else if (ctx->state == STATE_DRAW) {
        SDL_SetWindowTitle(ctx->window, "Wuziqi AI - Draw!");
    } else {
        const char* turn = (ctx->board.current_player == PLAYER_BLACK) ? "Black" : "White";
        char title[64];
        snprintf(title, sizeof(title), "Wuziqi AI - %s's Turn", turn);
        SDL_SetWindowTitle(ctx->window, title);
    }
}

// ===== 标题渲染（WUZIQI AI）=====
static void render_title(SDL_Renderer* renderer) {
    int title_y = 12;

    // 主标题 "WUZIQI AI"
    const char* title = "WUZIQI AI";
    int tw = get_text_width(title, 3.2f);
    draw_text(renderer, title, WINDOW_WIDTH/2 - tw/2, title_y, COLOR_TEXT_LIGHT, 3.2f);

    // 副标题 "GOMOKU"
    const char* subtitle = "GOMOKU";
    int sw = get_text_width(subtitle, 1.5f);
    draw_text(renderer, subtitle, WINDOW_WIDTH/2 - sw/2, title_y + 36, (SDL_Color){180, 155, 110, 255}, 1.5f);
}

// ===== 主菜单 =====
void render_menu(GameContext* ctx) {
    // 深色背景
    SDL_SetRenderDrawColor(ctx->renderer, COLOR_BG_DARK.r, COLOR_BG_DARK.g, COLOR_BG_DARK.b, COLOR_BG_DARK.a);
    SDL_RenderClear(ctx->renderer);

    // 标题
    int title_y = 100;

    // 主标题 "WUZIQI AI"
    const char* title = "WUZIQI AI";
    int title_w = get_text_width(title, 4.0f);
    draw_text(ctx->renderer, title, WINDOW_WIDTH/2 - title_w/2, title_y, COLOR_TEXT_LIGHT, 4.0f);

    // 副标题 "GOMOKU"
    const char* subtitle = "GOMOKU";
    int sub_w = get_text_width(subtitle, 2.0f);
    draw_text(ctx->renderer, subtitle, WINDOW_WIDTH/2 - sub_w/2, title_y + 42, (SDL_Color){180, 155, 110, 255}, 2.0f);

    // 菜单按钮
    Button start_pvp = {{WINDOW_WIDTH / 2 - 150, 250, 300, 52}, "Player vs Player", 0};
    Button start_pve = {{WINDOW_WIDTH / 2 - 150, 320, 300, 52}, "Player vs AI", 0};
    draw_button(ctx->renderer, &start_pvp);
    draw_button(ctx->renderer, &start_pve);
}

// ===== 存档加载界面 =====
void render_load_screen(GameContext* ctx) {
    SDL_SetRenderDrawColor(ctx->renderer, COLOR_BG_DARK.r, COLOR_BG_DARK.g, COLOR_BG_DARK.b, COLOR_BG_DARK.a);
    SDL_RenderClear(ctx->renderer);

    // 标题
    const char* title = "Load Saved Game";
    int tw = get_text_width(title, 2.5f);
    draw_text(ctx->renderer, title, WINDOW_WIDTH / 2 - tw / 2, 20, COLOR_TEXT_LIGHT, 2.5f);

    if (ctx->save_list.count == 0) {
        const char* empty = "No saved games found.";
        int ew = get_text_width(empty, 2.0f);
        draw_text(ctx->renderer, empty, WINDOW_WIDTH / 2 - ew / 2, 200, COLOR_TEXT, 2.0f);
    } else {
        // 表头
        draw_text(ctx->renderer, "Time", 50, 65, COLOR_TEXT, 1.5f);
        draw_text(ctx->renderer, "Mode", 330, 65, COLOR_TEXT, 1.5f);
        draw_text(ctx->renderer, "Moves", 430, 65, COLOR_TEXT, 1.5f);
        draw_text(ctx->renderer, "Status", 530, 65, COLOR_TEXT, 1.5f);

        // 分隔线
        SDL_SetRenderDrawColor(ctx->renderer, 100, 75, 45, 255);
        SDL_RenderDrawLine(ctx->renderer, 50, 85, WINDOW_WIDTH - 50, 85);

        // 列表项
        int item_h = 40;
        int start_y = 95;
        int max_visible = 12;

        for (int i = ctx->save_list.scroll_offset;
             i < ctx->save_list.count && (i - ctx->save_list.scroll_offset) < max_visible;
             i++) {
            SaveFileInfo* info = &ctx->save_list.files[i];
            int y = start_y + (i - ctx->save_list.scroll_offset) * item_h;

            int is_selected = (i == ctx->save_list.selected_index);
            SDL_Color bg = is_selected ?
                (SDL_Color){180, 140, 70, 255} : (SDL_Color){70, 50, 30, 200};
            SDL_Rect item_bg = {45, y, WINDOW_WIDTH - 90, item_h - 2};
            SDL_SetRenderDrawColor(ctx->renderer, bg.r, bg.g, bg.b, bg.a);
            SDL_RenderFillRect(ctx->renderer, &item_bg);

            draw_text(ctx->renderer, info->display_time, 50, y + 8, COLOR_TEXT, 1.2f);
            draw_text(ctx->renderer, info->mode_text, 330, y + 8, COLOR_TEXT, 1.2f);

            char moves_str[16];
            snprintf(moves_str, sizeof(moves_str), "%d", info->move_count);
            draw_text(ctx->renderer, moves_str, 430, y + 8, COLOR_TEXT, 1.2f);

            const char* status_str = (info->game_state == STATE_WIN) ? "Win" :
                                     (info->game_state == STATE_DRAW) ? "Draw" : "Playing";
            draw_text(ctx->renderer, status_str, 530, y + 8, COLOR_TEXT, 1.2f);
        }

        // 滚动提示
        if (ctx->save_list.count > max_visible) {
            char hint[64];
            snprintf(hint, sizeof(hint), "%d/%d saves (scroll with mouse wheel)",
                     ctx->save_list.count, ctx->save_list.count);
            int hw = get_text_width(hint, 1.0f);
            draw_text(ctx->renderer, hint, WINDOW_WIDTH / 2 - hw / 2, WINDOW_HEIGHT - 80, COLOR_TEXT, 1.0f);
        }
    }

    // 底部按钮
    Button btn_confirm = {{WINDOW_WIDTH / 2 - 110, WINDOW_HEIGHT - 55, 100, 35}, "Load", 0};
    Button btn_cancel = {{WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT - 55, 100, 35}, "Cancel", 0};
    draw_button(ctx->renderer, &btn_confirm);
    draw_button(ctx->renderer, &btn_cancel);
}

// ===== 主渲染入口 =====
void render_game(GameContext* ctx) {
    if (ctx->state == STATE_MENU) {
        render_menu(ctx);
    } else if (ctx->state == STATE_LOAD) {
        render_load_screen(ctx);
    } else {
        // 清屏（已在render_board中处理背景）
        SDL_SetRenderDrawColor(ctx->renderer, COLOR_BG_DARK.r, COLOR_BG_DARK.g, COLOR_BG_DARK.b, 255);
        SDL_RenderClear(ctx->renderer);

        // 标题
        render_title(ctx->renderer);

        // 状态栏
        render_status(ctx);

        // 棋盘
        render_board(ctx);

        // 棋子
        render_pieces(ctx);

        // 按钮
        render_buttons(ctx);

        // ===== 结算画面 =====
        if (ctx->state == STATE_WIN || ctx->state == STATE_DRAW) {
            // 半透明遮罩
            SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 0, 170);
            SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
            SDL_RenderFillRect(ctx->renderer, &overlay);
            SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_NONE);

            // 结算面板
            int panel_w = 480, panel_h = 400;
            int panel_x = WINDOW_WIDTH / 2 - panel_w / 2;
            int panel_y = WINDOW_HEIGHT / 2 - panel_h / 2;
            SDL_Rect panel = {panel_x, panel_y, panel_w, panel_h};
            draw_rounded_rect(ctx->renderer, &panel, 16, COLOR_STATUS_BG);
            draw_rounded_rect_outline(ctx->renderer, &panel, 16, 3, COLOR_BTN_BORDER);

            // 标题
            const char* title = "GAME OVER";
            int ttw = get_text_width(title, 2.8f);
            draw_text(ctx->renderer, title, WINDOW_WIDTH / 2 - ttw / 2, panel_y + 18, COLOR_TEXT_LIGHT, 2.8f);

            // 标题分隔线
            SDL_SetRenderDrawColor(ctx->renderer, 140, 105, 55, 200);
            SDL_RenderDrawLine(ctx->renderer, panel_x + 50, panel_y + 60,
                               panel_x + panel_w - 50, panel_y + 60);

            // 胜负结果（大字）
            const char* result_text;
            if (ctx->state == STATE_WIN) {
                result_text = (ctx->winner == PLAYER_BLACK) ? "Black Wins!" : "White Wins!";
            } else {
                result_text = "Draw!";
            }
            int rtw = get_text_width(result_text, 4.0f);
            draw_text(ctx->renderer, result_text, WINDOW_WIDTH / 2 - rtw / 2,
                     panel_y + 78, COLOR_TEXT_LIGHT, 4.0f);

            // 信息分隔线
            int info_sep_y = panel_y + 132;
            SDL_SetRenderDrawColor(ctx->renderer, 140, 105, 55, 200);
            SDL_RenderDrawLine(ctx->renderer, panel_x + 50, info_sep_y,
                               panel_x + panel_w - 50, info_sep_y);

            // 统计信息
            const char* mode_name = (ctx->mode == MODE_PVP) ? "Player vs Player" : "Player vs AI";

            int black_cnt = 0, white_cnt = 0;
            for (int r = 0; r < BOARD_SIZE; r++) {
                for (int c = 0; c < BOARD_SIZE; c++) {
                    if (ctx->board.grid[r][c] == PLAYER_BLACK) black_cnt++;
                    else if (ctx->board.grid[r][c] == PLAYER_WHITE) white_cnt++;
                }
            }

            int last_r = -1, last_c = -1;
            if (ctx->board.move_count > 0) {
                last_r = ctx->board.move_history[ctx->board.move_count - 1][0];
                last_c = ctx->board.move_history[ctx->board.move_count - 1][1];
            }

            int info_y = info_sep_y + 20;
            int info_gap = 36;
            char buf[128];

            // 模式
            snprintf(buf, sizeof(buf), "Mode:     %s", mode_name);
            draw_text(ctx->renderer, buf, panel_x + 50, info_y, COLOR_TEXT, 1.6f);
            info_y += info_gap;

            // 胜负方
            if (ctx->state == STATE_WIN) {
                const char* wname = (ctx->winner == PLAYER_BLACK) ? "Black" : "White";
                snprintf(buf, sizeof(buf), "Winner:   %s", wname);
            } else {
                snprintf(buf, sizeof(buf), "Result:   Draw");
            }
            draw_text(ctx->renderer, buf, panel_x + 50, info_y, COLOR_TEXT, 1.6f);
            info_y += info_gap;

            // 总步数
            snprintf(buf, sizeof(buf), "Moves:    %d", ctx->board.move_count);
            draw_text(ctx->renderer, buf, panel_x + 50, info_y, COLOR_TEXT, 1.6f);
            info_y += info_gap;

            // 黑白子统计
            snprintf(buf, sizeof(buf), "Pieces:   Black %d  /  White %d", black_cnt, white_cnt);
            draw_text(ctx->renderer, buf, panel_x + 50, info_y, COLOR_TEXT, 1.6f);
            info_y += info_gap;

            // 最后一步
            if (last_r >= 0) {
                snprintf(buf, sizeof(buf), "Final:    (%d, %d)", last_c + 1, last_r + 1);
                draw_text(ctx->renderer, buf, panel_x + 50, info_y, COLOR_TEXT, 1.6f);
            }
            info_y += info_gap;

            // 按钮分隔线
            SDL_SetRenderDrawColor(ctx->renderer, 140, 105, 55, 200);
            SDL_RenderDrawLine(ctx->renderer, panel_x + 50, info_y,
                               panel_x + panel_w - 50, info_y);

            // 底部按钮（坐标与 event.c 保持一致）
            int btn_y = info_y + 14;
            Button btn_new_game = {{panel_x + 30, btn_y, 130, 38}, "New Game", 0};
            Button btn_back_menu = {{panel_x + panel_w - 30 - 130, btn_y, 130, 38}, "Main Menu", 0};
            draw_button(ctx->renderer, &btn_new_game);
            draw_button(ctx->renderer, &btn_back_menu);
        }
    }

    SDL_RenderPresent(ctx->renderer);
}
