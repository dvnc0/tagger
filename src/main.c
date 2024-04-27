#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>

int makeDatabase();
int showTables();
int addSnippet(int argc, char *argv[]);

int main(int argc, char *argv[]) {
	
	if (argc == 1) {
		printf("Usage: %s <database>\n", argv[0]);
		return 1;
	}

	makeDatabase();

	if (strcmp(argv[1], "add") == 0) {
		printf("Adding snippet\n");
		addSnippet(argc, argv);
	} else if (strcmp(argv[1], "search") == 0) {
		printf("Search snippet\n");
	} else if (strcmp(argv[1], "delete") == 0) {
		printf("Delete snippet\n");
	} else if (strcmp(argv[1], "list") == 0) {
		printf("List snippets\n");
	} else if(strcmp(argv[1], "tables") == 0){
		showTables();
	} else {
		printf("Unknown command\n");
		return 1;
	}

	return 0;
}

int makeDatabase() {
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;

	if (access("tagger.db", F_OK) != 0) {
		printf("Creating database\n");
		rc = sqlite3_open("tagger.db", &db);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
			return 1;
		}

		char snippetTable[200] = "CREATE TABLE snippets (id INTEGER PRIMARY KEY AUTOINCREMENT, snippet TEXT, unique_id TEXT);";
		char tagsTable[100] = "CREATE TABLE tags (id INTEGER PRIMARY KEY AUTOINCREMENT, tag TEXT, snippet_id TEXT);";

		char index1[100] = "CREATE INDEX unique_id_index ON snippets (unique_id);";
		char index2[100] = "CREATE INDEX snippet_id_index ON tags (snippet_id);";

		rc = sqlite3_exec(db, snippetTable, 0, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}

		rc = sqlite3_exec(db, tagsTable, 0, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}

		rc = sqlite3_exec(db, index1, 0, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}

		rc = sqlite3_exec(db, index2, 0, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}

		sqlite3_close(db);
	}

	return 0;
}

int showTables() {
	sqlite3 *db;
	sqlite3_stmt *stmt;
	char *zErrMsg = 0;
	int rc;

	rc = sqlite3_open("tagger.db", &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	char *sql = "SELECT name FROM sqlite_master WHERE type='table';";

	sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
	int s;
	while ((s = sqlite3_step(stmt)) == SQLITE_ROW) {
		printf("%s\n", sqlite3_column_text(stmt, 0));
	}

	sqlite3_finalize(stmt);

	sqlite3_close(db);

	return 0;
}

int addSnippet(int argc, char *argv[]) {

	if (argc < 4) {
		printf("Usage: %s add <snippet> <tags>\n", argv[0]);
		return 1;
	}

	char uniqueId[20];

	sprintf(uniqueId, "%d", rand());

	sqlite3 *db;
	sqlite3_stmt *stmt;
	char *zErrMsg = 0;
	int rc;

	rc = sqlite3_open("tagger.db", &db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	char *sql = "INSERT INTO snippets (snippet, unique_id) VALUES (?, ?);";
	sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

	sqlite3_bind_text(stmt, 1, argv[2], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, uniqueId, -1, SQLITE_STATIC);

	sqlite3_step(stmt);

	sqlite3_finalize(stmt);

	for (int i = 3; i < argc; i++) {
		sql = "INSERT INTO tags (tag, snippet_id) VALUES (?, ?);";
		sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

		sqlite3_bind_text(stmt, 1, argv[i], -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, uniqueId, -1, SQLITE_STATIC);

		sqlite3_step(stmt);

		sqlite3_finalize(stmt);
	}

	sqlite3_close(db);

	return 0;
}