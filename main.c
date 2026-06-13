#include "game.h"
#include "board.h"
#include "render.h"
#include "event.h"
#include "ai.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 初始化游戏
void game_init(GameContext* ctx) {
    ctx->mode = MODE_PVP;
    ctx->state = STATE_MENU;
    ctx->winner = 0;
    ctx->ai_thinking = 0;
    board_init(&ctx->board);
}

// 重置游戏
void game_reset(GameContext* ctx) {
    board_init(&ctx->board);
    ctx->state = STATE_PLAYING;
    ctx->winner = 0;
    ctx->ai_thinking = 0;
}

// 切换玩家
void switch_player(Board* board) {
    board->current_player = (board->current_player == PLAYER_BLACK) 
                           ? PLAYER_WHITE : PLAYER_BLACK;
}

// 清理游戏资源
void game_cleanup(GameContext* ctx) {
    render_cleanup(ctx);
}

int main(int argc, char* argv[]) {
    GameContext ctx;
    memset(&ctx, 0, sizeof(ctx));
    
    // 初始化游戏数据
    game_init(&ctx);
    
    // 初始化SDL和渲染
    if (!render_init(&ctx)) {
        printf("Failed to initialize SDL!\n");
        return 1;
    }
    
    // 主循环
    SDL_Event event;
    int running = 1;
    
    while (running) {
        // 处理事件
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
                break;
            }
            if (event_handle(&ctx, &event)) {
                running = 0;
                break;
            }
        }
        
        // 渲染
        render_game(&ctx);
        
        // 控制帧率
        SDL_Delay(16);  // 约60FPS
    }
    
    // 清理资源
    game_cleanup(&ctx);
    
    return 0;
}
