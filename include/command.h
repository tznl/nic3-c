#ifndef COMMAND_H
#define COMMAND_H
void set_command(struct discord*);
void set_slash(struct discord*, const struct discord_ready*);
void spawn_card(struct discord*, const struct discord_interaction*);
void claim_card(struct discord*, const struct discord_interaction*);
void create_card(struct discord*, const struct discord_interaction*);
void remove_card(struct discord*, const struct discord_interaction*);
void inventory(struct discord*, const struct discord_interaction*);
void display_card(struct discord*, const struct discord_interaction*);
void clear_card(struct discord*, const struct discord_interaction*);
void on_interaction(struct discord*, const struct discord_interaction*);
int permission_check (struct discord*, const struct discord_interaction*);
#endif
