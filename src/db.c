#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

#include "config.h"
#include "db.h"

int check_user_in_db(const char* db_path, const char *username, const char *password)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int status = -1;

    if (sqlite3_open(db_path, &db) != SQLITE_OK)
        return -1;

    const char *sql = "SELECT role FROM users WHERE username = ? AND password = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        goto cleanup;

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        status = sqlite3_column_int(stmt, 0);
    }

cleanup:
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return status;
}

int add_user(const char *db_path, const struct user *user)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;

    if (!user)
        return -1;

    if (sqlite3_open(db_path, &db) != SQLITE_OK)
        return -1;

    const char *sql =
        "INSERT INTO users (role, username, password) "
        "VALUES (?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        goto error;

    sqlite3_bind_int(stmt, 1, user->status);
    sqlite3_bind_text(stmt, 2, user->username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user->password, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
        goto error;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;

error:
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return -1;
}

int delete_user(const char *db_path, const char *username)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;

    if (sqlite3_open(db_path, &db) != SQLITE_OK)
        return -1;

    const char *sql =
        "DELETE FROM users WHERE username = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        goto error;

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
        goto error;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;

error:
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return -1;
}

int print_all_users(const char *db_path)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;

    if (sqlite3_open(db_path, &db) != SQLITE_OK)
        return -1;

    const char *sql =
        "SELECT id, role, username FROM users;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        goto error;

    printf("ID | ROLE | USERNAME\n");
    printf("----------------------\n");

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf(
            "%d | %d | %s\n",
            sqlite3_column_int(stmt, 0),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_text(stmt, 2)
        );
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;

error:
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return -1;
}

