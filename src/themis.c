/*
 * themis.c
 *
 *  Created on: 15 avr. 2011
 *      Author: arnablan
 */
#include <sys/stat.h>
#include <locale.h>
#include <sys/socket.h>

#if USE_ENET
#include "oscillo_kernel.h"
#endif

#include "themis_ivy_cb.h"
#include "themis_ivy.h"
#include "themis_gtk.h"

type_themis themis;

/**
* Envoie un message de warning avec name_of_file, name_of_function, number_of_line et affiche le message formate avec les parametres variables.
*/
void print_warning(const char *name_of_file, const char* name_of_function, int numero_of_line, const char *message, ...)
{
	va_list arguments;
	va_start(arguments, message);
	printf( "\033[1;33m %s \t %s \t %i :\n \t Warning: ", name_of_file, name_of_function, numero_of_line);
	vprintf(message, arguments);
	printf("\033[0m\n");
	va_end(arguments);
}
/**
* Envoie un message d'erreur avec name_of_file, name_of_function, number_of_line et affiche le message formate avec les parametres variables.
*/
void fatal_error(const char *name_of_file, const char* name_of_function, int numero_of_line, const char *message, ...)
{
	va_list arguments;
	va_start(arguments, message);
	printf("\n\033[1;31m %s \t %s \t %i :\n \t Error: ", name_of_file, name_of_function, numero_of_line);
	vprintf(message, arguments);
	printf("\033[0m\n\n");
	va_end(arguments);
}
/**
* Envoie un message d'erreur avec name_of_file, name_of_function, number_of_line et affiche le message formate avec les parametres variables.
* Ajoute l'affichage de l'erreur system errno
* @TODO: faire apparaître une fenetre modale
*/
void fatal_system_error(const char *name_of_file, const char* name_of_function, int numero_of_line, const char *message, ...)
{
	va_list arguments;
	va_start(arguments, message);
	printf( "\n\033[1;31m %s \t %s \t %i :\n \t Error: ", name_of_file, name_of_function, numero_of_line);
	vprintf(message, arguments);
	printf("\033[0m\n");

	printf("System error: %s\n\n", strerror(errno));
	va_end(arguments);
}

/*********************************/

int set_relative_path_from_gfile(char *path_name, char *reference_path, GFile *file)
{
	char prefix[PATH_MAX];
	char *relative_path;

	GFile *common_parent_gfile, *parent_dir_file;

	common_parent_gfile = g_file_new_for_path(reference_path);
	relative_path = g_file_get_relative_path(common_parent_gfile, file);

	memset(prefix, 0, PATH_MAX);

	/* Si working_dir n'est pas un descendant de dir_file(repertoire du fichier.net) */
	while (g_file_has_parent(common_parent_gfile, NULL) && relative_path == NULL)
	{
		strncat(prefix, "../", PATH_MAX);
		parent_dir_file = g_file_get_parent(common_parent_gfile);
		relative_path = g_file_get_relative_path(parent_dir_file, file);
		g_object_unref(common_parent_gfile);
		common_parent_gfile = parent_dir_file;
	}
	g_object_unref(common_parent_gfile);

	if (relative_path != NULL)
	{
		snprintf(path_name, PATH_MAX, "%s%s", prefix, relative_path);
		g_free(relative_path);
		return (1);
	}
	else return (0);
}

void ui_update_data(char *reference_path)
{
	int i;

	for (i = 0; i < themis_ui.number_of_scripts; i++)
	{
		script_ui_update_data(&themis_ui.script_uis[i], reference_path);
	}
}

void ui_add_script(t_prom_script *prom_script)
{

	ui_script_init(&themis_ui.script_uis[themis_ui.number_of_scripts], prom_script);
	prom_script->state = No_Undef;
	themis_ui.number_of_scripts++;
}

void add_new_script(const char *prom_id)
{
	t_prom_script *prom_script;

	prom_script = promnet_add_new_prom_script(themis.promnet);
	promnet_prom_script_set_logical_name(prom_script, (char*) prom_id);
	strncpy(prom_script->path_prom_binary, "promethe", PATH_MAX);
	strncpy(prom_script->computer, "localhost", MAX_COMPUTER);

	ui_add_script(prom_script);
}

void remove_scripts()
{
	ui_remove_scripts();
	promnet_del_all_prom_script(themis.promnet);
}

void quit()
{
	gtk_main_quit();
	IvyStop();
	printf("\nThemis quitting.\nbus_id was: -d%s\n", themis.id);
	exit(0);
}

void load(char *filename)
{
	t_prom_script *current_script;
	t_promnet *promnet;
	char *dirname;

	dirname = g_path_get_dirname(filename);
	strncpy(themis.dirname, dirname, PATH_MAX);
	g_free(dirname);

	promnet = themis.promnet;
	remove_scripts();
	promnet_load_file_net(promnet, filename);

	ui_set_filename(filename);
	for (current_script = promnet->prom_script; current_script != NULL; current_script = current_script->next)
	{
		printf("Loading %s ...\n", current_script->logical_name);

		if (strcmp(current_script->computer, "") == 0) strcpy(current_script->computer, "localhost"); /* Par defaut le computeur est la machine de lancement */
		ui_add_script(current_script);
	}
}

