/*
 * script_gtk.c
 *
 *  Created on: 12 avr. 2011
 *      Author: Arnaud Blanchard
 */
#include "themis_gtk.h"
#include <read_prt.h>

#define PORTNAME_MAX 256

void script_ui_display_prom_bus_message(type_script_ui *script_ui, char *message)
{
	char time_string[SIZE_OF_TIME];
	time_t now;

	now = time(NULL);
	strftime(time_string, 16, "%Hh%M:", localtime(&now));

	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(script_ui->prom_bus_text_buffer), time_string, -1);
	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(script_ui->prom_bus_text_buffer), message, -1);
	gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(script_ui->prom_bus_text_buffer), "\n", -1);
}

void script_ui_set_state(type_script_ui *script_ui, int new_state)
{
	GdkColor color;

	if (script_ui->data->state != No_Quit && script_ui->data->state != No_Undef)
	{
		gdk_color_parse("grey", &color);
		gtk_widget_modify_bg(GTK_WIDGET(script_ui->state_displays[script_ui->data->state]), GTK_STATE_INSENSITIVE, &color);
	}
	else gtk_notebook_set_current_page(script_ui->notebook, 1);

	if (new_state != No_Quit)
	{
		gdk_color_parse("green", &color);
		gtk_widget_modify_bg(GTK_WIDGET(script_ui->state_displays[No_Quit]), GTK_STATE_INSENSITIVE, &color);
	}

	switch (new_state)
	{
	case No_Learn_and_Use:
	case No_Use_Only:
		gdk_color_parse("green", &color);
		gtk_widget_modify_bg(GTK_WIDGET(script_ui->state_displays[new_state]), GTK_STATE_INSENSITIVE, &color);
		break;
	case No_Not_Running:
		gdk_color_parse("yellow", &color);
		gtk_widget_modify_bg(GTK_WIDGET(script_ui->state_displays[new_state]), GTK_STATE_INSENSITIVE, &color);
		break;

	case No_Quit:
		gtk_widget_hide(GTK_WIDGET(script_ui->quit_button));
		gtk_widget_show(GTK_WIDGET(script_ui->launch_button));
		gtk_widget_hide_all(script_ui->launched_widget);

		gdk_color_parse("yellow", &color);
		gtk_widget_modify_bg(GTK_WIDGET(script_ui->state_displays[No_Quit]), GTK_STATE_INSENSITIVE, &color);
		break;
	}

	script_ui->data->state = new_state;
}

void script_ui_set_prom_id(type_script_ui *script_ui, char *prom_id, char *hostname)
{
	GdkColor color;

	strncpy(script_ui->data->logical_name, prom_id, SIZE_OF_PROM_ID);

	gdk_color_parse("green", &color);
	gtk_widget_modify_bg(script_ui->state_displays[No_Quit], GTK_STATE_INSENSITIVE, &color);

	strncpy(script_ui->data->computer, hostname, MAX_COMPUTER);
	gtk_label_set_text(script_ui->computer_label, script_ui->data->computer);
}

void script_ui_set_name(type_script_ui *ui, const char *name)
{
	gtk_entry_set_text(ui->name_entry, name);
}

void script_ui_connect_consoles(type_script_ui *ui)
{
	char *argv[5];
	char port[PORTNAME_MAX];
	pid_t pid;

	t_prom_script * script = ui->data;

	argv[0] = (char*) "rlwrap";
	argv[1] = (char*) "telnet";
	argv[2] = script->computer;
	argv[3] = port;
	argv[4] = NULL;

	snprintf(argv[3], PORTNAME_MAX, "%d", script->kernel_port);
	pid = vte_terminal_fork_command(ui->kernel_terminal, "rlwrap", argv, NULL, NULL, FALSE, FALSE, FALSE);
	if (pid == -1) EXIT_ON_ERROR("Impossible to connect to kernel port. Check that you have rlwrap and telnet.");
	else vte_terminal_reset(ui->kernel_terminal, TRUE, TRUE);

	snprintf(argv[3], PORTNAME_MAX, "%d", script->debug_port);
	pid = vte_terminal_fork_command(ui->debug_terminal, "rlwrap", argv, NULL, NULL, FALSE, FALSE, FALSE);
	if (pid == -1) EXIT_ON_ERROR("Impossible to connect to debug port. Check that you have rlwrap and telnet.");
	else vte_terminal_reset(ui->debug_terminal, TRUE, TRUE);

	snprintf(argv[3], PORTNAME_MAX, "%d", script->console_port);
	pid = vte_terminal_fork_command(ui->console_terminal, "rlwrap", argv, NULL, NULL, FALSE, FALSE, FALSE);
	if (pid == -1) EXIT_ON_ERROR("Impossible to connect to console port. Check that you have rlwrap and telnet.");
	else vte_terminal_reset(ui->console_terminal, TRUE, TRUE);

	on_show_log_button_clicked(NULL, ui);
}

