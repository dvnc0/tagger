#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <database.h>

sqlite3* getDatabaseConnection() {
	sqlite3 *databaseConnection;
	int rc;

	rc = sqlite3_open("tagger.db", &databaseConnection);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(databaseConnection));
		exit(1);
	}

	return databaseConnection;
}

int executeQuery(char *query) {
	sqlite3 *db = getDatabaseConnection();
	char *zErrMsg = 0;
	int rc;

	rc = sqlite3_exec(db, query, 0, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		closeDatabaseConnection(db);
		exit(1);
	}

	closeDatabaseConnection(db);
	return 0;
}

void closeDatabaseConnection(sqlite3 *db) {
	sqlite3_close(db);
}

int insert(char *query, char *params[]) {

	sqlite3 *db = getDatabaseConnection();
	sqlite3_stmt *stmt;
	int rc;

	rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't prepare statement: %s\n", sqlite3_errmsg(db));
		closeDatabaseConnection(db);
		exit(1);
	}

	int sizeOfParams = sizeof(&params);

	for (int i = 0; i < sizeOfParams; i++) {
		sqlite3_bind_text(stmt, i + 1, params[i], -1, SQLITE_STATIC);
	}

	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
	closeDatabaseConnection(db);

	return 0;
}