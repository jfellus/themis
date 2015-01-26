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
		fprintf(makefile, " %s_upload_file", file_path);
	}
}

void makefile_add_argument(FILE *makefile, char *file_path)
{
	if (file_path[0] != 0)
	{
		fprintf(makefile, " %s", file_path);
	}
}

void script_create_makefile(t_prom_script *script)
{
	FILE *makefile;
	char working_directory[PATH_MAX];
	const char *rsh_graphic_option;

	snprintf(script->path_makefile, PATH_MAX, "%s/%s/Makefile", themis.dirname, script->path_prom_deploy);
	if ((makefile = fopen(script->path_makefile, "w")) == NULL)
	{
		EXIT_ON_ERROR("Impossible to create %s", script->path_makefile);
		return;
	}

	fprintf(makefile, "-include perso.mk\n\n");

	fprintf(makefile, "\n/tmp/%s/logs:\n", getenv("USER"));
	fprintf(makefile, "\tmkdir -p $@\n");
	

	snprintf(working_directory, PATH_MAX, "%s/%s", themis.dirname, script->path_prom_deploy);

	if (script->prt == NULL)
	{
		script->is_local = 1;
		strcpy(script->computer,"localhost");
	}
	else
	{
		strcpy(script->computer, script->prt->hosts[script->prt->my_host].name);
		if ((strcmp(script->computer, "localhost") == 0) || (strcmp(script->computer, "127.0.0.1") == 0)) script->is_local = 1;
		else
		{
			if (script->login[0] == 0) EXIT_ON_ERROR("You execute the script on a distant computer, and you do not have set a login.", script->computer);
			script->is_local = 0;
		}
	}

	if (script->is_local)
	{
		fprintf(makefile, "working_directory:=%s\n\n", working_directory);
		fprintf(makefile, "\ndaemon_run_debug: | /tmp/%s/logs\n", getenv("USER"));
		fprintf(makefile, "\tnohup nemiver %s_debug %s %s %s %s %s %s -n%s -b%s -i%s %s --distant-terminal> /tmp/%s/logs/%s.log&\n", script->path_prom_binary, script->path_file_script, script->path_file_config, script->path_file_res, script->path_file_dev, script->path_file_gcd, script->path_file_prt, script->logical_name, themis.ip, themis.id, script->prom_args_line, getenv("USER"), script->logical_name);
		fprintf(makefile, "\ndaemon_run: | /tmp/%s/logs\n", getenv("USER"));
		fprintf(makefile, "\tnohup %s %s %s %s %s %s %s -n%s -b%s -i%s %s --distant-terminal> /tmp/%s/logs/%s.log&\n", script->path_prom_binary, script->path_file_script, script->path_file_config, script->path_file_res, script->path_file_dev, script->path_file_gcd, script->path_file_prt, script->logical_name, themis.ip, themis.id, script->prom_args_line, getenv("USER"), script->logical_name);
		fprintf(makefile, "\nrun_debug:\n");
		fprintf(makefile, "\tnemiver %s_debug %s %s %s %s %s %s -n%s -b%s -i%s %s || echo -e \"\\a\"\n", script->path_prom_binary, script->path_file_script, script->path_file_config, script->path_file_res, script->path_file_dev, script->path_file_gcd, script->path_file_prt, script->logical_name, themis.ip, themis.id, script->prom_args_line);
		fprintf(makefile, "\nrun_valgrind:\n");
		fprintf(makefile, "\tvalgrind %s_debug %s %s %s %s %s %s -n%s -b%s -i%s %s || echo -e \"\\a\"\n", script->path_prom_binary, script->path_file_script, script->path_file_config, script->path_file_res, script->path_file_dev, script->path_file_gcd, script->path_file_prt, script->logical_name, themis.ip, themis.id, script->prom_args_line);
		fprintf(makefile, "\nrun:\n");
		fprintf(makefile, "\t%s %s %s %s %s %s %s -n%s -b%s -i%s %s || echo -e \"\\a\"\n", script->path_prom_binary, script->path_file_script, script->path_file_config, script->path_file_res, script->path_file_dev, script->path_file_gcd, script->path_file_prt, script->logical_name, themis.ip, themis.id, script->prom_args_line);
	}
	else
	{
		if (script->login[0] == 0)
		PRINT_WARNING( "You need to specify a login to connect to a distant computer.");
		else
		{
			if (strncmp(script->path_prom_binary, "promethe", PATH_MAX) == 0) rsh_graphic_option = "-XC";
			else rsh_graphic_option = NULL;

			fprintf(makefile, ".PHONY:mkdir_promnet\n\n");
			fprintf(makefile, "synchronize_files:=%s\n\n", script->synchronize_files);
			fprintf(makefile, "synchronize_directories:=%s\n\n", script->synchronize_directories);

			fprintf(makefile, "mkdir_promnet:\n");
			fprintf(makefile, "\trsh -C %s@%s mkdir -p promnet/%s\n\n", script->login, script->computer, script->logical_name);
			fprintf(makefile, "mkdir_bin_leto_prom:\n");
			fprintf(makefile, "\trsh -C %s@%s mkdir -p bin_leto_prom\n\n", script->login, script->computer);
		   fprintf(makefile, "mkdir_local_lib:\n");
		   fprintf(makefile, "\trsh -C %s@%s mkdir -p .local/lib\n\n", script->login, script->computer);

			if (!script->overwrite_res)
			{
				fprintf(makefile, "%s_upload_promnet:%s mkdir_promnet\n", script->path_file_res, script->path_file_res);
				fprintf(makefile, "\trsync --ignore-existing $< %s@%s:promnet/%s/$<\n\n", script->login, script->computer, script->logical_name);
			}

			fprintf(makefile, "%s_upload_promnet:%s mkdir_promnet\n", script->path_file_gcd, script->path_file_gcd);
         fprintf(makefile, "\trsync --ignore-existing $< %s@%s:promnet/%s/$<\n\n", script->login, script->computer, script->logical_name);

			fprintf(makefile, "%%_upload_promnet:%% mkdir_promnet\n");
			fprintf(makefile, "\trsync -a $< %s@%s:promnet/%s/$<\n\n", script->login, script->computer, script->logical_name);
			fprintf(makefile, "%%_upload_file:%% mkdir_promnet\n");
			fprintf(makefile, "\trsync -a $< %s@%s:promnet/%s/$<\n\n", script->login, script->computer, script->logical_name);
			fprintf(makefile, "%%_upload_directory:%% mkdir_promnet\n");
			fprintf(makefile, "\trsync -a $< %s@%s:promnet/%s/$(<D)\n\n", script->login, script->computer, script->logical_name);
			fprintf(makefile, "all_upload_bin:~/bin_leto_prom/ mkdir_bin_leto_prom ~/.local/lib/libblc.so mkdir_local_lib\n");
			fprintf(makefile, "\trsync -a  $< %s@%s:bin_leto_prom/\n", script->login, script->computer);
         fprintf(makefile, "\trsync -a  ~/.local/lib/libblc.so  %s@%s:.local/lib/libblc.so\n\n", script->login, script->computer);
			fprintf(makefile, "all_upload: all_upload_bin $(foreach file, $(synchronize_files), $(file)_upload_file) $(foreach directory, $(synchronize_directories), $(directory)_upload_directory)");

			makefile_add_upload(makefile, script->path_file_script);
			makefile_add_upload(makefile, script->path_file_config);
			makefile_add_upload(makefile, script->path_file_res);
			makefile_add_upload(makefile, script->path_file_dev);
			makefile_add_upload(makefile, script->path_file_gcd);
			makefile_add_upload(makefile, script->path_file_prt);


			fprintf(makefile, "\ndaemon_run:\n");

			if (rsh_graphic_option == NULL)	fprintf(makefile, "\trsh -Ct %s@%s 'export LD_LIBRARY_PATH=~/.local/lib; mkdir -p /tmp/%s/logs; cd promnet/%s;nohup ~/bin_leto_prom/%s -n%s -i%s %s --distant-terminal ", script->login, script->computer, script->login, script->logical_name, script->path_prom_binary, script->logical_name, /*themis.ip,*/ themis.id, script->prom_args_line);
			else 	fprintf(makefile, "\trsh -XCt %s@%s 'export LD_LIBRARY_PATH=~/.local/lib; mkdir -p /tmp/%s/logs; cd promnet/%s;nohup ~/bin_leto_prom/%s -n%s -i%s %s  ", script->login, script->computer, script->login, script->logical_name, script->path_prom_binary, script->logical_name, /*themis.ip,*/ themis.id, script->prom_args_line);
			
			makefile_add_argument(makefile, script->path_file_script);
			makefile_add_argument(makefile, script->path_file_config);
			makefile_add_argument(makefile, script->path_file_res);
			makefile_add_argument(makefile, script->path_file_dev);
			makefile_add_argument(makefile, script->path_file_gcd);
			makefile_add_argument(makefile, script->path_file_prt);
			fprintf(makefile, " > /tmp/%s/logs/%s.log'&\n", script->login, script->logical_name);
		
			fprintf(makefile, "\nrun_debug:\n");
			fprintf(makefile, "\trsh -XCt %s@%s 'cd promnet/%s;nemiver ~/bin_leto_prom/%s_debug -n%s  -i%s %s", script->login, script->computer, script->logical_name, script->path_prom_binary, script->logical_name, /*themis.ip,*/ themis.id, script->prom_args_line);
			makefile_add_argument(makefile, script->path_file_script);
			makefile_add_argument(makefile, script->path_file_config);
			makefile_add_argument(makefile, script->path_file_res);
			makefile_add_argument(makefile, script->path_file_dev);
			makefile_add_argument(makefile, script->path_file_gcd);
			makefile_add_argument(makefile, script->path_file_prt);
			fprintf(makefile, " '|| echo -e \"\\a\" \n");

			fprintf(makefile, "\nrun_valgrind:\n");
			fprintf(makefile, "\trsh -XCt %s@%s 'cd promnet/%s;valgrind ~/bin_leto_prom/%s_debug -n%s  -i%s %s", script->login, script->computer, script->logical_name, script->path_prom_binary, script->logical_name, /*themis.ip,*/ themis.id, script->prom_args_line);
			makefile_add_argument(makefile, script->path_file_script);
			makefile_add_argument(makefile, script->path_file_config);
			makefile_add_argument(makefile, script->path_file_res);
			makefile_add_argument(makefile, script->path_file_dev);
			makefile_add_argument(makefile, script->path_file_gcd);
			makefile_add_argument(makefile, script->path_file_prt);
			fprintf(makefile, " '|| echo -e \"\\a\" \n");

			fprintf(makefile, "\nrun:\n");

			if (rsh_graphic_option == NULL)
			{
				fprintf(makefile, "\trsh -Ct %s@%s 'cd promnet/%s;~/bin_leto_prom/%s -n%s  -i%s %s ", script->login, script->computer, script->logical_name, script->path_prom_binary, script->logical_name, /*themis.ip,*/ themis.id, script->prom_args_line);
			}
			else 
			{
				fprintf(makefile, "\trsh -XCt %s@%s 'cd promnet/%s;~/bin_leto_prom/%s -n%s  -i%s %s  ", script->login, script->computer, script->logical_name, script->path_prom_binary, script->logical_name, /*themis.ip,*/ themis.id, script->prom_args_line);
			}
			
			makefile_add_argument(makefile, script->path_file_script);
			makefile_add_argument(makefile, script->path_file_config);
			makefile_add_argument(makefile, script->path_file_res);
			makefile_add_argument(makefile, script->path_file_dev);
			makefile_add_argument(makefile, script->path_file_gcd);
			makefile_add_argument(makefile, script->path_file_prt);
			fprintf(makefile, " '|| echo -e \"\\a\"\n");

		}

		fprintf(makefile, "\nshow_log:\n");
		fprintf(makefile, "\trsh -C %s@%s 'cat /tmp/%s/logs/%s.log'\n", script->login, script->computer, script->login, script->logical_name);
	}
	fclose(makefile);
}