void script_ui_display_status(type_script_ui *script_ui, const char *message, ...)
{
	va_list args;
	char full_message[SIZE_OF_STATUS_MESSAGE];
	char temporary_message[SIZE_OF_STATUS_MESSAGE];

	time_t now;
	va_start(args, message);

	now = time(NULL);
	strftime(full_message, 16, "%Hh%M:", localtime(&now));
	vsprintf(temporary_message, message, args);
	strncat(full_message, temporary_message, SIZE_OF_STATUS_MESSAGE - 1);

	gtk_statusbar_push(script_ui->statusbar, STATUSBAR_INFO, full_message);
	va_end(args);
}

void script_ui_display_command(type_script_ui *script_ui, char *working_directory, char *command)
{
	char full_message[SIZE_OF_COMMAND_LINE];

	time_t now;

	now = time(NULL);
	strftime(full_message, 16, "%Hh%M:", localtime(&now));
	strncat(full_message, working_directory, SIZE_OF_STATUS_MESSAGE - 1);
	gtk_text_buffer_insert_at_cursor(script_ui->command_text_buffer, full_message, -1);
	gtk_text_buffer_insert_at_cursor(script_ui->command_text_buffer, "/\n", -1);
	gtk_text_buffer_insert_at_cursor(script_ui->command_text_buffer, command, -1);
	gtk_text_buffer_insert_at_cursor(script_ui->command_text_buffer, "\n", -1);
}

void set_filename_field(type_script_ui *script_ui, GtkEntry *entry, GtkFileChooser *file_chooser, char *filename, const char *filter_text)
{
	char pathname[PATH_MAX];

	GtkFileFilter *file_filter, *generic_file_filter;

	file_filter = gtk_file_filter_new();
	generic_file_filter = gtk_file_filter_new();

	gtk_file_filter_add_pattern(file_filter, filter_text);
	gtk_file_filter_add_pattern(generic_file_filter, "*");

	gtk_file_filter_set_name(file_filter, filter_text);
	gtk_file_filter_set_name(generic_file_filter, "all types");

	gtk_file_chooser_add_filter(file_chooser, file_filter);
	gtk_file_chooser_add_filter(file_chooser, generic_file_filter);

	gtk_entry_set_text(entry, filename);

	snprintf(pathname, PATH_MAX, "%s/%s/%s", themis.dirname, script_ui->data->path_prom_deploy, filename);
	gtk_file_chooser_set_filename(file_chooser, pathname);

	g_object_set_data(G_OBJECT(entry), "file_chooser", file_chooser);
	g_object_set_data(G_OBJECT(entry), "filename", filename);
	g_object_set_data(G_OBJECT(file_chooser), "entry", entry);
	g_object_set_data(G_OBJECT(file_chooser), "filename", filename);

	g_signal_connect(entry, "activate", G_CALLBACK(on_file_entry_activate), script_ui);
	g_signal_connect(file_chooser, "selection-changed", G_CALLBACK(on_file_chooser_set), script_ui);
}

