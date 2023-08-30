#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include "discord.h"
#include "include/command.h"

enum cards_columns {id, name, description, quantity, image_link};
enum cards_relation {user_id, card_id};

int spawned_card = 0;

void on_interaction(struct discord* client, const struct discord_interaction* event)
{
/* user commands */

	inventory	(client, event);
	claim_card	(client, event);

/* admin commands */

	clear_card	(client, event);
	spawn_card	(client, event);
	create_card	(client, event);
	remove_card	(client, event);
}

void set_slash(struct discord* client, const struct discord_ready* event)
{
	struct discord_create_global_application_command clear_params = {
		.name = "clear",
		.description = "clear the currently spawned card",
		.default_permission = true,
	};
	discord_create_global_application_command(
		client, event->application->id, &clear_params, NULL);

	struct discord_create_global_application_command inventory_params = {
		.name = "inventory",
		.description = "view your collection of cards",
		.default_permission = true,
	};
	discord_create_global_application_command(
		client, event->application->id, &inventory_params, NULL);

	struct discord_create_global_application_command claim_params = {
		.name = "claim",
		.description = "claim the currently spawned card",
		.default_permission = true,
	};
	discord_create_global_application_command(
		client, event->application->id, &claim_params, NULL);

	struct discord_create_global_application_command spawncard_params = {
		.name = "spawncard",
		.description = "spawn a card",
		.default_permission = true,
	};
	discord_create_global_application_command(
		client, event->application->id, &spawncard_params, NULL);

	struct discord_application_command_option create_card_options[] = {
		{
			.type = DISCORD_APPLICATION_OPTION_INTEGER,
			.name = "id",
			.description = "assign numerical identifier (or random if specified)",
			.required = true,
		},
		{
			.type = DISCORD_APPLICATION_OPTION_STRING,
			.name = "name",
			.description = "card name",
			.required = true,
		},
		{
			.type = DISCORD_APPLICATION_OPTION_STRING,
			.name = "description",
			.description = "card description",
			.required = true,
		},
		{
			.type = DISCORD_APPLICATION_OPTION_STRING,
			.name = "quantity",
			.description = "card quantity (lower quantity is higher rarity)",
			.required = true,
		},
		{
			.type = DISCORD_APPLICATION_OPTION_STRING,
			.name = "image_link",
			.description = "link to image",
			.required = true,
		},
	};
 

	struct discord_create_global_application_command create_card_params = {
		.name = "create_card",
		.description = "create a card",
		.default_permission = true,
		.options =
			&(struct discord_application_command_options){
			.size = sizeof(create_card_options) / sizeof *create_card_options,
			.array = create_card_options,
			},
	};
	discord_create_global_application_command(
		client, event->application->id, &create_card_params, NULL);

	struct discord_application_command_option remove_card_options[] = {
		{
			.type = DISCORD_APPLICATION_OPTION_INTEGER,
			.name = "id",
			.description = "id of card to remove",
			.required = true,
		},
	};
 

	struct discord_create_global_application_command remove_card_params = {
		.name = "remove_card",
		.description = "remove a card",
		.default_permission = true,
		.options =
			&(struct discord_application_command_options){
			.size = sizeof(remove_card_options) / sizeof *remove_card_options,
			.array = remove_card_options,
			},
	};
	discord_create_global_application_command(
		client, event->application->id, &remove_card_params, NULL);

	discord_set_on_interaction_create(client, &on_interaction);
}

