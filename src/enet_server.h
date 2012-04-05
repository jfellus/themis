/*
 * profiler.h
 *
 *  Created on: 10 mars 2011
 *      Author: Arnaud Blanchard
 */

#ifndef ENET_SERVER_H
#define ENET_SERVER_H

#include <gtk/gtk.h>
#include <enet/enet.h>

typedef struct oscillo_kernel
{
  GtkWindow *window;
  ENetHost *server;
}type_oscillo_kernel;


void init_oscillo_kernel(int port);
void create_window_of_profiler(const char *title);


#endif