void script_ui_update_data(type_script_ui *script_ui, gchar *reference_dirname)
{
	char dirname[PATH_MAX];
	char prt_path[PATH_MAX];

	GFile *dir;
	t_prom_script *script;

	script = script_ui->data;

	strncpy(script->login, gtk_entry_get_text(script_ui->login_entry), LOGIN_NAME_MAX);
	strncpy(script->path_prom_deploy, gtk_entry_get_text(script_ui->path_entry), PATH_MAX);
	strncpy(script->logical_name, gtk_entry_get_text(script_ui->name_entry), SIZE_OF_NAME);
	strncpy(script->path_prom_binary, gtk_entry_get_text(script_ui->binary_entry), PATH_MAX);
	strncpy(script->keyboard_input, text_buffer_get_all_text(script_ui->keyboard_input_text_buffer), SIZE_OF_KEYBOARD_INPUT);
	strncpy(script->path_file_script, gtk_entry_get_text(script_ui->script_entry), PATH_MAX);
	strncpy(script->path_file_config, gtk_entry_get_text(script_ui->config_entry), PATH_MAX);
	strncpy(script->path_file_res, gtk_entry_get_text(script_ui->res_entry), PATH_MAX);
	strncpy(script->path_file_dev, gtk_entry_get_text(script_ui->dev_entry), PATH_MAX);
	strncpy(script->path_file_gcd, gtk_entry_get_text(script_ui->gcd_entry), PATH_MAX);
	strncpy(script->path_file_prt, gtk_entry_get_text(script_ui->prt_entry), PATH_MAX);
	strncpy(script->prom_args_line, gtk_entry_get_text(script_ui->arguments_entry), MAX_PROM_ARGS_LINE);

	script->overwrite_res = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(script_ui->overwrite_res_check_button));

	/* Cherche le chemin relatif en fonction de la psotion du fichier.net (reference_path)
	 reference_dirname est la racine commune entre themis.dirname et path_prom_deploy 	 */
	snprintf(dirname, PATH_MAX, "%s/%s", themis.dirname, script_ui->data->path_prom_deploy);
	dir = g_file_new_for_path(dirname);
	if (set_relative_path_from_gfile(script->path_prom_deploy, reference_dirname, dir))
	{
		gtk_entry_set_text(script_ui->path_entry, script->path_prom_deploy);
		if (themis.dirname != reference_dirname) strncpy(themis.dirname, reference_dirname, PATH_MAX); /* On ne fait une copie que si les pointeurs sont différents */
		on_path_entry_activate(script_ui->path_entry, script_ui);
	}
	else PRINT_WARNING("Directory %s not found.", dirname);

	gtk_frame_set_label(script_ui->frame, script->logical_name);

	if (script->path_file_prt[0] != 0)
	{
		snprintf(prt_path, PATH_MAX, "%s/%s/%s", reference_dirname, script_ui->data->path_prom_deploy, script->path_file_prt);
		script->prt = prt_init(prt_path, script->logical_name);
		if (script->prt == NULL ) script_ui_display_status(script_ui, "File prt: %s not found.", prt_path);
	}
	else script->prt = NULL;

	g_object_unref(dir);
}

