#ifndef RENDER_H
#define RENDER_H

#include "game.h"

// 初始化SDL和窗口
int render_init(GameContext* ctx);

// 清理SDL资源
void render_cleanup(GameContext* ctx);

// 渲染整个游戏画面
void render_game(GameContext* ctx);

// 渲染主菜单
void render_menu(GameContext* ctx);

// 渲染存档加载界面
void render_load_screen(GameContext* ctx);

// 渲染棋盘
void render_board(GameContext* ctx);

// 渲染棋子
void render_pieces(GameContext* ctx);

// 渲染UI按钮
void render_buttons(GameContext* ctx);

// 渲染游戏状态信息
void render_status(GameContext* ctx);

// 创建棋子纹理
SDL_Texture* create_piece_texture(SDL_Renderer* renderer, int is_black);

// 绘制圆角矩形
void draw_rounded_rect(SDL_Renderer* renderer, SDL_Rect* rect, int r, SDL_Color color);

// 绘制按钮（纯SDL2实现，无TTF依赖）
void draw_button(SDL_Renderer* renderer, Button* btn);

// 绘制文字（简单内嵌位图字体）
void draw_text(SDL_Renderer* renderer, const char* text, int x, int y, SDL_Color color, float scale);

// 获取文字宽度
int get_text_width(const char* text, float scale);

#endif // RENDER_H
