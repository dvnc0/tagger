#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>
#include <database.h>
#include <time.h>

int makeDatabase();
int addSnippet(int argc, char *argv[]);
char *generateUniqueId();
int getSnippet(char *snippet);
int getAllTags();

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
		getSnippet(argv[2]);
	} else if (strcmp(argv[1], "delete") == 0) {
		printf("Delete snippet\n");
	} else if (strcmp(argv[1], "tags") == 0) {
		printf("List tags\n");
		getAllTags();
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

int getSnippet(char *snippet) {

	char *sql = "SELECT * FROM snippets JOIN tags ON snippets.unique_id = tags.snippet_id WHERE snippet LIKE ? OR tag LIKE ?;";

	char *params[] = {snippet, snippet};

	struct Result result = fetchAll(sql, params);

	for (int i = 0; i < result.size; i++) {
		if (strcmp(result.rows[i].field_name, "snippet") == 0) {
			printf("%s\n", result.rows[i].value);
		}
	}

	free(result.rows);

	return 0;
}

int getAllTags() {
	char *sql = "SELECT * FROM tags;";
	char *params[] = {NULL};

	struct Result result = fetchAll(sql, params);

	for (int i = 0; i < result.size; i++) {
		if (strcmp(result.rows[i].field_name, "tag") == 0) {
			printf("%s\n", result.rows[i].value);
		}
	}

	free(result.rows);

	return 0;
}