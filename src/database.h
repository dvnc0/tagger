#include <sqlite3.h>

#define INITIAL_ROW_CAPACITY 10

struct Row {
	char *field_name;
	char *value;
};

struct Result {
	struct Row *rows;
	int size;
	int capacity;
};

sqlite3* getDatabaseConnection();
int executeQuery(char *query);
void closeDatabaseConnection(sqlite3 *db);
int insert(char *query, char *params[]);
struct Result fetchAll(char *sql, char *params[]);