void ui_script_init(type_script_ui *script_ui, t_prom_script *script)
{
	GError *g_error = NULL;
	GtkBuilder *builder;
	char builder_filename[PATH_MAX];
	char path_name[PATH_MAX];

	builder = gtk_builder_new();
	snprintf(builder_filename, PATH_MAX, "%s/glades/distant_promethe.glade", bin_leto_prom_path);
	gtk_builder_add_from_file(builder, builder_filename, &g_error);
	if (g_error != NULL ) EXIT_ON_ERROR("%s", g_error->message);

	script_ui->data = script;
	script_ui->frame = GTK_FRAME(gtk_builder_get_object(builder, "promethe_frame"));
	script_ui->statusbar = GTK_STATUSBAR(gtk_builder_get_object(builder, "statusbar"));

	script_ui->detail_window = GTK_WINDOW(gtk_builder_get_object(builder, "detail_window"));
	script_ui->notebook = GTK_NOTEBOOK(gtk_builder_get_object(builder, "notebook"));
	script_ui->detail_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "detail_button"));
	script_ui->launched_widget = GTK_WIDGET(gtk_builder_get_object(builder, "launched_widget"));
	script_ui->launch_button = GTK_BUTTON(gtk_builder_get_object(builder, "launch_button"));
	script_ui->computer_label = GTK_LABEL(gtk_builder_get_object(builder, "computer_label"));
	script_ui->quit_button = GTK_BUTTON(gtk_builder_get_object(builder, "quit_button"));

	script_ui->state_displays[No_Quit] = GTK_WIDGET(gtk_builder_get_object(builder, "quit_state_button"));
	script_ui->state_displays[No_Learn_and_Use] = GTK_WIDGET(gtk_builder_get_object(builder, "learn_state_button"));
	script_ui->state_displays[No_Use_Only] = GTK_WIDGET(gtk_builder_get_object(builder, "use_only_state_button"));
	script_ui->state_displays[No_Not_Running] = GTK_WIDGET(gtk_builder_get_object(builder, "stanby_state_button"));

	/* Settings */
	script_ui->login_entry = GTK_ENTRY(gtk_builder_get_object(builder, "login_entry"));
	script_ui->path_chooser = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "path_chooser"));
	script_ui->path_entry = GTK_ENTRY(gtk_builder_get_object(builder, "path_entry"));
	script_ui->terminal = VTE_TERMINAL(gtk_builder_get_object(builder, "terminal"));
	script_ui->kernel_terminal = VTE_TERMINAL(gtk_builder_get_object(builder, "kernel_terminal"));
	script_ui->debug_terminal = VTE_TERMINAL(gtk_builder_get_object(builder, "debug_terminal"));
	script_ui->console_terminal = VTE_TERMINAL(gtk_builder_get_object(builder, "console_terminal"));

	script_ui->keyboard_input_text_buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "keyboard_input_text_buffer"));
	script_ui->name_entry = GTK_ENTRY(gtk_builder_get_object(builder, "name_entry"));
	script_ui->binary_entry = GTK_ENTRY(gtk_builder_get_object(builder, "binary_entry"));
	script_ui->script_chooser = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "script_chooser"));
	script_ui->script_entry = GTK_ENTRY(gtk_builder_get_object(builder, "script_entry"));
	script_ui->config_chooser = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "config_chooser"));
	script_ui->config_entry = GTK_ENTRY(gtk_builder_get_object(builder, "config_entry"));
	script_ui->res_chooser = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "res_chooser"));
	script_ui->res_entry = GTK_ENTRY(gtk_builder_get_object(builder, "res_entry"));
	script_ui->overwrite_res_check_button = GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "overwrite_res_check_button"));
	script_ui->dev_chooser = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "dev_chooser"));
	script_ui->dev_entry = GTK_ENTRY(gtk_builder_get_object(builder, "dev_entry"));
	script_ui->gcd_chooser = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "gcd_chooser"));
	script_ui->gcd_entry = GTK_ENTRY(gtk_builder_get_object(builder, "gcd_entry"));
	script_ui->prt_chooser = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "prt_chooser"));
	script_ui->prt_entry = GTK_ENTRY(gtk_builder_get_object(builder, "prt_entry"));
	script_ui->arguments_entry = GTK_ENTRY(gtk_builder_get_object(builder, "arguments_entry"));
	script_ui->prom_bus_text_buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "prom_bus_text_buffer"));
	script_ui->prom_bus_entry = GTK_ENTRY(gtk_builder_get_object(builder, "script_prom_bus_entry"));
	script_ui->readme_text_buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "readme_text_buffer"));

	/* Debug */
	script_ui->command_text_buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "command_text_buffer"));
	gtk_box_pack_start(GTK_BOX(themis_ui.display_of_scripts), GTK_WIDGET(script_ui->frame), 0, 0, THIN_BORDER);

	gtk_frame_set_label(script_ui->frame, script->logical_name);
	gtk_window_set_title(script_ui->detail_window, script->logical_name);

	gtk_entry_set_text(script_ui->login_entry, script->login);
	gtk_entry_set_text(script_ui->name_entry, script->logical_name);
	gtk_entry_set_text(script_ui->path_entry, script->path_prom_deploy);
	gtk_entry_set_text(script_ui->binary_entry, script->path_prom_binary);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(script_ui->overwrite_res_check_button), script->overwrite_res);

	gtk_text_buffer_insert_at_cursor(script_ui->keyboard_input_text_buffer, script->keyboard_input, -1);
	set_filename_field(script_ui, script_ui->script_entry, script_ui->script_chooser, script->path_file_script, "*.script");
	set_filename_field(script_ui, script_ui->config_entry, script_ui->config_chooser, script->path_file_config, "*.config");
	set_filename_field(script_ui, script_ui->res_entry, script_ui->res_chooser, script->path_file_res, "*.res");
	set_filename_field(script_ui, script_ui->dev_entry, script_ui->dev_chooser, script->path_file_dev, "*.dev");
	set_filename_field(script_ui, script_ui->gcd_entry, script_ui->gcd_chooser, script->path_file_gcd, "*.gcd");
	set_filename_field(script_ui, script_ui->prt_entry, script_ui->prt_chooser, script->path_file_prt, "*.prt");
	gtk_entry_set_text(script_ui->arguments_entry, script->prom_args_line);

	snprintf(path_name, PATH_MAX, "%s/%s", themis.dirname, script->path_prom_deploy);
	gtk_file_chooser_set_current_folder(script_ui->path_chooser, path_name);

	gtk_builder_connect_signals(builder, script_ui);
	g_object_unref(G_OBJECT(builder));

	vte_terminal_fork_command(script_ui->terminal, NULL, NULL, NULL, NULL, FALSE, FALSE, FALSE);

	gtk_widget_show_all(GTK_WIDGET(script_ui->frame));
	gtk_widget_hide_all(script_ui->launched_widget);
	gtk_widget_hide(GTK_WIDGET(script_ui->quit_button));

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

