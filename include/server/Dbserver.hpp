#pragma once
#ifndef __DBSERVER__
#define __DBSERVER__

#include <iostream>
#include <sqlite3.h>

extern sqlite3* db;

inline void InitializeDb(){
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



inline bool insertUser(sqlite3* db, const std::string& username, const std::string& password) {
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



inline bool checkLogin(sqlite3* db, const std::string& username, const std::string& password) {
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

inline bool checkName(sqlite3* db, const std::string& username){
    const char* sql = "SELECT id FROM users WHERE username=?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << "\n";
        return false;
    }

    // Bind the parameters
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    bool found = (rc == SQLITE_ROW); 

    return found;
}

inline bool updateUsername(sqlite3* db, const std::string& olduser, std::string& newuser){

    const char* sql2 = "SELECT id FROM users WHERE username=?;";

    sqlite3_stmt* stmt;
    int user_id = -1;

    if(sqlite3_prepare_v2(db, sql2, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, olduser.c_str(), -1, SQLITE_TRANSIENT);


    if(sqlite3_step(stmt) == SQLITE_ROW){
        user_id = sqlite3_column_int(stmt, 0);
    }


    sqlite3_finalize((stmt));

    const char* sql3 = "UPDATE users SET username=? WHERE id=?;";

    sqlite3_stmt* stmt2;
    
    if(sqlite3_prepare_v2(db, sql3, -1, &stmt2, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepaer statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt2, 1, newuser.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt2, 2, user_id);

    if(sqlite3_step(stmt2) != SQLITE_DONE){
        std::cerr << "Failed to Update username: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_finalize(stmt2);

    return true;
}


inline bool updatePassword(sqlite3* db, const std::string& username, const std::string& pass){

    const char* sql = "SELECT id FROM users WHERE username=?;";

    sqlite3_stmt* stmt;
    int user_id = -1;

    if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK ){

        std::cerr << "Failed to prepaer statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    if(sqlite3_step(stmt) == SQLITE_ROW){
        user_id = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);

    const char* sql2 = "UPDATE users SET password=? WHERE id=?;";

    sqlite3_stmt* stmt2;

    if(sqlite3_prepare_v2(db, sql2, -1, &stmt2, nullptr) != SQLITE_OK){

        std::cerr << "Failed to prepaer statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt2, 1, pass.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt2, 2, user_id);

    if(sqlite3_step(stmt2) != SQLITE_DONE){
        std::cerr << "Failed to Update username: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_finalize(stmt2);
    return true;
    

}


#endif