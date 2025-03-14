#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "user.h"

// 声明外部变量
extern User current_user;

// 初始化管理员账号
void init_admin() {
    FILE *fp = fopen(USER_DB, "rb");
    if (!fp) {
        // 如果文件不存在，创建默认管理员账号
        User admin = {"admin", "123456", ROLE_ADMIN};
        fp = fopen(USER_DB, "wb");
        if (fp) {
            fwrite(&admin, sizeof(User), 1, fp);
            fclose(fp);
            printf("已创建默认管理员账号\n");
            printf("用户名：admin\n");
            printf("密码：123456\n");
        }
    } else {
        fclose(fp);
    }
}

// 用户登录
void login(User *current_user) {
    char username[50];
    char password[50];
    int attempts = 0;
    const int MAX_ATTEMPTS = 3;
    int choice;
    char input[50];
    
    // 初始化管理员账号
    init_admin();
    
    while (1) {
        printf("\n========== 欢迎使用学生管理系统 ==========\n");
        printf("1. 用户登录\n");
        printf("2. 新用户注册\n");
        printf("0. 退出系统\n");
        printf("请选择操作: ");
        
        // 使用fgets读取整行输入
        if (fgets(input, sizeof(input), stdin)) {
            // 清除输入中的换行符
            input[strcspn(input, "\n")] = 0;
            
            // 检查输入是否为单个数字
            if (strlen(input) == 1 && input[0] >= '0' && input[0] <= '9') {
                choice = input[0] - '0';
            } else {
                printf("\n错误：请输入有效的数字选项(0-2)！\n");
                continue;
            }
        } else {
            // 如果fgets失败，清理输入缓冲区并继续
            while (getchar() != '\n');
            printf("\n错误：输入无效，请重试！\n");
            continue;
        }
        
        switch(choice) {
            case 1:
                // 用户登录
                while (attempts < MAX_ATTEMPTS) {
                    printf("\n========== 用户登录 ==========\n");
                    printf("请输入用户名: ");
                    scanf("%49s", username);
                    printf("请输入密码: ");
                    scanf("%49s", password);
                    
                    FILE *fp = fopen(USER_DB, "rb");
                    if (!fp) {
                        printf("错误：无法打开用户数据文件！\n");
                        return;
                    }
                    
                    User user;
                    int found = 0;
                    
                    while (fread(&user, sizeof(User), 1, fp)) {
                        if (strcmp(user.username, username) == 0 && 
                            strcmp(user.password, password) == 0) {
                            found = 1;
                            *current_user = user;
                            break;
                        }
                    }
                    fclose(fp);
                    
                    if (found) {
                        printf("\n登录成功！\n");
                        if (user.role == ROLE_ADMIN) {
                            printf("欢迎管理员 %s\n", username);
                        } else {
                            printf("欢迎用户 %s\n", username);
                        }
                        return;
                    } else {
                        attempts++;
                        printf("\n错误：用户名或密码错误！\n");
                        if (attempts < MAX_ATTEMPTS) {
                            printf("还剩 %d 次尝试机会\n", MAX_ATTEMPTS - attempts);
                        } else {
                            printf("\n错误：登录尝试次数过多，请稍后再试！\n");
                            attempts = 0; // 重置尝试次数
                            break; // 返回主菜单
                        }
                    }
                }
                break;
                
            case 2:
                // 新用户注册
                register_user();
                attempts = 0; // 重置登录尝试次数
                break;
                
            case 0:
                printf("\n感谢使用，再见！\n");
                exit(0);
                
            default:
                printf("\n无效的选择，请输入0-2之间的数字！\n");
        }
    }
}

// 注册新用户
void register_user() {
    User new_user;
    FILE *fp;
    
    printf("\n========== 用户注册 ==========\n");
    
    // 输入用户名
    do {
        printf("请输入用户名（3-20个字符）: ");
        scanf("%49s", new_user.username);
        
        if (strlen(new_user.username) < 3 || strlen(new_user.username) > 20) {
            printf("错误：用户名长度必须在3-20个字符之间！\n");
            continue;
        }
        
        // 检查用户名是否已存在
        fp = fopen(USER_DB, "rb");
        if (fp) {
            User temp;
            int exists = 0;
            while (fread(&temp, sizeof(User), 1, fp)) {
                if (strcmp(temp.username, new_user.username) == 0) {
                    exists = 1;
                    break;
                }
            }
            fclose(fp);
            if (exists) {
                printf("错误：该用户名已存在！\n");
                continue;
            }
        }
        break;
    } while (1);
    
    // 输入密码
    do {
        printf("请输入密码（6-20个字符）: ");
        scanf("%49s", new_user.password);
        
        if (strlen(new_user.password) < 6 || strlen(new_user.password) > 20) {
            printf("错误：密码长度必须在6-20个字符之间！\n");
            continue;
        }
        break;
    } while (1);
    
    // 新注册用户默认为普通用户
    new_user.role = ROLE_USER;
    
    // 保存用户信息
    fp = fopen(USER_DB, "ab");
    if (!fp) {
        printf("错误：无法创建用户文件！\n");
        return;
    }
    
    if (fwrite(&new_user, sizeof(User), 1, fp) != 1) {
        printf("错误：保存用户信息失败！\n");
        fclose(fp);
        return;
    }
    
    printf("\n注册成功！请使用新账号登录。\n");
    fclose(fp);
}

// 修改密码
void change_password(User *current_user) {
    char old_password[50];
    char new_password[50];
    char confirm_password[50];
    
    printf("\n========== 修改密码 ==========\n");
    
    // 验证旧密码
    printf("请输入当前密码: ");
    scanf("%49s", old_password);
    
    if (strcmp(old_password, current_user->password) != 0) {
        printf("错误：当前密码错误！\n");
        return;
    }
    
    // 输入新密码
    do {
        printf("请输入新密码（6-20个字符）: ");
        scanf("%49s", new_password);
        
        if (strlen(new_password) < 6 || strlen(new_password) > 20) {
            printf("错误：密码长度必须在6-20个字符之间！\n");
            continue;
        }
        
        printf("请确认新密码: ");
        scanf("%49s", confirm_password);
        
        if (strcmp(new_password, confirm_password) != 0) {
            printf("错误：两次输入的密码不一致！\n");
            continue;
        }
        break;
    } while (1);
    
    // 更新密码
    strcpy(current_user->password, new_password);
    
    // 更新文件中的用户信息
    FILE *fp = fopen(USER_DB, "r+b");
    if (!fp) {
        printf("错误：无法打开用户数据文件！\n");
        return;
    }
    
    User temp;
    while (fread(&temp, sizeof(User), 1, fp)) {
        if (strcmp(temp.username, current_user->username) == 0) {
            fseek(fp, -(long)sizeof(User), SEEK_CUR);
            if (fwrite(current_user, sizeof(User), 1, fp) != 1) {
                printf("错误：更新密码失败！\n");
                fclose(fp);
                return;
            }
            break;
        }
    }
    
    printf("\n密码修改成功！\n");
    fclose(fp);
} 