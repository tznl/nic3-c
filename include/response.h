#ifndef RESPONSE_H
#define RESPONSE_H
#include <mysql.h>

void simple_embed_string
	(struct discord*, const struct discord_interaction*, char*);
void claim_success_embed
	(struct discord*, const struct discord_interaction*, MYSQL_ROW);

void spawncard_success_embed                                                     
	(struct discord* client, const struct discord_interaction* event,
	MYSQL_ROW row, float rarity);

void inventory_embed                                                     
	(struct discord* client, const struct discord_interaction* event,
	MYSQL_ROW row, float rarity);
#endif
