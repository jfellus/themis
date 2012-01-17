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
  result=system("amixer set PCM 100%");

  result=system(speak_string);
}

void ivyApplicationCallback (IvyClientPtr app, void *user_data, IvyApplicationEvent event)
{
  char *appname;
  char *host;
  (void)user_data;

  appname = IvyGetApplicationName (app);
  host = IvyGetApplicationHost (app);

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
  setlocale(LC_ALL, "C");
  strncpy(prom_bus_address, "127.255.255.255:2010", SIZE_OF_ADDRESS);
  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-b") == 0)
    {
      if (i < argc)
      {
        i++;
        strncpy(prom_bus_address, argv[i], SIZE_OF_ADDRESS);
      }
      else
      EXIT_ON_ERROR("An address like '127.255.255.255:2010' must follow the option '-b'.");
    }
  }

  gethostname(computer_name, SIZE_OF_COMPUTER);
  snprintf(ivy_prom_name, SIZE_OF_IVY_PROM_NAME, "shirka:%s", computer_name);
  IvyInit(ivy_prom_name, NULL, ivyApplicationCallback, NULL, NULL, NULL);
  IvyBindMsg(ivy_speak_callback, NULL, "^speak\\((.*)\\)$");

  printf("Shirka is ready.\n");
  IvyStart(prom_bus_address);
  IvyMainLoop();

  return 0;
}