void remove_card(struct discord* client, const struct discord_interaction* event)
{
	if (event->type != DISCORD_INTERACTION_APPLICATION_COMMAND) return;
	if (strcmp(event->data->name, "remove_card")) return; 
	if (atoi( event->data->options->array[0].value) < 1) {
		struct discord_embed embeds[] = {
			{
				.title = "please specifiy an id greater than 0",
				.timestamp = discord_timestamp(client),
			},
		};

		struct discord_interaction_response params = {
			.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
			.data = &(struct discord_interaction_callback_data){ 
				.embeds =
					&(struct discord_embeds){
					.size = sizeof(embeds) / sizeof *embeds,
					.array = embeds,
					},
			}
		};
		discord_create_interaction_response(client, event->id, event->token, &params, NULL);
		return;
	}

	extern MYSQL *conn;
	MYSQL_RES *res;
	char query_buffer[200];

	sprintf(query_buffer, "SELECT * FROM cards WHERE id=%d", 
		atoi( event->data->options->array[0].value ) );
	mysql_query(conn, query_buffer);

	res = mysql_use_result(conn);
	mysql_fetch_row(res);
	int dbcheck_cards = mysql_num_rows(res);
	mysql_free_result(res);


	sprintf(query_buffer, "SELECT * FROM relation WHERE card_id=%d", 
		atoi( event->data->options->array[0].value ) );
	mysql_query(conn, query_buffer);

	res = mysql_use_result(conn);
	mysql_fetch_row(res);
	int dbcheck_relation = mysql_num_rows(res);
	mysql_free_result(res);

	if (dbcheck_cards == 0 && dbcheck_relation == 0) {
		struct discord_embed embeds[] = {
			{
				.title = "a card with this id doesnt exist ",
				.timestamp = discord_timestamp(client),
			},
		};

		struct discord_interaction_response params = {
			.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
			.data = &(struct discord_interaction_callback_data){ 
				.embeds =
					&(struct discord_embeds){
					.size = sizeof(embeds) / sizeof *embeds,
					.array = embeds,
					},
			}
		};
		discord_create_interaction_response(client, event->id, event->token, &params, NULL);
		return;
	}

	sprintf(query_buffer, "DELETE FROM cards WHERE id=%d", 
		atoi( event->data->options->array[0].value ) );
	mysql_query(conn, query_buffer);

	sprintf(query_buffer, "DELETE FROM relation WHERE card_id=%d", 
		atoi( event->data->options->array[0].value ) );
	mysql_query(conn, query_buffer);

	struct discord_embed embeds[] = {
		{
			.title = "success",
			.timestamp = discord_timestamp(client),
		},
	};
	struct discord_interaction_response params = {
		.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
		.data = &(struct discord_interaction_callback_data){ 
			.embeds =
				&(struct discord_embeds){
				.size = sizeof(embeds) / sizeof *embeds,
				.array = embeds,
				},
		}
	};
	discord_create_interaction_response(client, event->id, event->token, &params, NULL);
}

