#include "event.h"
#include "board.h"
#include "render.h"
#include "save_load.h"
#include "ai.h"
#include <stdlib.h>

// 将鼠标坐标转换为棋盘行列
int event_mouse_to_board(int mouse_x, int mouse_y, int* row, int* col) {
    int board_pixel_size = (BOARD_SIZE - 1) * CELL_SIZE;
    int board_x = (WINDOW_WIDTH - board_pixel_size) / 2;
    int coord_space = 18;
    int board_y = MARGIN + coord_space + 5;

    // 计算最近的棋盘交叉点
    int cell_x = mouse_x - board_x;
    int cell_y = mouse_y - board_y;

    int nearest_col = (cell_x + CELL_SIZE / 2) / CELL_SIZE;
    int nearest_row = (cell_y + CELL_SIZE / 2) / CELL_SIZE;

    // 检查是否在棋盘范围内（允许一定误差）
    if (nearest_row >= 0 && nearest_row < BOARD_SIZE &&
        nearest_col >= 0 && nearest_col < BOARD_SIZE) {
        *row = nearest_row;
        *col = nearest_col;
        return 1;
    }

    return 0;
}

// 检查按钮点击
int event_check_button_click(Button* btn, int mouse_x, int mouse_y) {
    return (mouse_x >= btn->rect.x && mouse_x <= btn->rect.x + btn->rect.w &&
            mouse_y >= btn->rect.y && mouse_y <= btn->rect.y + btn->rect.h);
}

// 处理游戏中的鼠标点击
static void handle_game_click(GameContext* ctx, int mouse_x, int mouse_y) {
    // 检查按钮点击
    if (event_check_button_click(&ctx->btn_new_game, mouse_x, mouse_y)) {
        game_reset(ctx);
        return;
    }

    if (event_check_button_click(&ctx->btn_undo, mouse_x, mouse_y)) {
        board_undo(&ctx->board);
        // 如果是人机对战，需要连续悔两步（AI的一步和玩家的一步）
        if (ctx->mode == MODE_PVE && ctx->board.move_count > 0) {
            board_undo(&ctx->board);
        }
        return;
    }

    if (event_check_button_click(&ctx->btn_save, mouse_x, mouse_y)) {
        char path[256];
        game_get_default_save_path(path, 256);
        if (game_save(ctx, path)) {
            show_message_box("保存成功", "棋局已保存");
        } else {
            show_message_box("保存失败", "无法保存棋局");
        }
        return;
    }

    if (event_check_button_click(&ctx->btn_load, mouse_x, mouse_y)) {
        // 进入存档选择界面
        game_scan_saves(&ctx->save_list);
        ctx->state = STATE_LOAD;
        return;
    }

    if (event_check_button_click(&ctx->btn_mode_pvp, mouse_x, mouse_y)) {
        ctx->mode = MODE_PVP;
        game_reset(ctx);
        return;
    }

    if (event_check_button_click(&ctx->btn_mode_pve, mouse_x, mouse_y)) {
        ctx->mode = MODE_PVE;
        game_reset(ctx);
        return;
    }

    if (event_check_button_click(&ctx->btn_back_to_menu, mouse_x, mouse_y)) {
        ctx->state = STATE_MENU;
        return;
    }

    if (ctx->state == STATE_WIN || ctx->state == STATE_DRAW) {
        // 结算面板按钮（坐标与 render.c 结算面板保持一致）
        int panel_w = 480, panel_h = 400;
        int panel_x = WINDOW_WIDTH / 2 - panel_w / 2;
        int panel_y = WINDOW_HEIGHT / 2 - panel_h / 2;

        int info_sep_y = panel_y + 132;
        int info_y = info_sep_y + 20 + 5 * 36;  // 5行信息 + 分隔线
    
        int btn_y = info_y + 14;
        SDL_Rect btn_new_game = {panel_x + 30, btn_y, 130, 38};
        SDL_Rect btn_back_menu = {panel_x + panel_w - 30 - 130, btn_y, 130, 38};

        if (mouse_x >= btn_new_game.x && mouse_x <= btn_new_game.x + btn_new_game.w &&
            mouse_y >= btn_new_game.y && mouse_y <= btn_new_game.y + btn_new_game.h) {
            game_reset(ctx);
            ctx->state = STATE_PLAYING;
            return;
        }

        if (mouse_x >= btn_back_menu.x && mouse_x <= btn_back_menu.x + btn_back_menu.w &&
            mouse_y >= btn_back_menu.y && mouse_y <= btn_back_menu.y + btn_back_menu.h) {
            ctx->state = STATE_MENU;
            return;
        }
        return;
    }

    // 处理棋盘点击（落子）
    if (ctx->ai_thinking) return;  // AI思考中，不允许点击

    int row, col;
    if (event_mouse_to_board(mouse_x, mouse_y, &row, &col)) {
        if (board_place_piece(&ctx->board, row, col, ctx->board.current_player)) {
            // 检查是否获胜
            int winner = board_check_win(&ctx->board, row, col);
            if (winner) {
                ctx->state = STATE_WIN;
                ctx->winner = winner;
                return;
            }

            // 检查是否平局
            if (board_is_full(&ctx->board)) {
                ctx->state = STATE_DRAW;
                return;
            }

            // 切换玩家
            switch_player(&ctx->board);

            // 如果是人机对战且轮到AI
            if (ctx->mode == MODE_PVE && ctx->board.current_player == PLAYER_WHITE) {
                ctx->ai_thinking = 1;
                int ai_row, ai_col;
                ai_make_move(&ctx->board, &ai_row, &ai_col);
                board_place_piece(&ctx->board, ai_row, ai_col, ctx->board.current_player);

                winner = board_check_win(&ctx->board, ai_row, ai_col);
                if (winner) {
                    ctx->state = STATE_WIN;
                    ctx->winner = winner;
                    ctx->ai_thinking = 0;
                    return;
                }

                if (board_is_full(&ctx->board)) {
                    ctx->state = STATE_DRAW;
                    ctx->ai_thinking = 0;
                    return;
                }

                switch_player(&ctx->board);
                ctx->ai_thinking = 0;
            }
        }
    }
}

