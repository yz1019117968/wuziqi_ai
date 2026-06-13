#include "save_load.h"
#include "game.h"
#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#include <windows.h>
#endif

// 获取带时间戳的保存路径
void game_get_default_save_path(char* buffer, int buffer_size) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    snprintf(buffer, buffer_size, "saves/save_%04d%02d%02d_%02d%02d%02d.txt",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
}

// 确保 saves 目录存在
static void ensure_saves_dir(void) {
    #ifdef _WIN32
        mkdir("saves");
    #else
        mkdir("saves", 0755);
    #endif
}

// 从存档文件中读取元数据（不完整加载）
static int read_save_metadata(const char* filepath, SaveFileInfo* info) {
    FILE* file = fopen(filepath, "r");
    if (!file) return 0;

    char line[256];
    info->move_count = 0;
    info->game_state = STATE_PLAYING;
    strcpy(info->mode_text, "?");

    while (fgets(line, sizeof(line), file)) {
        int len = (int)strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
            len--;
        }

        char* key = strtok(line, "=");
        char* value = strtok(NULL, "=");
        if (key && value) {
            if (strcmp(key, "MODE") == 0) {
                int m = atoi(value);
                strcpy(info->mode_text, (m == MODE_PVE) ? "PvE" : "PvP");
            } else if (strcmp(key, "STATE") == 0) {
                info->game_state = atoi(value);
            } else if (strcmp(key, "MOVE_COUNT") == 0) {
                info->move_count = atoi(value);
            } else if (strcmp(key, "SAVE_TIME") == 0) {
                // 将 YYYYMMDD_HHMMSS 格式转换为可读格式
                char time_raw[32];
                strcpy(time_raw, value);
                if (strlen(time_raw) >= 15) {
                    snprintf(info->display_time, sizeof(info->display_time),
                             "%c%c%c%c-%c%c-%c%c %c%c:%c%c:%c%c",
                             time_raw[0], time_raw[1], time_raw[2], time_raw[3],
                             time_raw[4], time_raw[5],
                             time_raw[6], time_raw[7],
                             time_raw[9], time_raw[10],
                             time_raw[11], time_raw[12],
                             time_raw[13], time_raw[14]);
                }
            }
        }
    }

    fclose(file);
    return 1;
}

// 扫描 saves 目录获取存档列表
int game_scan_saves(SaveFileList* list) {
    list->count = 0;
    list->scroll_offset = 0;
    list->selected_index = 0;  // 默认选中第一项

    ensure_saves_dir();

    #ifdef _WIN32
        WIN32_FIND_DATA find_data;
        HANDLE hFind;
        char pattern[256];
        snprintf(pattern, sizeof(pattern), "saves\\save_*.txt");
        hFind = FindFirstFile(pattern, &find_data);
        if (hFind == INVALID_HANDLE_VALUE) {
            return 0; // 没有存档文件
        }

        do {
            if (list->count >= MAX_SAVE_FILES) break;
            SaveFileInfo* info = &list->files[list->count];
            snprintf(info->filename, sizeof(info->filename), "saves/%s", find_data.cFileName);
            // 从文件名提取时间
            // 格式: save_YYYYMMDD_HHMMSS.txt
            char name_only[256];
            strcpy(name_only, find_data.cFileName);
            // 去掉 .txt 后缀
            char* dot = strrchr(name_only, '.');
            if (dot) *dot = '\0';
            // 去掉 "save_" 前缀
            char* time_part = name_only + 5;
            // 转换为可读格式
            if (strlen(time_part) >= 15) {
                snprintf(info->display_time, sizeof(info->display_time),
                         "%c%c%c%c-%c%c-%c%c %c%c:%c%c:%c%c",
                         time_part[0], time_part[1], time_part[2], time_part[3],
                         time_part[4], time_part[5],
                         time_part[6], time_part[7],
                         time_part[9], time_part[10],
                         time_part[11], time_part[12],
                         time_part[13], time_part[14]);
            } else {
                strcpy(info->display_time, "Unknown");
            }
            // 读取更多元数据
            char fullpath[512];
            snprintf(fullpath, sizeof(fullpath), "%s", info->filename);
            read_save_metadata(fullpath, info);

            list->count++;
        } while (FindNextFile(hFind, &find_data));
        FindClose(hFind);
    #else
        // 非 Windows 平台：使用简单的目录扫描
        FILE* fp;
        char cmd[256];
        #ifdef _WIN32
            // handled above
        #else
            snprintf(cmd, sizeof(cmd), "ls saves/save_*.txt 2>/dev/null");
            fp = popen(cmd, "r");
            if (!fp) return 0;
            while (fgets(cmd, sizeof(cmd), fp) && list->count < MAX_SAVE_FILES) {
                int l = (int)strlen(cmd);
                while (l > 0 && (cmd[l-1] == '\n' || cmd[l-1] == '\r')) cmd[--l] = '\0';
                if (l == 0) continue;
                SaveFileInfo* info = &list->files[list->count];
                strncpy(info->filename, cmd, sizeof(info->filename) - 1);
                // ... same time extraction logic ...
                list->count++;
            }
            pclose(fp);
        #endif
    #endif

    return list->count;
}

