#ifndef _EMULATOR_H_
#define _EMULATOR_H_

#include <stdbool.h>
#include <glib.h>

void emulator_init(int argc, char* argv[]);
void emulator_destroy();
gpointer emulator_run(gpointer data);
void emulator_refresh_screen(struct ef9345 *ef_ctx);
void emulator_key_press(int key);
void emulator_key_release(int key);

#endif
