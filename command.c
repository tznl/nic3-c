#include <mysql.h>
#include <stdlib.h>

#include "discord.h"
#include "include/command.h"

void set_command(struct discord* client)
{
	discord_set_on_command(client, "spawncard", &spawn_card);
}

void spawn_card(struct discord* client, const struct discord_message* event)
{
	if (!valid_command(client, event)) return;

	extern MYSQL *conn;

/*
 *	int id = 0;
 *	char name[20];
 *	char description[100];
 *	int rarity = 0;
 *	char* image_link;
 */
	MYSQL_RES *res;
	MYSQL_ROW row;
	char query_buffer[100];

/* generate random number from int min to the maximum number of cards */

	int min = 1;
	int max = 0;
	int rd = 0;

	mysql_query(conn, "SELECT COUNT(*) FROM cards");
	res = mysql_use_result(conn);
	row = mysql_fetch_row(res);

	max = atoi(row[0]);
	rd = rand() % max + min;

	printf("max: %d\n\n", max);
	printf("random: %d\n\n", rd);

	mysql_free_result(res);

/* grab the mysql entry depending on the random id grabbed */

	sprintf(query_buffer, "SELECT * FROM cards WHERE id = %d", rd);
	mysql_query(conn, query_buffer);
	res = mysql_use_result(conn);
	row = mysql_fetch_row(res);

	struct discord_embed_field fields[] = {
    		{
//			.name = "idk this is just some random big text",
			.value = "type !claim to capture",
		},
	};

	char embed_desc[50];
	sprintf(embed_desc, "%s available for capture", row[1]);

	struct discord_embed embeds[] = {
		{
			.title = "card spawned!1",
			.description = embed_desc,
			.timestamp = discord_timestamp(client),
			.image =
				&(struct discord_embed_image){
					.url = row[4],
			},
			.fields =
				&(struct discord_embed_fields){
				.size = sizeof(fields) / sizeof *fields,
				.array = fields,
				},
		},
	};
 
	struct discord_create_message params = {
		.embeds =
			&(struct discord_embeds){
			.size = sizeof(embeds) / sizeof *embeds,
			.array = embeds,
			},
	};

	discord_create_message(client, event->channel_id, &params, NULL);

	mysql_free_result(res);
}


bool valid_command(struct discord* client, const struct discord_message* event)
{
	bool check = true;

	struct discord_channel ret_channel = { 0 };
	struct discord_ret_channel ret = { .sync = &ret_channel };
	struct discord_create_dm params = { .recipient_id = event->author->id };

	discord_create_dm(client, &params, &ret);

	if (event->author->bot || event->channel_id == ret_channel.id) check = false;

	return check;
}
