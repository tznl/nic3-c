#include "discord.h"
#include "include/command.h"

void set_command(struct discord* client)
{
	discord_set_on_command(client, "spawncard", &spawn_card);
}

void spawn_card(struct discord* client, const struct discord_message* event)
{
	if (!valid_command(client, event)) return;

	struct discord_create_message params1 = { .content = "spawned" };
	discord_create_message(client, event->channel_id, &params1, NULL);
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
