/*
 * script.c
 *
 *  Created on: Mar 13, 2012
 *      Author: arnablan
 */

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#include "prom_tools/include/libpromnet.h"
#include "prom_tools/include/basic_tools.h"

#include "themis.h"
#include "script.h"

void makefile_add_upload(FILE *makefile, char *file_path)
{

	if (file_path[0] != 0)
	{
		fprintf(makefile, " %s_upload_promnet", file_path);
	}
}

void makefile_add_argument(FILE *makefile, char *file_path)
{
	gchar *basename;

	if (file_path[0] != 0)
	{
		basename = g_path_get_basename(file_path);
		fprintf(makefile, " %s", basename);
		g_free(basename);
	}
}

void script_create_makefile(t_prom_script *script)
{
	FILE *makefile;
	char working_directory[PATH_MAX];
	const char *rsh_graphic_option;
	int nohup = 0;

	snprintf(script->path_makefile, PATH_MAX, "%s/Makefile.%s", themis.tmp_dir, script->logical_name);
	if ((makefile = fopen(script->path_makefile, "w")) == NULL) EXIT_ON_ERROR("Impossible to create %s", script->path_makefile);

	fprintf(makefile, "\n/tmp/%s/logs:\n", getenv("USER"));
	fprintf(makefile, "\tmkdir -p $@\n");

	snprintf(working_directory, PATH_MAX, "%s/%s", themis.dirname, script->path_prom_deploy);

	if (script->prt == NULL) script->is_local = 1;
	else
	{
		strcpy(script->computer, script->prt->hosts[script->prt->my_host].name);
		if ((strcmp(script->computer, "localhost") == 0) || (strcmp(script->computer, "127.0.0.1") == 0) || (script->login[0] == 0)) script->is_local = 1;
		else script->is_local = 0;
	}

	if (script->is_local)
	{
		nohup = 1;

		fprintf(makefile, "working_directory:=%s\n\n", working_directory);
		fprintf(makefile, "\nrun_debug: | /tmp/%s/logs\n", getenv("USER"));
		fprintf(makefile, "\tcd $(working_directory) && nohup nemiver %s_debug %s %s %s %s %s %s -n%s -b%s -i%s %s --distant-terminal> /tmp/%s/logs/%s.log\n", script->path_prom_binary, script->path_file_script, script->path_file_config, script->path_file_res, script->path_file_dev, script->path_file_gcd, script->path_file_prt, script->logical_name, themis.ip, themis.id, script->prom_args_line, getenv("USER"), script->logical_name);
		fprintf(makefile, "\nrun: | /tmp/%s/logs\n", getenv("USER"));
		fprintf(makefile, "\tcd $(working_directory) && nohup %s %s %s %s %s %s %s -n%s -b%s -i%s %s --distant-terminal> /tmp/%s/logs/%s.log\n", script->path_prom_binary, script->path_file_script, script->path_file_config, script->path_file_res, script->path_file_dev, script->path_file_gcd, script->path_file_prt, script->logical_name, themis.ip, themis.id, script->prom_args_line, getenv("USER"), script->logical_name);

	}
	else
	{
		if (script->login[0] == 0)
		PRINT_WARNING( "You need to specify a login to connect to a distant computer.");
		else
		{
			if (strncmp(script->path_prom_binary, "promethe", PATH_MAX) == 0) rsh_graphic_option = "-X";
			else rsh_graphic_option = NULL;

			fprintf(makefile, ".PHONY:mkdir_promnet\n\n");
			fprintf(makefile, "synchronize_paths:=%s\n\n", script->synchronize_paths);
			fprintf(makefile, "mkdir_promnet:\n");
			fprintf(makefile, "\trsh %s@%s mkdir -p promnet/%s\n\n", script->login, script->computer, script->logical_name);
			fprintf(makefile, "mkdir_bin_leto_prom:\n");
			fprintf(makefile, "\trsh %s@%s mkdir -p bin_leto_prom\n\n", script->login, script->computer);

			if (!script->overwrite_res)
			{
				fprintf(makefile, "%s_upload_promnet:%s mkdir_promnet\n", script->path_file_res, script->path_file_res);
				fprintf(makefile, "\trsync --ignore-existing $< %s@%s:promnet/%s/$(<F)\n\n", script->login, script->computer, script->logical_name);
			}

			fprintf(makefile, "%%_upload_promnet:%% mkdir_promnet\n");
			fprintf(makefile, "\trsync -a $< %s@%s:promnet/%s/$(<F)\n\n", script->login, script->computer, script->logical_name);
			fprintf(makefile, "%%_upload_synchronize_path:%% mkdir_promnet\n");
			fprintf(makefile, "\trsync -a $< %s@%s:promnet/%s/$(<D)\n\n", script->login, script->computer, script->logical_name);
			fprintf(makefile, "all_upload_bin_leto_prom:~/bin_leto_prom/ mkdir_bin_leto_prom\n");
			fprintf(makefile, "\trsync -a  $< %s@%s:bin_leto_prom/\n\n", script->login, script->computer);
			fprintf(makefile, "all_upload: all_upload_bin_leto_prom $(foreach synchronize_path, $(synchronize_paths), $(synchronize_path)_upload_synchronize_path)");

			makefile_add_upload(makefile, script->path_file_script);
			makefile_add_upload(makefile, script->path_file_config);
			makefile_add_upload(makefile, script->path_file_res);
			makefile_add_upload(makefile, script->path_file_dev);
			makefile_add_upload(makefile, script->path_file_gcd);
			makefile_add_upload(makefile, script->path_file_prt);

			fprintf(makefile, "\nrun_debug:\n");
			fprintf(makefile, "\trsh -X %s@%s 'cd promnet/%s;nemiver ~/bin_leto_prom/%s_debug -n%s -b%s -i%s %s", script->login, script->computer, script->logical_name, script->path_prom_binary, script->logical_name, themis.ip, themis.id, script->prom_args_line);
			makefile_add_argument(makefile, script->path_file_script);
			makefile_add_argument(makefile, script->path_file_config);
			makefile_add_argument(makefile, script->path_file_res);
			makefile_add_argument(makefile, script->path_file_dev);
			makefile_add_argument(makefile, script->path_file_gcd);
			makefile_add_argument(makefile, script->path_file_prt);
			fprintf(makefile, " > /tmp/%s/logs/%s.log'&\n", script->login, script->logical_name);

			fprintf(makefile, "\nrun:\n");

			if (rsh_graphic_option == NULL)
			{
				nohup = 1;
				fprintf(makefile, "\trsh %s@%s 'mkdir -p /tmp/%s/logs; cd promnet/%s;nohup ~/bin_leto_prom/%s -n%s -b%s -i%s %s --distant-terminal ", script->login, script->computer, script->login, script->logical_name, script->path_prom_binary, script->logical_name, themis.ip, themis.id, script->prom_args_line);
			}
			else /* En mode graphic on ne peut pas faire nohup */
			{
				nohup = 0;
				fprintf(makefile, "\trsh -X %s@%s 'mkdir -p /tmp/%s/logs; cd promnet/%s;nohup ~/bin_leto_prom/%s -n%s -b%s -i%s %s  ", script->login, script->computer, script->login, script->logical_name, script->path_prom_binary, script->logical_name, themis.ip, themis.id, script->prom_args_line);
			}
		}

		makefile_add_argument(makefile, script->path_file_script);
		makefile_add_argument(makefile, script->path_file_config);
		makefile_add_argument(makefile, script->path_file_res);
		makefile_add_argument(makefile, script->path_file_dev);
		makefile_add_argument(makefile, script->path_file_gcd);
		makefile_add_argument(makefile, script->path_file_prt);
		fprintf(makefile, " > /tmp/%s/logs/%s.log'&\n", script->login, script->logical_name);

		fprintf(makefile, "\nshow_log:\n");
		fprintf(makefile, "\trsh %s@%s 'cat /tmp/%s/logs/%s.log'&\n", script->login, script->computer, script->login, script->logical_name);
	}
	fclose(makefile);
}
