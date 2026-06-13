#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H

#include "game.h"

// 保存棋局到文件，返回1成功，0失败
int game_save(GameContext* ctx, const char* filename);

// 从文件加载棋局，返回1成功，0失败
int game_load(GameContext* ctx, const char* filename);

// 生成默认保存文件名（带时间戳）
void game_get_default_save_path(char* buffer, int buffer_size);

// 扫描 saves 目录获取存档列表
int game_scan_saves(SaveFileList* list);

// 显示保存/加载结果的提示信息
void show_message_box(const char* title, const char* message);

#endif // SAVE_LOAD_H
