#include <sqlite3.h>

sqlite3* getDatabaseConnection();
int executeQuery(char *query);
void closeDatabaseConnection(sqlite3 *db);
int insert(char *query, char *params[]);