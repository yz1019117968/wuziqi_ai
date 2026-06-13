#ifndef EVENT_H
#define EVENT_H

#include "game.h"

// 处理SDL事件，返回0继续，1退出
int event_handle(GameContext* ctx, SDL_Event* event);

// 处理鼠标点击事件
void event_handle_click(GameContext* ctx, int mouse_x, int mouse_y);

// 将鼠标坐标转换为棋盘行列
int event_mouse_to_board(int mouse_x, int mouse_y, int* row, int* col);

// 检查按钮点击
int event_check_button_click(Button* btn, int mouse_x, int mouse_y);

// 处理菜单事件
int event_handle_menu(GameContext* ctx, SDL_Event* event);

#endif // EVENT_H
