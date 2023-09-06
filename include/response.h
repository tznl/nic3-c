#ifndef RESPONSE_H
#define RESPONSE_H
#include <mysql.h>

enum cards_columns {C_ID, C_NAME, C_DESCRIPTION, C_QUANTITY, C_IMAGE_LINK};         
enum cards_relation {R_USER_ID, R_CARD_ID}; 

void simple_embed_string
(struct discord*, const struct discord_interaction*, char*);

void claim_success_embed
(struct discord*, const struct discord_interaction*, MYSQL_ROW);

void spawncard_success_embed                                                     
(struct discord*, const struct discord_interaction*, MYSQL_ROW, float);

void inventory_embed                                                     
(struct discord*, const struct discord_interaction*, MYSQL_ROW, float);
#endif
