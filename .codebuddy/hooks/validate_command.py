#!/usr/bin/env python3
import json
import sys

DANGEROUS_COMMANDS = [
    'rm -rf ',           # 删除整个文件系统
    'rm -rf /*',          # 同上，变体
    'dd if=/dev/zero',    # 覆写磁盘
    'chmod -R 000 /',     # 破坏所有文件权限
    'mv / ',              # 移动根目录
]

def main():
    input_data = json.loads(sys.stdin.read())

    if input_data.get('tool_name') != 'Bash':
        print(json.dumps({
            "continue": True,
            "hookSpecificOutput": {
                "hookEventName": "PreToolUse",
                "permissionDecision": "allow"
            }
        }))
        return 0

    command = input_data.get('tool_input', {}).get('command', '')

    for dangerous in DANGEROUS_COMMANDS:
        if dangerous in command:
            output = {
                "continue": False,
                "hookSpecificOutput": {
                    "hookEventName": "PreToolUse",
                    "permissionDecision": "deny",
                    "permissionDecisionReason": f"检测到危险命令: {dangerous}"
                }
            }
            print(json.dumps(output, ensure_ascii=False))
            return 0

    print(json.dumps({
        "continue": True,
        "hookSpecificOutput": {
            "hookEventName": "PreToolUse",
            "permissionDecision": "allow"
        }
    }))
    return 0

if __name__ == "__main__":
    sys.exit(main())