void create_card(struct discord* client, const struct discord_interaction* event)
{
	if (event->type != DISCORD_INTERACTION_APPLICATION_COMMAND) return;
	if (strcmp(event->data->name, "create_card")) return; 
	if (atoi( event->data->options->array[0].value) < 1) {
		struct discord_embed embeds[] = {
			{
				.title = "please specifiy an id greater than 0",
				.timestamp = discord_timestamp(client),
			},
		};

		struct discord_interaction_response params = {
			.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
			.data = &(struct discord_interaction_callback_data){ 
				.embeds =
					&(struct discord_embeds){
					.size = sizeof(embeds) / sizeof *embeds,
					.array = embeds,
					},
			}
		};
		discord_create_interaction_response(client, event->id, event->token, &params, NULL);
		return;
	}

	extern MYSQL *conn;
	MYSQL_RES *res;
	char query_buffer[2000];

	sprintf(query_buffer, "SELECT * FROM cards WHERE id=%d", 
		atoi( event->data->options->array[0].value ) );
	mysql_query(conn, query_buffer);

	res = mysql_use_result(conn);
	mysql_fetch_row(res);

	if (mysql_num_rows(res) != 0) {
		struct discord_embed embeds[] = {
			{
				.title = "a card with this id already exists :<",
				.timestamp = discord_timestamp(client),
			},
		};

		struct discord_interaction_response params = {
			.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
			.data = &(struct discord_interaction_callback_data){ 
				.embeds =
					&(struct discord_embeds){
					.size = sizeof(embeds) / sizeof *embeds,
					.array = embeds,
					},
			}
		};
		discord_create_interaction_response(client, event->id, event->token, &params, NULL);
		mysql_free_result(res);
		return;
	}
	mysql_free_result(res);

	sprintf(query_buffer, 
	"INSERT INTO cards (id, name, description, quantity, image_link) " 
	"VALUES (%d, '%s', '%s', %d, '%s')",
		atoi( event->data->options->array[0].value ),
		event->data->options->array[1].value,
		event->data->options->array[2].value,
		atoi( event->data->options->array[3].value ) ,
		event->data->options->array[4].value);

	mysql_query(conn, query_buffer);

	struct discord_embed embeds[] = {
		{
			.title = "success",
			.timestamp = discord_timestamp(client),
		},
	};
	struct discord_interaction_response params = {
		.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
		.data = &(struct discord_interaction_callback_data){ 
			.embeds =
				&(struct discord_embeds){
				.size = sizeof(embeds) / sizeof *embeds,
				.array = embeds,
				},
		}
	};
	discord_create_interaction_response(client, event->id, event->token, &params, NULL);
}
void clear_card(struct discord* client, const struct discord_interaction* event)
{

	if (event->type != DISCORD_INTERACTION_APPLICATION_COMMAND) return;
	if (strcmp(event->data->name, "clear")) return; 
	if (spawned_card == 0) {
		struct discord_embed embeds[] = {
			{
				.title = "no spawned card to clear",
				.timestamp = discord_timestamp(client),
			},
		};

		struct discord_interaction_response params = {
			.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
			.data = &(struct discord_interaction_callback_data){ 
				.embeds =
					&(struct discord_embeds){
					.size = sizeof(embeds) / sizeof *embeds,
					.array = embeds,
					},
			}
		};
		discord_create_interaction_response(client, event->id, event->token, &params, NULL);
		return;
	}

	spawned_card = 0;

	struct discord_embed embeds[] = {
		{
			.title = "spawn cleared",
			.timestamp = discord_timestamp(client),
		},
	};

	struct discord_interaction_response params = {
		.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
		.data = &(struct discord_interaction_callback_data){ 
			.embeds =
				&(struct discord_embeds){
				.size = sizeof(embeds) / sizeof *embeds,
				.array = embeds,
				},
		}
	};
 

	discord_create_interaction_response(client, event->id, event->token, &params, NULL);
}

void inventory(struct discord* client, const struct discord_interaction* event)
{
	if (event->type != DISCORD_INTERACTION_APPLICATION_COMMAND ||
		strcmp(event->data->name, "inventory")) return; 
	extern MYSQL *conn;

	MYSQL_RES *res;
	MYSQL_ROW row;

	char query_buffer[200];

	sprintf(query_buffer, "SELECT card_id FROM relation WHERE user_id = %ld", 
		event->member->user->id);
	mysql_query(conn, query_buffer);
	res = mysql_use_result(conn);
	row = mysql_fetch_row(res);

	if (mysql_num_rows(res) == 0) {
		struct discord_embed embeds[] = {
			{
				.title = "you have no cards :<",
				.timestamp = discord_timestamp(client),
			},
		};
	
		struct discord_interaction_response params = {
			.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
			.data = &(struct discord_interaction_callback_data){ 
				.embeds =
					&(struct discord_embeds){
					.size = sizeof(embeds) / sizeof *embeds,
					.array = embeds,
					},
			}
		};

		discord_create_interaction_response(client, event->id, event->token, &params, NULL);
		mysql_free_result(res);
		return;
	}
	mysql_free_result(res);

	mysql_query(conn, "SELECT SUM(quantity) FROM cards");
	res = mysql_use_result(conn);
	row = mysql_fetch_row(res);
	int quantity_total = atoi(row[0]);
	mysql_free_result(res);

	sprintf(query_buffer, 
		"SELECT * FROM cards, relation "
		"WHERE cards.id = relation.card_id "
		"AND relation.user_id = %ld "
		"ORDER BY cards.id", 
		event->member->user->id);

	mysql_query(conn, query_buffer);
	res = mysql_store_result(conn);
	row = mysql_fetch_row(res);

	char rarity_text[50];
	float rarity = (atoi(row[quantity]) / (float)quantity_total) * 100;
	sprintf(rarity_text, "%.2f%% rarity", rarity);

	char title[50];
	sprintf(title, "%s's inventory", event->member->user->username);

	struct discord_embed_field fields[] = {
   			{
			.name = row[name],
			.value = row[description],
		},
	};
	
	struct discord_embed embeds[] = {
		{
			.title = title,
			.description = rarity_text,
			.timestamp = discord_timestamp(client),
			.color = (rarity/100) * 16777215,
			.footer = &(struct discord_embed_footer){
				.text = row[id]
				},
			.image =
				&(struct discord_embed_image){
					.url = row[image_link],
			},
			.fields =
				&(struct discord_embed_fields){
				.size = sizeof(fields) / sizeof *fields,
				.array = fields,
				},
		},
	};
	 
	struct discord_interaction_response params = {
		.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
		.data = &(struct discord_interaction_callback_data){ 
			.embeds =
				&(struct discord_embeds){
				.size = sizeof(embeds) / sizeof *embeds,
				.array = embeds,
				},
		}
	};
	
	discord_create_interaction_response(client, event->id, event->token, &params, NULL);
	

	mysql_free_result(res);
}

