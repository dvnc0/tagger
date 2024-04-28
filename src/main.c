#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>
#include <database.h>
#include <time.h>

int makeDatabase();
int showTables();
int addSnippet(int argc, char *argv[]);
char *generateUniqueId();

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

// return must be freed
char *generateUniqueId() {
    srand(time(NULL));
    char *uniqueId = malloc(9 * sizeof(char));
    if (uniqueId != NULL) {
        for (int i = 0; i < 8; i++) {
            uniqueId[i] = 'a' + (rand() % 26);
        }
        uniqueId[8] = '\0';
    }
    return uniqueId;
}

int makeDatabase() {
	if (access("tagger.db", F_OK) != 0) {

		printf("Creating database\n");

		char snippetTable[200] = "CREATE TABLE snippets (id INTEGER PRIMARY KEY AUTOINCREMENT, snippet TEXT, unique_id TEXT);";
		char tagsTable[100] = "CREATE TABLE tags (id INTEGER PRIMARY KEY AUTOINCREMENT, tag TEXT, snippet_id TEXT);";

		char index1[100] = "CREATE INDEX unique_id_index ON snippets (unique_id);";
		char index2[100] = "CREATE INDEX snippet_id_index ON tags (snippet_id);";

		executeQuery(snippetTable);
		executeQuery(tagsTable);
		executeQuery(index1);
		executeQuery(index2);
	}

	return 0;
}

// todo
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

	char *uniqueId = generateUniqueId();

	char *sql = "INSERT INTO snippets (snippet, unique_id) VALUES (?, ?);";
	
	char *params[] = {argv[2], uniqueId};
	
	int tagCount = argc - 3;

	char *tags[tagCount];
	int n = 0;

	for (int i = 3; i < argc; i++) {
		tags[n] = argv[i];
		n++;
	}

	insert(sql, params);

	sql = "INSERT INTO tags (tag, snippet_id) VALUES (?, ?);";

	for (int i = 0; i < tagCount; i++) {
		char *tagParams[] = {tags[i], uniqueId};
		insert(sql, tagParams);
	}

	free(uniqueId);
	
	return 0;
}