void makefile_add_upload(FILE *makefile, char *file_path)
{

	if (file_path[0] != 0)
	{
		fprintf(makefile, " %s_upload_promnet", file_path);
	}
}

void script_ui_launch(type_script_ui *script_ui, int is_debug)
{
	char command_line[SIZE_OF_COMMAND_LINE];
	char working_directory[PATH_MAX];
	char fullname[PATH_MAX];
	const char *rsh_graphic_option;
	GdkColor color;
	GtkWidget *dialog;
	t_prom_script *script;
	pid_t pid;
	FILE *makefile;
	char makefile_name[PATH_MAX];

	script_ui_update_data(script_ui, themis.dirname);
	script = script_ui->data;

	/* Test si le fichier.res exist et sinon propose de le compiler */
	snprintf(fullname, PATH_MAX, "%s/%s/%s", themis.dirname, script->path_prom_deploy, script->path_file_res);

	if (!g_file_test(fullname, G_FILE_TEST_EXISTS))
	{
		dialog = gtk_message_dialog_new(GTK_WINDOW(themis_ui.window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_YES_NO, "The file:\n %s\n\n does not exist, do you want to compile before ?", fullname);

		if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES)
		{
			on_compile_button_clicked(NULL, script_ui);
		}
		gtk_widget_destroy(dialog);
	}

	snprintf(makefile_name, PATH_MAX, "%s/Makefile.%s", themis.tmp_dir, script->logical_name);

	if ((makefile = fopen(makefile_name, "w")) == NULL ) EXIT_ON_ERROR("Impossible to create %s", makefile_name);
	snprintf(working_directory, PATH_MAX, "%s/%s", themis.dirname, script->path_prom_deploy);

	if (script->prt == NULL ) script->is_local = 1;
	else
	{
		strcpy(script->computer, script->prt->hosts[script->prt->my_host].name);
		if ((strcmp(script->computer, "localhost") == 0) || (strcmp(script->computer, "127.0.0.1") == 0 ) || (script->login[0] == 0))  script->is_local = 1;
		else script->is_local = 0;
	}

	if (script->is_local)
	{
		if (is_debug)
		{
			snprintf(command_line, SIZE_OF_COMMAND_LINE, "nohup nemiver %s_debug %s %s %s %s %s %s -n%s -b%s -i%s %s --distant-terminal> /tmp/%s/logs/%s.log&\n", script->path_prom_binary, script->path_file_script, script->path_file_config, script->path_file_res, script->path_file_dev, script->path_file_gcd, script->path_file_prt, script->logical_name, themis.ip, themis.id, script->prom_args_line, getenv("USER"), script->logical_name);
		}
		else
		{
			snprintf(command_line, SIZE_OF_COMMAND_LINE, "nohup %s %s %s %s %s %s %s -n%s -b%s -i%s %s --distant-terminal> /tmp/%s/logs/%s.log&\n", script->path_prom_binary, script->path_file_script, script->path_file_config, script->path_file_res, script->path_file_dev, script->path_file_gcd, script->path_file_prt, script->logical_name, themis.ip, themis.id, script->prom_args_line, getenv("USER"), script->logical_name);
		}

		pid = vte_terminal_fork_command(script_ui->terminal, NULL, NULL, NULL, working_directory, 0, 0, 0);
		vte_terminal_feed_child(script_ui->terminal, command_line, -1);

		gethostname(script->computer, MAX_COMPUTER);
	}
	else
	{
		if (script->login[0] == 0) PRINT_WARNING("You need to specify a login to connect to a distant computer.");
		else
		{
			if (strncpy(script->path_prom_binary, "promethe", PATH_MAX)) rsh_graphic_option = "-X";
			else rsh_graphic_option = "";

			fprintf(makefile, ".PHONY:mkdir_promnet\n\n");
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
			fprintf(makefile, "\trsync  $< %s@%s:promnet/%s/$(<F)\n\n", script->login, script->computer, script->logical_name);
			fprintf(makefile, "%%_upload_binary:~/bin_leto_prom/%% mkdir_bin_leto_prom\n");
			fprintf(makefile, "\trsync  $< %s@%s:bin_leto_prom/$*\n\n", script->login, script->computer);
			fprintf(makefile, "all_upload: %s_upload_binary", script->path_prom_binary);

			makefile_add_upload(makefile, script->path_file_script);
			makefile_add_upload(makefile, script->path_file_config);
			makefile_add_upload(makefile, script->path_file_res);
			makefile_add_upload(makefile, script->path_file_dev);
			makefile_add_upload(makefile, script->path_file_gcd);
			makefile_add_upload(makefile, script->path_file_prt);
			fprintf(makefile, "\n\n");
			fprintf(makefile, "run:all_upload\n");

			if (is_debug)
			{
				fprintf(makefile, "\trsh -X %s@%s cd promnet/%s;nohup nemiver ~/bin_leto_prom/%s_debug -n%s -b%s -i%s %s --distant-terminal> /tmp/%s/logs/%s.log", script->login, script->computer, script->logical_name, script->path_prom_binary, script->logical_name, themis.ip, themis.id, script->prom_args_line, getenv("USER"), script->logical_name);
			}
			else
			{
				fprintf(makefile, "\trsh %s %s@%s cd promnet/%s;nohup ~/bin_leto_prom/%s -n%s -b%s -i%s %s > /tmp/%s/logs/%s.log", rsh_graphic_option, script->login, script->computer, script->logical_name, script->path_prom_binary, script->logical_name, themis.ip, themis.id, script->prom_args_line, getenv("USER"), script->logical_name);
			}

			makefile_add_argument(makefile, script->path_file_script);
			makefile_add_argument(makefile, script->path_file_config);
			makefile_add_argument(makefile, script->path_file_res);
			makefile_add_argument(makefile, script->path_file_dev);
			makefile_add_argument(makefile, script->path_file_gcd);
			makefile_add_argument(makefile, script->path_file_prt);
			fprintf(makefile, "&\n");
			fclose(makefile);

			snprintf(command_line, SIZE_OF_COMMAND_LINE, "make --jobs --file=%s run\n", makefile_name);
			pid = vte_terminal_fork_command(script_ui->terminal, NULL, NULL, NULL, working_directory, 0, 0, 0);
			vte_terminal_feed_child(script_ui->terminal, command_line, -1);

			snprintf(command_line, SIZE_OF_COMMAND_LINE, "rlogin %s %s@%s:promnet/%s\n", rsh_graphic_option, script->login, script->computer, script->logical_name);
		}
	}

	gtk_notebook_set_current_page(script_ui->notebook, 1);

	gdk_color_parse("grey", &color);
	gtk_widget_modify_bg(script_ui->state_displays[No_Quit], GTK_STATE_INSENSITIVE, &color);

}