void claim_card(struct discord* client, const struct discord_interaction* event)
{
	if (event->type != DISCORD_INTERACTION_APPLICATION_COMMAND) return;
	if (strcmp(event->data->name, "claim")) return; 
	if (spawned_card == 0) {
		struct discord_embed embeds[] = {
			{
				.title = "no card to claim :<",
				.timestamp = discord_timestamp(client),
			},
		};
	
		struct discord_interaction_response params = {
			.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
			.data = &(struct discord_interaction_callback_data){ 
				.embeds =
					&(struct discord_embeds){
					.size = sizeof(embeds) / sizeof *embeds,
					.array = embeds,
					},
			}
		};

		discord_create_interaction_response(client, event->id, event->token, &params, NULL);
		return;
	}

	extern MYSQL *conn;

	MYSQL_RES *res;
	MYSQL_ROW row;

	char query_buffer[200];

/* check for redundancy */

	sprintf(query_buffer, "SELECT * FROM relation WHERE user_id = %ld AND card_id = %d", 
		 event->member->user->id,
		spawned_card);


	mysql_query(conn, query_buffer);
	res = mysql_use_result(conn);
	row = mysql_fetch_row(res);

	if (mysql_num_rows(res) != 0) {
		struct discord_embed embeds[] = {
			{
				.title = "you already have this cardd",
				.timestamp = discord_timestamp(client),
			},
		};
	
		struct discord_interaction_response params = {
			.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
			.data = &(struct discord_interaction_callback_data){ 
				.embeds =
					&(struct discord_embeds){
					.size = sizeof(embeds) / sizeof *embeds,
					.array = embeds,
					},
			}
		};

		discord_create_interaction_response(client, event->id, event->token, &params, NULL);
		mysql_free_result(res);
		return;
	}

/* store card into relation database */

	sprintf(query_buffer, "INSERT INTO relation( user_id, card_id) VALUES( %ld , %d )", 
		 event->member->user->id,
		spawned_card);
	mysql_query(conn, query_buffer);

/* idk lole print results */

	sprintf(query_buffer, "SELECT * FROM cards WHERE id = %d", spawned_card);
	mysql_query(conn, query_buffer);
	res = mysql_use_result(conn);
	row = mysql_fetch_row(res);

	char embed_desc[50];
	sprintf(embed_desc, "%s was claimed by %s", row[name],  event->member->user->username);

	char avatar_url[200];
	sprintf(
		avatar_url, 
		"https://cdn.discordapp.com/avatars/%ld/%s.jpg", 
		 event->member->user->id,
		 event->member->user->avatar);
	
	struct discord_embed embeds[] = {
		{
			.title = "card claimed",
			.description = embed_desc,
			.timestamp = discord_timestamp(client),
			.image =
				&(struct discord_embed_image){
					.url = avatar_url,
			},
		},
	};
 
	struct discord_interaction_response params = {
		.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
		.data = &(struct discord_interaction_callback_data){ 
			.embeds =
				&(struct discord_embeds){
				.size = sizeof(embeds) / sizeof *embeds,
				.array = embeds,
				},
		}
	};

	discord_create_interaction_response(client, event->id, event->token, &params, NULL);

	spawned_card = 0;
	mysql_free_result(res);
}

