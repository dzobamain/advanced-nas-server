#ifndef DB_H
#define DB_H

#define USERNAME_MAX 64
#define PASSWORD_MAX 64

struct user {
    int id;
    int role; // 0 = admin, 1 = user, 2 = limited
    char username[USERNAME_MAX];
    char password[PASSWORD_MAX];
};

int is_user_in_db(const char *db_path, const struct user *usr);
int add_user(const char *db_path, const struct user *usr);
int delete_user(const char *db_path, const char *username);
int print_all_users(const char *db_path);

#endif // DB_H