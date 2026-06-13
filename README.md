# 五子棋 AI 游戏

使用 C 语言和 SDL2 图形库开发的五子棋游戏，支持人人对战和人机对战。

## 功能特性

- **人人对战**：两名玩家轮流在 15×15 棋盘上落子
- **人机对战**：与 AI 对战，AI 采用 Minimax + Alpha-Beta 剪枝算法
- **悔棋功能**：支持撤回上一步落子
- **保存/加载棋局**：将当前棋局保存为文本文件，支持从文件加载
- **图形界面**：使用 SDL2 渲染棋盘、棋子及 UI 元素
- **落子动画**：棋子带有渐变渲染效果

## 快速开始（无需编译，直接运行）

**下载发布包即可运行，无需安装任何编译器或依赖库！**

1. 下载 `wuziqi_ai_release.zip`
2. 解压到任意目录
3. 双击 `wuziqi_ai.exe` 即可开始游戏

发布包仅包含：
- `wuziqi_ai.exe` - 游戏主程序（已静态链接，不依赖 MinGW）
- `SDL2.dll` - SDL2 运行时库
- `README.md` - 说明文档
- `saves/` - 棋局存档目录

**最低系统要求**：Windows 7 及以上版本

## 从源码编译（可选）

如果你需要修改代码并重新编译，需要安装 MinGW-w64 GCC。

### 1. 安装 MinGW-w64 GCC

从以下地址下载并解压 MinGW-w64：
- https://github.com/niXman/mingw-builds-binaries/releases
- 选择 `x86_64-*-win32-seh-ucrt-*.7z` 或类似版本

将解压后的 `bin` 目录（包含 `gcc.exe`）添加到系统 PATH 环境变量。

验证安装：
```bash
gcc --version
```

### 2. 编译

**方式一：一键编译**
```bash
build.bat
```

**方式二：使用 Makefile**
```bash
make
```

编译后的 `wuziqi_ai.exe` 已静态链接 GCC 运行时，可直接分发，无需附带 MinGW DLL。

### 3. 运行
```bash
wuziqi_ai.exe
```

## 游戏操作

- **鼠标点击**：在棋盘交叉点落子
- **新游戏**：重置棋局
- **悔棋**：撤回上一步
- **保存**：保存当前棋局到 `saves/game_save.txt`
- **加载**：从 `saves/game_save.txt` 加载棋局
- **模式切换**：切换人人对战/人机对战

## 项目结构

```
wuziqi_ai/
├── wuziqi_ai.exe           # 已编译的可执行文件（可直接运行）
├── wuziqi_ai_release.zip   # 发布包（可直接分发给他人）
├── SDL2.dll                 # SDL2 运行时（已包含在发布包）
├── main.c                   # 程序入口
├── Makefile                 # 编译配置 (make)
├── build.bat                # 一键编译脚本 (Windows)
├── deps/                    # 第三方依赖（编译时需要）
│   └── sdl2/               # SDL2 开发库（自包含）
│       ├── bin/             # SDL2.dll 运行时
│       ├── include/         # SDL2 头文件
│       └── lib/             # SDL2 链接库
├── include/                 # 项目头文件
│   ├── game.h              # 游戏核心定义
│   ├── board.h             # 棋盘操作接口
│   ├── ai.h                # AI 算法接口
│   ├── render.h            # 渲染接口
│   ├── event.h             # 事件处理接口
│   └── save_load.h         # 数据持久化接口
├── src/                     # 源文件
│   ├── board.c             # 棋盘逻辑实现
│   ├── ai.c                # AI 算法实现
│   ├── render.c            # 渲染实现
│   ├── event.c             # 事件处理实现
│   └── save_load.c         # 持久化实现
├── assets/                  # 资源文件
└── saves/                   # 棋局保存目录
```

## AI 算法说明

AI 使用 Minimax 博弈树搜索算法，配合 Alpha-Beta 剪枝优化，搜索深度为 4 层。

评估函数考虑：
- 活四、冲四、活三、冲三、活二等棋型
- 进攻权重高于防守

## 许可证

MIT License
