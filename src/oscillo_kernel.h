/*
 * profiler.h
 *
 *  Created on: 10 mars 2011
 *      Author: Arandu Blanchard
 */

#ifndef OSCILLO_KERNEL_H
#define OSCILLO_KERNEL_H

#include <gtk/gtk.h>
#include <enet/enet.h>

typedef struct oscillo_kernel
{
  GtkWindow *window;
  ENetHost *server;
}type_oscillo_kernel;


void init_oscillo_kernel(int port);

#endif
