#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "user.h"
#include "student.h"

// 全局变量
User current_user = {"", "", ROLE_USER};

#ifdef _WIN32
#include <windows.h>
void init_console() {
    // 设置控制台代码页为UTF-8
    SetConsoleOutputCP(65001);  // 输出UTF-8编码
    SetConsoleCP(65001);        // 输入UTF-8编码
    
    // 设置本地化环境
    setlocale(LC_ALL, "zh_CN.UTF-8");
    
    // 获取标准输出句柄并设置字体
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX info = { 0 };
    info.cbSize = sizeof(info);
    info.dwFontSize.Y = 16;
    info.FontWeight = FW_NORMAL;
    wcscpy(info.FaceName, L"新宋体");
    SetCurrentConsoleFontEx(hOut, FALSE, &info);
}
#else
void init_console() {
    // 在Unix/Linux系统下设置UTF-8环境
    setlocale(LC_ALL, "zh_CN.UTF-8");
    // 设置环境变量
    putenv("LANG=zh_CN.UTF-8");
}
#endif

// 函数声明
void admin_menu();
void user_menu();
void change_password(User *current_user);

int main() {
    // 初始化控制台编码设置
    init_console();
    
    // 验证编码设置
    printf("系统初始化完成，编码设置：UTF-8\n");
    
    // 登录
    login(&current_user);
    
    if(current_user.role == ROLE_ADMIN) {
        admin_menu();
    } else {
        user_menu();
    }
    return 0;
}

void admin_menu() {
    int choice;
    while(1) {
        printf("\n管理员菜单\n");
        printf("1. 添加学生\n");
        printf("2. 删除学生\n");
        printf("3. 修改学生\n");
        printf("4. 成绩排序\n");
        printf("5. 数据统计\n");
        printf("6. CSV导入\n");
        printf("7. 修改密码\n");
        printf("0. 退出\n");
        printf("请选择操作: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: add_student(); break;
            case 2: delete_student(); break;
            case 3: modify_student(); break;
            case 4: sort_students(); break;
            case 5: show_statistics(); break;
            case 6: import_csv(); break;
            case 7: change_password(&current_user); break;
            case 0: exit_system(); return;
            default: printf("无效选择!\n");
        }
    }
}

void user_menu() {
    int choice;
    while(1) {
        printf("\n普通用户菜单\n");
        printf("1. 查看所有学生\n");
        printf("2. 查看统计信息\n");
        printf("3. 修改密码\n");
        printf("0. 退出\n");
        printf("请选择操作: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: display_students(); break;
            case 2: show_statistics(); break;
            case 3: change_password(&current_user); break;
            case 0: exit_system(); return;
            default: printf("无效选择!\n");
        }
    }
}