// 处理存档加载界面点击
static void handle_load_click(GameContext* ctx, int mouse_x, int mouse_y) {
    // Load 按钮 — 加载当前选中的存档
    SDL_Rect btn_confirm = {WINDOW_WIDTH / 2 - 110, WINDOW_HEIGHT - 55, 100, 35};
    if (mouse_x >= btn_confirm.x && mouse_x <= btn_confirm.x + btn_confirm.w &&
        mouse_y >= btn_confirm.y && mouse_y <= btn_confirm.y + btn_confirm.h) {
        int idx = ctx->save_list.selected_index;
        if (ctx->save_list.count > 0 && idx >= 0 && idx < ctx->save_list.count) {
            if (game_load(ctx, ctx->save_list.files[idx].filename)) {
                // 先切状态，防止消息框阻塞期间 render_load_screen 仍被绘制
                ctx->state = STATE_PLAYING;
                // 强制立即重绘一帧（清掉 load screen 残留）
                render_game(ctx);
                show_message_box("加载成功", "棋局已加载");
            } else {
                show_message_box("加载失败", "无法加载棋局");
            }
        }
        return;
    }

    // Cancel 按钮 — 返回游戏
    SDL_Rect btn_cancel = {WINDOW_WIDTH / 2 + 10, WINDOW_HEIGHT - 55, 100, 35};
    if (mouse_x >= btn_cancel.x && mouse_x <= btn_cancel.x + btn_cancel.w &&
        mouse_y >= btn_cancel.y && mouse_y <= btn_cancel.y + btn_cancel.h) {
        ctx->state = STATE_PLAYING;
        return;
    }

    // 点击存档列表项 — 只高亮选中，不直接加载
    int item_h = 40;
    int start_y = 95;
    int max_visible = 12;
    for (int i = ctx->save_list.scroll_offset;
         i < ctx->save_list.count && (i - ctx->save_list.scroll_offset) < max_visible;
         i++) {
        int y = start_y + (i - ctx->save_list.scroll_offset) * item_h;
        SDL_Rect item_rect = {45, y, WINDOW_WIDTH - 90, item_h - 2};
        if (mouse_x >= item_rect.x && mouse_x <= item_rect.x + item_rect.w &&
            mouse_y >= item_rect.y && mouse_y <= item_rect.y + item_rect.h) {
            ctx->save_list.selected_index = i;  // 选中该记录
            return;
        }
    }
}

