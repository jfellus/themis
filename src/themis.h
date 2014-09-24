/*
Copyright  ETIS — ENSEA, Université de Cergy-Pontoise, CNRS (1991-2014)
promethe@ensea.fr

Authors: P. Andry, J.C. Baccon, D. Bailly, A. Blanchard, S. Boucena, A. Chatty, N. Cuperlier, P. Delarboulas, P. Gaussier, 
C. Giovannangeli, C. Grand, L. Hafemeister, C. Hasson, S.K. Hasnain, S. Hanoune, J. Hirel, A. Jauffret, C. Joulain, A. Karaouzène,  
M. Lagarde, S. Leprêtre, M. Maillard, B. Miramond, S. Moga, G. Mostafaoui, A. Pitti, K. Prepin, M. Quoy, A. de Rengervé, A. Revel ...

See more details and updates in the file AUTHORS 

This software is a computer program whose purpose is to simulate neural networks and control robots or simulations.
This software is governed by the CeCILL v2.1 license under French law and abiding by the rules of distribution of free software. 
You can use, modify and/ or redistribute the software under the terms of the CeCILL v2.1 license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info".
As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, 
users are provided only with a limited warranty and the software's author, the holder of the economic rights,  and the successive licensors have only limited liability. 
In this respect, the user's attention is drawn to the risks associated with loading, using, modifying and/or developing or reproducing the software by the user in light of its specific status of free software, 
that may mean  that it is complicated to manipulate, and that also therefore means that it is reserved for developers and experienced professionals having in-depth computer knowledge. 
Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured 
and, more generally, to use and operate it in the same conditions as regards security. 
The fact that you are presently reading this means that you have had knowledge of the CeCILL v2.1 license and that you accept its terms.
*/
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
  char pandora[PATH_MAX];
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
Node *themis_get_xml_preferences(Node *tree);
void preferences_load();
void preferences_save();


/* Functions not dependant of graphic libraries */
void prom_bus_send_message(const char *format, ...);
void add_new_script(const char *name);
void remove_scripts();
void load(char *filename);
int save(char *filename);
void quit();

#endif
