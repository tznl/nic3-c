#include <sys/stat.h>
#include <mysql.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "discord.h"
#include "log.h"

#include "include/command.h"
#include "include/main.h"

MYSQL *conn;

int main(int argc, char *argv[])
{
	srand(time(NULL));
	mkdir("log", 0777);

	const char *config_file;
	if (argc > 1) {
		config_file = argv[1];
	} else {
		config_file = "../config.json";
	}	
	struct discord *client = discord_config_init(config_file);
	struct ccord_szbuf_readonly config_rtr;

	MYSQL_RES *res;
	MYSQL_ROW row;
        char server[100];
        char user[100];
        char password[100];
        char database[100];

	config_rtr = discord_config_get_field(client, (char *[2]){ "sql", "host" }, 2);
	snprintf(server, sizeof(server), "%.*s", (int)config_rtr.size, config_rtr.start);

	config_rtr = discord_config_get_field(client, (char *[2]){ "sql", "user" }, 2);
	snprintf(user, sizeof(user), "%.*s", (int)config_rtr.size, config_rtr.start);

	config_rtr = discord_config_get_field(client, (char *[2]){ "sql", "password" }, 2);
	snprintf(password, sizeof(password), "%.*s", (int)config_rtr.size, config_rtr.start);

 	config_rtr = discord_config_get_field(client, (char *[2]){ "sql", "database" }, 2);
	snprintf(database, sizeof(database), "%.*s", (int)config_rtr.size, config_rtr.start);

	conn = mysql_init(NULL);

	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	if (mysql_query(conn, "SHOW TABLES")) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	}
	res = mysql_use_result(conn);

	printf("MySQL Tables in mysql database:\n");
	while ((row = mysql_fetch_row(res)) != NULL)
		printf("%s \n", row[0]);

	set_prefix(client);
	discord_set_on_ready(client, &on_ready);
	set_command(client);

	discord_run(client);

/*
 * any code after this isnt supposed to run.
 * if this shiz ever starts running tell me because that way i can start freeing memory
 * end exiting gracefully instead of just breaking shit.
 */

	printf("\n\nloleoleoleole\n\n\n\n");

}

void set_prefix(struct discord *client)
{
	char prefix[10];
	struct ccord_szbuf_readonly value = discord_config_get_field(
		client, 
		(char *[3]) { "discord", "default_prefix", "prefix" },
		3);

	snprintf(prefix, sizeof(prefix) , "%.*s", (int)value.size, value.start);

	discord_set_prefix(
		client, 
		prefix
	);
}

void on_ready(struct discord *client, const struct discord_ready *event)
{
	log_info("\n\n\n%s#%s connected yeaa.\n\n",
 		event->user->username, event->user->discriminator);
}