void spawn_card(struct discord* client, const struct discord_interaction* event)
{
	if (event->type != DISCORD_INTERACTION_APPLICATION_COMMAND) return;
	if (strcmp(event->data->name, "spawncard")) return; 
	if (spawned_card != 0) {
		struct discord_embed embeds[] = {
			{
				.title = "card already spawned",
				.timestamp = discord_timestamp(client),
			},
		};
	
		struct discord_interaction_response params = {
			.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
			.data = &(struct discord_interaction_callback_data){ 
				.embeds =
					&(struct discord_embeds){
					.size = sizeof(embeds) / sizeof *embeds,
					.array = embeds,
					},
			}
		};
	
		discord_create_interaction_response(client, event->id, event->token, &params, NULL);
		return;
	}

	extern MYSQL *conn;

	MYSQL_RES *res;
	MYSQL_ROW row;
	char query_buffer[200];

/* sum total quantity of cards and get a random number between it */

	mysql_query(conn, "SELECT SUM(quantity) FROM cards");
	res = mysql_use_result(conn);
	row = mysql_fetch_row(res);

	int rd = 0;
	int quantity_total = atoi(row[0]);
	rd = rand() % quantity_total + 1;

	mysql_free_result(res);

	printf("max: %d\n\n", quantity_total);
	printf("random: %d\n\n", rd);

/* grab the mysql entry depending on the random id grabbed */

	sprintf(query_buffer, 
		"SELECT id FROM "
		"(SELECT id, SUM(quantity) "
		"OVER (ORDER BY quantity) AS total FROM cards) a "
		"WHERE total >= %d ORDER BY id ASC LIMIT 0, 1", rd);

	mysql_query(conn, query_buffer);
	res = mysql_use_result(conn);
	row = mysql_fetch_row(res);
	spawned_card = atoi(row[0]);
	printf("id: %d\n\n", spawned_card);
	mysql_free_result(res);

	sprintf(query_buffer, "SELECT * FROM cards WHERE id = %d", spawned_card);
	mysql_query(conn, query_buffer);
	res = mysql_use_result(conn);
	row = mysql_fetch_row(res);

	int selected_quantity = atoi(row[quantity]);

	float rarity = ((float)selected_quantity / quantity_total)* 100;

	char rarity_text[50];
	sprintf(rarity_text, "%.2f%% rarity!", rarity);

	struct discord_embed_field fields[] = {
    		{
			.name = rarity_text,
			.value = "type /claim to capture",
		},
	};

	char embed_desc[50];
	sprintf(embed_desc, "%s available for capture", row[name]);

	struct discord_embed embeds[] = {
		{
			.title = "card spawned!1",
			.description = embed_desc,
			.timestamp = discord_timestamp(client),
			.color = (rarity/100) * 16777215,
			.image =
				&(struct discord_embed_image){
					.url = row[image_link],
			},
			.fields =
				&(struct discord_embed_fields){
				.size = sizeof(fields) / sizeof *fields,
				.array = fields,
				},
		},
	};
 
	struct discord_interaction_response params = {
		.type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,
		.data = &(struct discord_interaction_callback_data){ 
			.embeds =
				&(struct discord_embeds){
				.size = sizeof(embeds) / sizeof *embeds,
				.array = embeds,
				},
		}
	};
	
	discord_create_interaction_response(client, event->id, event->token, &params, NULL);

	mysql_free_result(res);
}
