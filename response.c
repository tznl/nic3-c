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

