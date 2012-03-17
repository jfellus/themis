/*
 * themis_ivy_cb.c
 *
 *  Created on: Apr 27, 2011
 *      Author: arnablan
 */

#include "themis_gtk.h"
#include "basic_tools.h"

void ivy_status_callback(IvyClientPtr app, void *user_data, int argc, char **argv)
{
	char *prom_id;
	int state;
	type_script_ui *script_ui;
	(void) argc;
	(void) user_data;

	prom_id = IvyGetApplicationName(app);
	if (sscanf(argv[0], "%d", &state) != 1) PRINT_WARNING("Wrong message: %s", argv[0]); /* %[^,] signifie que l'on prend tous les caracteres sauf la virgule. */

	script_ui = ui_get_script_ui_with_id(prom_id);
	if (script_ui != NULL) script_ui_set_state(script_ui, state);
}

void ivy_msg_rt_callback(IvyClientPtr app, void *remote_control, int argc, char **argv)
{
	char *prom_id;
	int state;
	type_script_ui *script_ui;

	(void) remote_control;
	(void) argc;
	(void) argv;

	prom_id = IvyGetApplicationName(app);
	if (sscanf(argv[0], "%d", &state) != 1) PRINT_WARNING("Wrong message format: %s", argv[0]); /* %[^,] signifie que l'on prend tous les caracteres suaf la virgule. */
	script_ui = ui_get_script_ui_with_id(prom_id);
	widget_set_warning(script_ui->rt_state_button);

}

void ivy_net_callback(IvyClientPtr app, void *user_data, int argc, char **argv)
{
	char *prom_id;
	char group_name[SIZE_OF_GROUP_NAME];
	type_script_ui *script_ui;
  unsigned long error_flag;
	(void) user_data;
	(void) argc;
	(void) argv;

	prom_id = IvyGetApplicationName(app);

  if (sscanf(argv[0], "%[^,],%lu", group_name, &error_flag) != 2) PRINT_WARNING("Wrong format of message: %s\n", argv[0]);
	script_ui = ui_get_script_ui_with_id(prom_id);
	widget_set_warning(script_ui->net_state_button);
}

void ivy_receive_any_message_callback(IvyClientPtr app, void *user_data, int argc, char **argv)
{
	type_script_ui *script_ui;
	char *prom_id;
	(void) argc;
	(void) user_data;

	prom_id = IvyGetApplicationName(app);
	script_ui = ui_get_script_ui_with_id(prom_id);

	if (script_ui != NULL) script_ui_display_prom_bus_message(script_ui, argv[0]);
	ui_display_prom_bus_message(prom_id, argv[0]);
}

void ivy_here_callback(IvyClientPtr app, void *user_data, int argc, char **argv)
{
	int state, debug_port, kernel_port, console_port;

	char *prom_id, *hostname;

	type_script_ui *script_ui;
	(void) argc;
	(void) user_data;

	prom_id = IvyGetApplicationName(app);
	hostname = IvyGetApplicationHost(app);

	if (sscanf(argv[0], "%d,%d,%d,%d",&state, &kernel_port, &console_port, &debug_port) != 4) PRINT_WARNING("Wrong message format: %s\n\tYour prom_kernel is probably to old (<244)", argv[0]);

	script_ui = ui_get_script_ui_with_id(prom_id);

	if (script_ui == NULL)
	{
		add_new_script(prom_id);
		script_ui = &themis_ui.script_uis[themis_ui.number_of_scripts - 1];
		script_ui_set_prom_id(script_ui, prom_id, hostname);
	}

	if (state != No_Quit)
	{
		gtk_widget_hide(GTK_WIDGET(script_ui->launch_button));
		gtk_widget_show(GTK_WIDGET(script_ui->quit_button));
		gtk_widget_show_all(script_ui->launched_widget);

	}

	script_ui->data->debug_port = debug_port;
	script_ui->data->kernel_port = kernel_port;
	script_ui->data->console_port = console_port;

	script_ui_set_state(script_ui, state);
	script_ui_connect_consoles(script_ui);
	/* Envoi d'eventuelles commande clavier une fois que promethe vient de demmarer*/
	vte_terminal_feed_child(script_ui->console_terminal, script_ui->data->keyboard_input, -1);


}
