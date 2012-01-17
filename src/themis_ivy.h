/*
 *
 *  Created on: Sep 16, 2010
 *  Author: Arnaud Blanchard
 *
 *  Description: Gere les callbacks ivy. Devra etre remanie en utilisant prom_bus.
 *
 */
#ifndef THEMIS_IVY_H
#define THEMIS_IVY_H

#include "../../shared/include/Ivy/ivy.h"
#include "../../shared/include/Ivy/ivyglibloop.h"

#include "../../prom_tools/include/prom_bus.h"

#define SIZE_OF_IVY_PROM_NAME 128
#define MAX_SIZE_OF_PROM_BUS_MESSAGE 256

extern char ivy_prom_name[SIZE_OF_IVY_PROM_NAME];

void ivyApplicationCallback(IvyClientPtr app, void *user_data, IvyApplicationEvent event);

void prom_bus_send_direct_message(IvyClientPtr app, const char *format, ...);
void prom_bus_send_message(const char *format, ...);

void prom_bus_init(char *broadcast_ip);

#endif
