#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

#include "config.h"
#include "db.h"

int is_user_in_db(const char *db_path, const struct user *usr)
{
    /*
     * Checks whether a user exists in the database
     * specified by db_path.
     *
     * Searches for a matching username and password.
     *
     * Returns -1 on error.
     */
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    int result = -1;

    const char *sql = "SELECT 1 FROM users WHERE username = ? AND password = ? LIMIT 1;";

    if (sqlite3_open(db_path, &db) != SQLITE_OK)
        goto cleanup;

    // Prepare SQL statement
    if (sqlite3_prepare_v2(db, sql, (int)strlen(sql), &stmt, NULL) != SQLITE_OK)
        goto cleanup;

    // Find user with matching username and password
    if ((sqlite3_bind_text(stmt, 1, usr->username, strlen(usr->username), SQLITE_TRANSIENT)) != SQLITE_OK)
        goto cleanup;

    if (sqlite3_bind_text(stmt, 2, usr->password, strlen(usr->password), SQLITE_TRANSIENT) != SQLITE_OK)
        goto cleanup;

    int step = sqlite3_step(stmt);
    if (step == SQLITE_ROW) {
        result = 1;
    } 
    else if (step == SQLITE_DONE) {
        result = 0;
    }

cleanup:
    if (stmt) sqlite3_finalize(stmt);
    if (db) sqlite3_close(db);
    return result;
}

int add_user(const char *db_path, const struct user *usr)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;

    if (!usr)
        return -1;

    if (sqlite3_open(db_path, &db) != SQLITE_OK)
        return -1;

    const char *sql =
        "INSERT INTO users (role, username, password) "
        "VALUES (?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        goto error;

    sqlite3_bind_int(stmt, 1, usr->role);
    sqlite3_bind_text(stmt, 2, usr->username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, usr->password, -1, SQLITE_STATIC);

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