int save(char *filename)
{
	char *reference_dirname;

	/* On s'assure que les data (t_prom_script) correspondent a l'interface utilisateur avant de sauver. On utilise aussi l'emplcement du fichier de sauvegarde comme reference pour les chemins relatifs */
	reference_dirname = g_path_get_dirname(filename);
	ui_update_data(reference_dirname);
	g_free(reference_dirname);

	return (promnet_save(themis.promnet, filename));
}

/**
 * @param argc Number of arguments
 * @param argv Arguments of themis (e.g. <filename>.net -b <ip>:<port>)
 * @return Code d'erreur (0 if ok).
 */
int main(int argc, char *argv[])
{
	int option;
	struct sigaction action;
	struct stat file_stat;
	char command_line[SIZE_OF_COMMAND_LINE];

	printf("\nSVN revision: %s \n", STRINGIFY_CONTENT(SVN_REVISION));
	if (USE_ENET) printf("enet: enabled\n");
	else printf("enet: disabled\n\n");

	if (promnet_version != PROMNET_VERSION) EXIT_ON_ERROR("The current promnet version of themis %d and the promnet version of basicparser are different.\n\tUpdate basic_parser.", PROMNET_VERSION, promnet_version);

	sigfillset(&action.sa_mask);

	action.sa_handler = quit;
	action.sa_flags = 0;

	sigaction(SIGINT, &action, NULL);

	if (!g_thread_supported()) g_thread_init(NULL);
	else printf("g_thread deja initialisees\n");

	gdk_threads_init(); /* Called to initialize internal mutex "gdk_threads_mutex".*/
	gtk_init(&argc, &argv);

	setlocale(LC_ALL, "C");

	themis.filename[0] = 0;
	themis.ip[0] = 0;
	themis.id[0] = 0;

	while ((option = getopt(argc, argv, "b:i:")) != -1)
	{
		switch (option)
		{
		case 'b':
			strncpy(themis.ip, optarg, INET_ADDRSTRLEN);
			break;
		case 'i':
					strncpy(themis.id, optarg, BUS_ID_MAX);
					break;
		default: /* '?' */
			fprintf(stderr, "\tUsage: %s [file.net] [-b broadcast_ip] [-i bus_id] \n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (optind < argc)
	{
		/* Pour être sur de ne pas ecraser de la memoire, en faire une macro */
		if (sscanf(argv[optind],  "%" STRINGIFY_CONTENT(PATH_MAX) "s.net", themis.filename) == 1)
		{
			if (stat(themis.filename, &file_stat) != 0) EXIT_ON_ERROR("%s cannot be found.", themis.filename);
		}
	}

	if (themis.id[0]==0)
	{
		gethostname(themis.host, HOST_NAME_MAX);
		themis.pid = getpid();
		snprintf(themis.id, BUS_ID_MAX, "%s:%li", themis.host, (long int) themis.pid);
		printf("bus_id: -i%s\n\n", themis.id);
	}

	if (themis.ip[0] == 0) strcpy(themis.ip, "127.255.255.255");

	snprintf(bin_leto_prom_path, PATH_MAX, "%s/bin_leto_prom", getenv("HOME"));
	gettimeofday(&themis.starting_time, NULL);

	snprintf(themis.tmp_dir, PATH_MAX, "/tmp/%s/themis", getenv("USER"));

	snprintf(command_line, SIZE_OF_COMMAND_LINE, "mkdir -p %s\n", themis.tmp_dir);
	if (system(command_line) != 0) PRINT_WARNING("Error creating the directory. Using the command:\n%s", command_line);

	themis.promnet = promnet_init();

	ui_init();
	if (themis.filename[0] != 0)
	{
		load(themis.filename);
	}
	else
	{
		strncpy(themis.dirname, ".", PATH_MAX);
		add_new_script("name");
	}

	prom_bus_init(themis.ip); /* lancement du bus ivy */
	IvyBindMsg(ivy_receive_any_message_callback, NULL, "^%s:(.*)", themis.id);
	IvyBindMsg(ivy_here_callback, NULL, "^%s:here\\((.*)\\)$", themis.id);
	IvyBindMsg(ivy_status_callback, NULL, "^%s:status\\((.*)\\)$", themis.id);
	display_status_message("broadcast on %s:2010", themis.ip);

#if USE_ENET
	init_oscillo_kernel(1234); /* Port 1234 */
#endif

	gdk_threads_enter(); /*  version threadee de l'IHM */
	gtk_main(); /* Boucle principale de Gtk */
	gdk_threads_leave();

	quit();
	return(0);
}
