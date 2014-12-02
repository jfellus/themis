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
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netdb.h>

#include "net_message_debug_dist.h"
#include "themis_ivy.h"

#include "themis_gtk.h"
#include "themis.h"

#define BROADCAST_MAX 32

/* Ce pointeur doit etre toujours valable tant que ivy tourne */
char ivy_prom_name[SIZE_OF_IVY_PROM_NAME];

long computer_id;
static sem_t ivy_semaphore;

char computer_name[MAXBUF];

void prom_bus_send_message(const char *format, ...)
{
  char buffer[MAX_SIZE_OF_PROM_BUS_MESSAGE];
  va_list arguments;

  va_start(arguments, format);
  vsprintf(buffer, format, arguments);
  sem_wait(&ivy_semaphore);
  IvySendMsg("%s:%s", themis.id, buffer);
  sem_post(&ivy_semaphore);
}

/*
void ivy_stat_callback(IvyClientPtr app, void *user_data, int argc, char **argv)
{
  char *prom_id, value_name[VALUE_NAME_SIZE];
  type_themis *themis = user_data;
  GtkTreeIter iter;
  float min, max, mean, sigma2;
  int sample;
  struct timeval now;
  float received_ms;
  (void) argc;

  prom_id = IvyGetApplicationName(app);

  gtk_list_store_append(get_promethe_with_id(themis, prom_id)->stat_store, &iter);

  sscanf(argv[0], "%[^,],%d,%f,%f,%f,%f", value_name, &sample, &min, &max, &mean, &sigma2);

  gettimeofday(&now, NULL);

  received_ms = diff_timespec_in_ms(&themis->starting_time, &now);
  gtk_list_store_set(get_promethe_with_id(themis, prom_id)->stat_store, &iter, 0, received_ms, 1, value_name, 2, sample, 3, min, 4, max, 5, mean, 6, sqrtf(sigma2), -1);
}*/

void ivyApplicationCallback (IvyClientPtr app, void *user_data, IvyApplicationEvent event)
{
  char *appname;
  (void)user_data;

  appname = IvyGetApplicationName (app);

  switch  (event)  {
  case IvyApplicationConnected:
     if (strncmp(appname, themis.id, strlen(themis.id)) == 0)  printf("Connection of : %s\n", appname);
    break;

  case IvyApplicationDisconnected:
     if (strncmp(appname, themis.id, strlen(themis.id)) == 0)  printf("%s disconnected\n", appname);
    break;

  default:
     if (strncmp(appname, themis.id, strlen(themis.id)) == 0)  printf("%s: unkown event %d\n", appname, event);
    break;
  }
}

void prom_bus_init(char *brodcast_ip)
{
	char broadcast[BROADCAST_MAX];
  char computer_name[HOST_NAME_MAX];
  pid_t my_pid;

  snprintf(broadcast,  BROADCAST_MAX, "%s:2010", brodcast_ip);

  gethostname(computer_name, HOST_NAME_MAX);
  my_pid = getpid();

  snprintf(ivy_prom_name, SIZE_OF_IVY_PROM_NAME, "themis:%s:%ld", computer_name, (long int)my_pid);

  sem_init(&ivy_semaphore, 0, 1);   /* semaphore necessaire pour faire des appels ivy thread safes... Prom_bus_send_message n'est pas sudffisant ??? */
  IvyInit(ivy_prom_name, "Themis started", ivyApplicationCallback, NULL, NULL, NULL);
  IvyStart(broadcast);
}
