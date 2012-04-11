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

void fatal_error(const char *name_of_file, const char* name_of_function, int numero_of_line, const char *message, ...)
{
	va_list arguments;
	va_start(arguments, message);
	printf("\033[1;31m %s \t %s \t %i :\n \t Fatal error: ", name_of_file, name_of_function, numero_of_line);
	vprintf(message, arguments);
	printf("\033[0m\n");
	va_end(arguments);
	exit(EXIT_FAILURE);
}

/**
* Envoie un message d'erreur avec name_of_file, name_of_function, number_of_line et affiche le message formate avec les parametres variables.
* Ajoute l'affichage de l'erreur system errno
*/
void fatal_system_error(const char *name_of_file, const char* name_of_function, int numero_of_line, const char *message, ...)
{
	va_list arguments;
	va_start(arguments, message);
	printf( "\n\033[1;31m %s \t %s \t %i :\n \t Error: ", name_of_file, name_of_function, numero_of_line);
	vprintf(message, arguments);
	printf("System error: %s\n\n", strerror(errno));
	printf("\033[0m\n\n");

	va_end(arguments);
	exit(EXIT_FAILURE);
}


void print_warning(const char *name_of_file, const char* name_of_function, int numero_of_line, const char *message, ...)
{
	va_list arguments;
	va_start(arguments, message);
	printf("\033[1;33m %s \t %s \t %i :\n \t Warning: ", name_of_file, name_of_function, numero_of_line);
	vprintf(message, arguments);
	printf("\033[0m\n");
	va_end(arguments);
}



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