// 处理菜单点击
static void handle_menu_click(GameContext* ctx, int mouse_x, int mouse_y) {
    // 检查"人人对战"按钮
    SDL_Rect btn_pvp = {WINDOW_WIDTH / 2 - 110, 250, 220, 52};
    if (mouse_x >= btn_pvp.x && mouse_x <= btn_pvp.x + btn_pvp.w &&
        mouse_y >= btn_pvp.y && mouse_y <= btn_pvp.y + btn_pvp.h) {
        ctx->mode = MODE_PVP;
        ctx->state = STATE_PLAYING;
        game_reset(ctx);
        return;
    }

    // 检查"人机对战"按钮
    SDL_Rect btn_pve = {WINDOW_WIDTH / 2 - 110, 320, 220, 52};
    if (mouse_x >= btn_pve.x && mouse_x <= btn_pve.x + btn_pve.w &&
        mouse_y >= btn_pve.y && mouse_y <= btn_pve.y + btn_pve.h) {
        ctx->mode = MODE_PVE;
        ctx->state = STATE_PLAYING;
        game_reset(ctx);
        return;
    }
}

// 处理鼠标事件
void event_handle_click(GameContext* ctx, int mouse_x, int mouse_y) {
    if (ctx->state == STATE_MENU) {
        handle_menu_click(ctx, mouse_x, mouse_y);
    } else if (ctx->state == STATE_LOAD) {
        handle_load_click(ctx, mouse_x, mouse_y);
    } else {
        handle_game_click(ctx, mouse_x, mouse_y);
    }
}

// 更新按钮悬停状态
static void update_button_hover(Button* btn, int mouse_x, int mouse_y) {
    btn->is_hovered = event_check_button_click(btn, mouse_x, mouse_y);
}

// 处理SDL事件
int event_handle(GameContext* ctx, SDL_Event* event) {
    int mouse_x, mouse_y;

    switch (event->type) {
        case SDL_QUIT:
            return 1;  // 退出

        case SDL_MOUSEBUTTONDOWN:
            if (event->button.button == SDL_BUTTON_LEFT) {
                event_handle_click(ctx, event->button.x, event->button.y);
            }
            break;

        case SDL_MOUSEWHEEL:
            if (ctx->state == STATE_LOAD) {
                int max_visible = 12;
                int max_scroll = ctx->save_list.count - max_visible;
                if (max_scroll < 0) max_scroll = 0;
                ctx->save_list.scroll_offset -= event->wheel.y; // 向下滚为正
                if (ctx->save_list.scroll_offset < 0) ctx->save_list.scroll_offset = 0;
                if (ctx->save_list.scroll_offset > max_scroll) ctx->save_list.scroll_offset = max_scroll;
            }
            break;

        case SDL_MOUSEMOTION:
            mouse_x = event->motion.x;
            mouse_y = event->motion.y;

            if (ctx->state == STATE_MENU) {
                // 菜单按钮悬停检测（简化）
            } else {
                // 更新按钮悬停状态
                update_button_hover(&ctx->btn_new_game, mouse_x, mouse_y);
                update_button_hover(&ctx->btn_undo, mouse_x, mouse_y);
                update_button_hover(&ctx->btn_save, mouse_x, mouse_y);
                update_button_hover(&ctx->btn_load, mouse_x, mouse_y);
                update_button_hover(&ctx->btn_mode_pvp, mouse_x, mouse_y);
                update_button_hover(&ctx->btn_mode_pve, mouse_x, mouse_y);
                update_button_hover(&ctx->btn_back_to_menu, mouse_x, mouse_y);
            }
            break;
    }

    return 0;  // 继续运行
}

// 处理菜单事件（简化接口）
int event_handle_menu(GameContext* ctx, SDL_Event* event) {
    return event_handle(ctx, event);
}
