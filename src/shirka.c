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
/**
 * @author Adrien Jauffret
 * @date April 2011
 */

#include <math.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <Ivy/ivyloop.h>

#include <locale.h>
#include "net_message_debug_dist.h"
#include "basic_tools.h"
#include "themis_ivy.h"

#define THIN_BORDER 3

#define GROUP_NAME_SIZE 64
#define VALUE_NAME_SIZE 64
#define MAXIMUM_SIZE_OF_PROM_BUS_COMMAND 64
#define SIZE_OF_TITLE 128

#define STATUSBAR_BROADCAST 0
#define STATUSBAR_INFO 1
#define SIZE_OF_TIME 256
#define SIZE_OF_SPEAK 256

#define SIZE_OF_ADDRESS 64

#define SIZE_OF_COMPUTER 64


typedef struct prom_bus_command {
  char command[MAXIMUM_SIZE_OF_PROM_BUS_COMMAND];
} type_prom_bus_command;

/* Ce pointeur doit etre toujours valable tant que ivy tourne */
char ivy_prom_name[SIZE_OF_IVY_PROM_NAME];
char bus_id[BUS_ID_MAX];


void ivy_speak_callback(IvyClientPtr app, void *user_data, int argc, char **argv)
{
  char time_string[SIZE_OF_TIME];
  char speak_string[SIZE_OF_SPEAK];
  int result;
  time_t now;
  (void)user_data;
  (void) argc;
  (void)app;

  now = time(NULL);
  strftime(time_string, 16, "%Hh%M:", localtime(&now)); /* Affichage de l'heure d'arrive des msgs */

  printf("%s %s\n",time_string,argv[0]);
  snprintf(speak_string, SIZE_OF_SPEAK, "espeak %s",argv[0]); /* construction de la commande system appelant le speech vocodeur 'espeak' */

  result=system("amixer set Master 100%"); /* prereglage du volume general Master&PCM */
  if (result != 0) EXIT_ON_ERROR("Fail changing general volume");
  result=system("amixer set PCM 100%");
  if (result != 0) EXIT_ON_ERROR("Fail changing PCM volume");
  result=system(speak_string);
  if (result != 0) EXIT_ON_ERROR("Fail saying: %s", speak_string);
}

void ivyApplicationCallback (IvyClientPtr app, void *user_data, IvyApplicationEvent event)
{
  char *appname;
  char *host;
  (void)user_data;

  appname = IvyGetApplicationName (app);
  host = IvyGetApplicationIp (app);

  switch  (event)  {
  case IvyApplicationConnected:
    printf("Connexion de %s d'address %s\n", appname, host);
    break;

  case IvyApplicationDisconnected:
    printf("%s disconnected from %s\n", appname,  host);
    break;

  default:
    printf("%s: unkown event %d\n", appname, event);
    break;
  }
}

void quit()
{
  IvyStop();
  exit(0);
}

int main(int argc, char *argv[])
{
  char computer_name[SIZE_OF_COMPUTER];
  char prom_bus_address[SIZE_OF_ADDRESS];
  int i;
	int option;

	bus_id[0]=0;

  setlocale(LC_ALL, "C");
  strncpy(prom_bus_address, "127.255.255.255:2010", SIZE_OF_ADDRESS);
  for (i = 1; i < argc; i++)
  {
  	while ((option = getopt(argc, argv, "b:i:")) != -1)
  	{
  		switch (option)
  		{
  		case 'b':
  			strncpy(prom_bus_address, optarg, INET_ADDRSTRLEN);
  			break;
  		case 'i':
  			strncpy(bus_id, optarg, BUS_ID_MAX);
  			break;
  		default: /* '?' */
  			EXIT_ON_ERROR("\tUsage: %s [-b broadcast_ip] [-i bus_id] \n", argv[0]);
  			break;
  		}
  	}
  }

  if (bus_id[0]==0) EXIT_ON_ERROR("You need to specify a bus id: -i[name of the bus id]");


  gethostname(computer_name, SIZE_OF_COMPUTER);
  snprintf(ivy_prom_name, SIZE_OF_IVY_PROM_NAME, "shirka:%s", computer_name);
  IvyInit(ivy_prom_name, NULL, ivyApplicationCallback, NULL, NULL, NULL);
  IvyBindMsg(ivy_speak_callback, NULL, "^%s:speak\\((.*)\\)$", bus_id);

  printf("Shirka is ready with bus_id: -i%s.\n", bus_id);
  IvyStart(prom_bus_address);
  IvyMainLoop();

  return 0;
}




