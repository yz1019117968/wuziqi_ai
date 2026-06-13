@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ============================================
echo   五子棋 AI - 一键编译脚本
echo ============================================
echo.

:: 检查 gcc 是否可用
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo [错误] 未找到 gcc 编译器！
    echo.
    echo 请安装 MinGW-w64 并确保 gcc 在系统 PATH 中。
    echo.
    echo 推荐下载地址:
    echo   https://github.com/niXman/mingw-builds-binaries/releases
    echo   选择 x86_64-*-win32-seh-*.7z
    echo.
    echo 安装后请将 MinGW\bin 目录添加到 PATH 环境变量。
    pause
    exit /b 1
)

echo [信息] 检测到 gcc 编译器:
gcc --version | findstr /c:"gcc"
echo.

:: 设置路径 (项目内 deps)
set PROJECT_DIR=%~dp0
set INCLUDES=-I%PROJECT_DIR%include -I%PROJECT_DIR%deps\sdl2\include
set LDFLAGS=-L%PROJECT_DIR%deps\sdl2\lib -static-libgcc -static-libstdc++
set LIBS=-lmingw32 -lSDL2main -lSDL2 -mwindows
set CFLAGS=-Wall -std=c99 -O2
set TARGET=wuziqi_ai.exe

echo [信息] 编译源文件...
echo.

:: 编译各个源文件
echo   编译 main.c ...
gcc %CFLAGS% %INCLUDES% -c -o main.o main.c
if %errorlevel% neq 0 goto :error

echo   编译 src/board.c ...
gcc %CFLAGS% %INCLUDES% -c -o src/board.o src/board.c
if %errorlevel% neq 0 goto :error

echo   编译 src/ai.c ...
gcc %CFLAGS% %INCLUDES% -c -o src/ai.o src/ai.c
if %errorlevel% neq 0 goto :error

echo   编译 src/render.c ...
gcc %CFLAGS% %INCLUDES% -c -o src/render.o src/render.c
if %errorlevel% neq 0 goto :error

echo   编译 src/event.c ...
gcc %CFLAGS% %INCLUDES% -c -o src/event.o src/event.c
if %errorlevel% neq 0 goto :error

echo   编译 src/save_load.c ...
gcc %CFLAGS% %INCLUDES% -c -o src/save_load.o src/save_load.c
if %errorlevel% neq 0 goto :error

echo.
echo [信息] 链接可执行文件...
gcc -o %TARGET% main.o src/board.o src/ai.o src/render.o src/event.o src/save_load.o %LDFLAGS% %LIBS%
if %errorlevel% neq 0 goto :error

echo.
echo ============================================
echo   编译成功！生成: %TARGET%
echo ============================================
echo.
echo 运行方式:
echo   %TARGET%
echo.
pause
exit /b 0

:error
echo.
echo ============================================
echo   编译失败！请检查错误信息。
echo ============================================
pause
exit /b 1
