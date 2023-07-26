#ifndef COMMAND_H
#define COMMAND_H
void spawn_card(struct discord*, const struct discord_message*);
void set_command(struct discord*);
bool valid_command(struct discord*, const struct discord_message*);
#endif