// 保存棋局到文件
int game_save(GameContext* ctx, const char* filename) {
    ensure_saves_dir();

    FILE* file = fopen(filename, "w");
    if (!file) {
        return 0;
    }

    // 写入保存时间
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    fprintf(file, "SAVE_TIME=%04d%02d%02d_%02d%02d%02d\n",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
    
    // 保存游戏模式
    fprintf(file, "MODE=%d\n", ctx->mode);
    
    // 保存游戏状态
    fprintf(file, "STATE=%d\n", ctx->state);
    
    // 保存当前玩家
    fprintf(file, "CURRENT_PLAYER=%d\n", ctx->board.current_player);
    
    // 保存落子数量
    fprintf(file, "MOVE_COUNT=%d\n", ctx->board.move_count);
    
    // 保存棋盘
    fprintf(file, "BOARD_START\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            fprintf(file, "%d", ctx->board.grid[i][j]);
            if (j < BOARD_SIZE - 1) {
                fprintf(file, " ");
            }
        }
        fprintf(file, "\n");
    }
    fprintf(file, "BOARD_END\n");
    
    // 保存落子历史
    fprintf(file, "HISTORY_START\n");
    for (int i = 0; i < ctx->board.move_count; i++) {
        fprintf(file, "%d %d\n", 
                ctx->board.move_history[i][0], 
                ctx->board.move_history[i][1]);
    }
    fprintf(file, "HISTORY_END\n");
    
    fclose(file);
    return 1;
}

// 从文件加载棋局
int game_load(GameContext* ctx, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return 0;
    }
    
    char line[256];
    int reading_board = 0;
    int reading_history = 0;
    int board_row = 0;
    
    // 初始化棋盘
    board_init(&ctx->board);
    
    while (fgets(line, sizeof(line), file)) {
        // 去除换行符
        int len = (int)strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
            len--;
        }
        
        if (strcmp(line, "BOARD_START") == 0) {
            reading_board = 1;
            board_row = 0;
            continue;
        } else if (strcmp(line, "BOARD_END") == 0) {
            reading_board = 0;
            continue;
        } else if (strcmp(line, "HISTORY_START") == 0) {
            reading_history = 1;
            continue;
        } else if (strcmp(line, "HISTORY_END") == 0) {
            reading_history = 0;
            continue;
        }
        
        if (reading_board) {
            // 解析棋盘行
            int col = 0;
            char* token = strtok(line, " ");
            while (token != NULL && col < BOARD_SIZE) {
                ctx->board.grid[board_row][col] = atoi(token);
                token = strtok(NULL, " ");
                col++;
            }
            board_row++;
        } else if (reading_history) {
            // 解析落子历史
            int row, col;
            if (sscanf(line, "%d %d", &row, &col) == 2) {
                ctx->board.move_history[ctx->board.move_count][0] = row;
                ctx->board.move_history[ctx->board.move_count][1] = col;
                ctx->board.move_count++;
            }
        } else {
            // 解析键值对
            char* key = strtok(line, "=");
            char* value = strtok(NULL, "=");
            if (key && value) {
                if (strcmp(key, "MODE") == 0) {
                    ctx->mode = atoi(value);
                } else                 if (strcmp(key, "STATE") == 0) {
                    int loaded_state = atoi(value);
                    // 存档状态映射：只保留 WIN/DRAW，其他统一为 PLAYING
                    if (loaded_state == STATE_WIN || loaded_state == STATE_DRAW) {
                        ctx->state = loaded_state;
                    } else {
                        ctx->state = STATE_PLAYING;
                    }
                } else if (strcmp(key, "CURRENT_PLAYER") == 0) {
                    ctx->board.current_player = atoi(value);
                } else if (strcmp(key, "MOVE_COUNT") == 0) {
                    ctx->board.move_count = atoi(value);
                }
            }
        }
    }
    
    fclose(file);
    
    // 检查是否有获胜者
    if (ctx->board.move_count > 0) {
        int last_row = ctx->board.move_history[ctx->board.move_count - 1][0];
        int last_col = ctx->board.move_history[ctx->board.move_count - 1][1];
        ctx->winner = board_check_win(&ctx->board, last_row, last_col);
        if (ctx->winner) {
            ctx->state = STATE_WIN;
        }
    }
    
    return 1;
}

// 显示消息框（使用SDL的简单实现）
void show_message_box(const char* title, const char* message) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title, message, NULL);
}
