#include "discord.h"
#include "include/response.h"

void simple_embed_string
(struct discord* client, const struct discord_interaction* event, char* text)
{
	struct discord_embed embeds[] = {
		{
			.title = text,
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
	discord_create_interaction_response
		(client, event->id, event->token, &params, 0);
}

void claim_success_embed
(struct discord* client, const struct discord_interaction* event, MYSQL_ROW row)
{
	char embed_desc[50];
	sprintf(embed_desc,
		"%s was claimed by %s",
	row[C_NAME],  event->member->user->username);

	char avatar_url[200];
	sprintf(avatar_url, "https://cdn.discordapp.com/avatars/%ld/%s.jpg",
	event->member->user->id, event->member->user->avatar);

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

	discord_create_interaction_response
		(client, event->id, event->token, &params, NULL);
}

void spawncard_success_embed
(struct discord* client, const struct discord_interaction* event, 
MYSQL_ROW row, float rarity)
{
	char rarity_text[50];
	sprintf(rarity_text, "%.2f%% rarity!", rarity);

	struct discord_embed_field fields[] = {
		{
			.name = rarity_text,
			.value = "type /claim to capture",
		},
	};

	char embed_desc[50];
	sprintf(embed_desc, "%s available for capture", row[C_NAME]);

	struct discord_embed embeds[] = {
		{
			.title = "card spawned!1",
			.description = embed_desc,
			.timestamp = discord_timestamp(client),
			.color = (rarity/100) * 16777215,
			.image =
				&(struct discord_embed_image){
					.url = row[C_IMAGE_LINK],
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
	
	discord_create_interaction_response
		(client, event->id, event->token, &params, NULL);
}

void inventory_embed
(struct discord* client, const struct discord_interaction* event, 
MYSQL_ROW row, float rarity, int type)
{
	int final_type;

	switch (type) {
		case DISCORD_INTERACTION_APPLICATION_COMMAND: 
			final_type = 
				DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE;
			break;
		case DISCORD_INTERACTION_MESSAGE_COMPONENT: 
			final_type = DISCORD_INTERACTION_UPDATE_MESSAGE;
			break;
		default: break;
	}

	char rarity_text[50];
	sprintf(rarity_text, "%.2f%% rarity", rarity);

	char title[50];
	sprintf(title, "%s's inventory", event->member->user->username);

	struct discord_embed_field fields[] = {
		{
			.name = row[C_NAME],
			.value = row[C_DESCRIPTION],
		},
	};

	struct discord_embed embeds[] = {
		{
			.title = title,
			.description = rarity_text,
			.timestamp = discord_timestamp(client),
			.color = (rarity/100) * 16777215,
			.footer = &(struct discord_embed_footer){
				.text = row[C_ID]
			},
			.image =
				&(struct discord_embed_image){
					.url = row[C_IMAGE_LINK],
				},
			.fields =
				&(struct discord_embed_fields){
					.size = sizeof(fields) / sizeof *fields,
					.array = fields,
			},
		},
	};

	struct discord_component select_menu[] = {
		{
			.type = DISCORD_COMPONENT_BUTTON,
			.custom_id = "button_left",
			.style = DISCORD_BUTTON_PRIMARY,
			.label = "Previous",
		},
		{
			.type = DISCORD_COMPONENT_BUTTON,
			.custom_id = "button_right",
			.style = DISCORD_BUTTON_PRIMARY,
			.label = "Next",
		},
	};

	struct discord_component action_rows[] = {
		{
			.type = DISCORD_COMPONENT_ACTION_ROW,
			.components =
				&(struct discord_components){
					.size = sizeof(select_menu) / 
						sizeof *select_menu,
					.array = select_menu,
				},
		},
	};
		
	struct discord_interaction_response params = {
		.type = final_type,
		.data = &(struct discord_interaction_callback_data){
			.embeds =
				&(struct discord_embeds){
					.size = sizeof(embeds) / 
						sizeof *embeds,
					.array = embeds,
				},
			.components =                                            
				&(struct discord_components){                    
					.size = sizeof(action_rows) / 
						sizeof *action_rows,
					.array = action_rows,                            
				},
		}
	};

	discord_create_interaction_response
		(client, event->id, event->token, &params, NULL);
}
