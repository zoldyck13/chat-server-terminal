#include <iostream>
#include <sqlite3.h>

sqlite3* db;

void InitializeDb(){
    char* errMsg = nullptr;

    int rc = sqlite3_open("admin.db", &db);
    if (rc != SQLITE_OK){
        std::cerr << "Cannont open databse: " << sqlite3_errmsg(db) << std::endl;
        exit(-1);
    }

    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT UNIQUE NOT NULL, "
        "password TEXT NOT NULL);";

    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        exit(-1);
    }

    std::cout << "Database and users table created successfully.\n";

}



bool insertUser(sqlite3* db, const std::string& username, const std::string& password) {
    const char* sql = "INSERT INTO users (username, password) VALUES (?, ?);";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    // Bind username and password
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE) {
        std::cout << "User inserted successfully\n";
        return true;
    } else {
        std::cerr << "Insert failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }
}



bool checkLogin(sqlite3* db, const std::string& username, const std::string& password) {
    const char* sql = "SELECT id FROM users WHERE username=? AND password=?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    // Bind the parameters
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    bool found = (rc == SQLITE_ROW); // Found a matching record

    sqlite3_finalize(stmt);
    return found;
}