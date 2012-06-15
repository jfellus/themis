/*
 * themis.h
 *
 *  Created on: Apr 27, 2011
 *      Author: arnablan
 */
#ifndef THEMIS_H
#define THEMIS_H

#include <bits/local_lim.h>
#include <sys/time.h>

#include <prom_tools/include/prom_bus.h>
#include <prom_tools/include/xml_tools.h>
#include <prom_tools/include/basic_tools.h>
#include <prom_tools/include/libpromnet.h>
#include <prom_kernel/include/net_message_debug_dist.h>

#include "script.h"

#define SIZE_OF_PROM_BUS_BUFFER 1024
#define SIZE_OF_IVY_PROM_NAME 128

#define IVY_MAX_SIZE_OF_MESSAGES 128

#define SIZE_OF_COMMAND_LINE 1024
#define MAX_SIZE_OF_INPUT_FEED 512
#define MAX_NUMBER_OF_ARGS 128

#define SIZE_OF_NAME 128
#define SIZE_OF_TITLE 128
#define SIZE_OF_ID 128
#define SIZE_OF_ADDRESS 64
#define SIZE_OF_ARGUMENT 128
#define SIZE_OF_GROUP_NAME 64
#define SIZE_OF_TIME 16
#define SIZE_OF_TARGET 64


#define MAX_NUMBER_OF_SCRIPTS 64
#define MAX_NUMBER_OF_COMPUTERS 64
#define NUMBER_OF_ARGUMENTS_MAX 64

#ifdef USE_ENET
#define USE_ENET 1
#else
#define USE_ENET 0
#endif

char bin_leto_prom_path[PATH_MAX];

#define SIZE_OF_PID 16

typedef struct themis
{
  struct timeval starting_time;
  char filename[PATH_MAX];
  char preferences[PATH_MAX];
  char dirname[PATH_MAX];
	FILE *makefile;
  char ip[INET_ADDRSTRLEN];
  char tmp_dir[PATH_MAX];
  char host[HOST_NAME_MAX];
  char id[BUS_ID_MAX];
  pid_t pid;



  /** Computer */
  computer *first_computer; /* Shared with basicparser */

  /** Themis */
  t_promnet *promnet; /* Defini dans basic_parser */

}type_themis;

extern type_themis themis;

/* User interface functions (implementation in the graphic (e.g themis_gtk.c) part */
void ui_remove_scripts();
void ui_update_data(char *reference_dirname);
void display_status_message(const char *message, ...);
Node *themis_get_xml_informations(Node *tree);

/* Functions not dependant of graphic libraries */
void prom_bus_send_message(const char *format, ...);
void add_new_script(const char *name);
void remove_scripts();
void load(char *filename);
int save(char *filename);
void quit();

#endif
