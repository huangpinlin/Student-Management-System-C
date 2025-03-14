#ifndef USER_H
#define USER_H

#define USER_DB "users.dat"

// 用户角色枚举
typedef enum {
    ROLE_ADMIN = 1,
    ROLE_USER = 2
} UserRole;

// 用户结构体
typedef struct {
    char username[50];
    char password[50];
    UserRole role;
} User;

// 全局变量声明
extern User current_user;

// 函数声明
void init_admin();
void login(User *current_user);
void register_user();
void change_password(User *current_user);
void save_users(User *users, int count);
void admin_menu();
void user_menu();

#endif // USER_H