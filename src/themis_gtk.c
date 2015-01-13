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
 * @author Arnaud Blanchard
 * @date Septembre 2010
 */

#include <stdarg.h>
#include <math.h>
#include <sys/time.h>

#include "themis_gtk.h"
#include "themis_ivy.h"
#include "themis.h"

#define MESSAGE_MAX 256

typedef struct prom_bus_command {
  char command[MAXIMUM_SIZE_OF_PROM_BUS_COMMAND];
} type_prom_bus_command;

type_themis_ui themis_ui;

static gboolean widget_reset_state_timeout_callback(gpointer data)
{
  GtkWidget *widget = (GtkWidget*) data;
  gtk_widget_modify_bg(widget, GTK_STATE_INSENSITIVE, NULL);
  return FALSE;
}

void window_get_size(int *width, int *height)
{
  gtk_window_get_size(GTK_WINDOW(themis_ui.window), width, height);
}

void window_set_size(int width, int height)
{
  gtk_window_resize(GTK_WINDOW(themis_ui.window), width, height);
}

void widget_set_warning(GtkWidget *state_button)
{
  GdkColor color;

  gdk_color_parse("orange", &color);
  gtk_widget_modify_bg(state_button, GTK_STATE_INSENSITIVE, &color);
  g_timeout_add_seconds(1, widget_reset_state_timeout_callback, state_button);
}

void fatal_error(const char *name_of_file, const char* name_of_function, int numero_of_line, const char *message, ...)
{
  char total_message[MESSAGE_MAX];
  GtkWidget *dialog;

  va_list arguments;
  va_start(arguments, message);
  vsnprintf(total_message, MESSAGE_MAX, message, arguments);
  va_end(arguments);

  dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s \t %s \t %i :\n \t Error: %s \n", name_of_file, name_of_function, numero_of_line, total_message);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

/**
 * Envoie un message d'erreur avec name_of_file, name_of_function, number_of_line et affiche le message formate avec les parametres variables.
 * Ajoute l'affichage de l'erreur system errno
 */
void fatal_system_error(const char *name_of_file, const char* name_of_function, int numero_of_line, const char *message, ...)
{
  char total_message[MESSAGE_MAX];
  GtkWidget *dialog;

  va_list arguments;
  va_start(arguments, message);
  vsnprintf(total_message, MESSAGE_MAX, message, arguments);
  va_end(arguments);

  dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s \t %s \t %i :\n \t Error: %s\nSystem error: %s\n", name_of_file, name_of_function, numero_of_line, total_message, strerror(errno));
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

void display_status_message(const char *message, ...)
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

  gtk_statusbar_push(themis_ui.statusbar, STATUSBAR_INFO, full_message);
  va_end(args);
}

/** Tools gtk */
char *text_view_get_all_text(GtkTextView *text_view)
{
  GtkTextIter start_iter, end_iter;
  GtkTextBuffer *text_buffer;

  text_buffer = gtk_text_view_get_buffer(text_view);
  gtk_text_buffer_get_start_iter(text_buffer, &start_iter);
  gtk_text_buffer_get_end_iter(text_buffer, &end_iter);

  return gtk_text_buffer_get_text(text_buffer, &start_iter, &end_iter, TRUE);

}

char *text_buffer_get_all_text(GtkTextBuffer *text_buffer)
{
  GtkTextIter start_iter, end_iter;

  gtk_text_buffer_get_start_iter(text_buffer, &start_iter);
  gtk_text_buffer_get_end_iter(text_buffer, &end_iter);

  return gtk_text_buffer_get_text(text_buffer, &start_iter, &end_iter, TRUE);
}

void ui_remove_scripts()
{
  int i;
  type_script_ui *script_ui;

  for (i = 0; i < themis_ui.number_of_scripts; i++)
  {
    script_ui = &themis_ui.script_uis[i];
    gtk_widget_destroy(GTK_WIDGET(script_ui->detail_window));
    gtk_widget_destroy(GTK_WIDGET(script_ui->frame));
  }
  themis_ui.number_of_scripts = 0;
}

void load_graphics_preferences(Node *tree)
{
  int i;
  Node *script;

  script = xml_get_first_child_with_node_name(tree, "script");
  if (xml_get_int(script, "detail_open") == 1)
  {
    gtk_toggle_button_set_active(themis_ui.script_uis[0].detail_button, TRUE);
    gtk_widget_show_all(GTK_WIDGET(themis_ui.script_uis[0].detail_window));
  }

  for (i = 1; i < themis_ui.number_of_scripts; i++)
  {
    script = xml_get_next_homonymous_sibling(script);

    if (xml_get_int(script, "detail_open") == 1)
    {
      gtk_toggle_button_set_active(themis_ui.script_uis[i].detail_button, TRUE);
      gtk_widget_show_all(GTK_WIDGET(themis_ui.script_uis[i].detail_window));
    }
  }
}

/**************************************** Menu ***************************************************/
void on_menu_item_open_activate(GtkWidget *widget, gpointer user_data)
{
  gchar *filename;
  GtkWidget *dialog;
  GtkFileFilter *file_filter, *generic_file_filter;
  (void) widget;
  (void) user_data;

  dialog = gtk_file_chooser_dialog_new("Open", GTK_WINDOW(themis_ui.window), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  file_filter = gtk_file_filter_new();
  generic_file_filter = gtk_file_filter_new();

  gtk_file_filter_add_pattern(file_filter, "*.net");
  gtk_file_filter_add_pattern(generic_file_filter, "*");

  gtk_file_filter_set_name(file_filter, "coeos/themis (.net)");
  gtk_file_filter_set_name(generic_file_filter, "all types");

  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), file_filter);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), generic_file_filter);

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
  {
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    strncpy(themis.filename, filename, PATH_MAX);
    g_free(filename);

    load(themis.filename);
    display_status_message("File %s loaded.", themis.filename);
  }

  gtk_widget_destroy(dialog);
}

void on_menu_item_save_as_activate(GObject *object, gpointer user_data)
{
  gchar *filename;
  GtkWidget *dialog;
  GtkFileFilter *file_filter, *generic_file_filter;
  (void) object;
  (void) user_data;

  dialog = gtk_file_chooser_dialog_new("Save", GTK_WINDOW(themis_ui.window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);

  if (themis.filename[0] != 0) gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), themis.filename);

  file_filter = gtk_file_filter_new();
  generic_file_filter = gtk_file_filter_new();

  gtk_file_filter_add_pattern(file_filter, "*.net");
  gtk_file_filter_add_pattern(generic_file_filter, "*");

  gtk_file_filter_set_name(file_filter, "coeos/themis (.net)");
  gtk_file_filter_set_name(generic_file_filter, "all types");

  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), file_filter);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), generic_file_filter);

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
  {
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    strncpy(themis.filename, filename, PATH_MAX);
    g_free(filename);

    if (save(themis.filename) == 0) display_status_message("%s saved.", themis.filename);
    else display_status_message("Fail to save %s !!!", themis.filename);
  }
  gtk_widget_destroy(dialog);
}

void on_menu_item_save_activate(GObject *object, gpointer user_data)
{
  if (themis.filename[0] == 0) on_menu_item_save_as_activate(object, user_data);
  else
  {
    if (save(themis.filename) == 0) display_status_message("%s saved.", themis.filename);
    else display_status_message("Fail to save %s !!!", themis.filename);
  }
}

void on_menu_coeos_activate()
{
  char command_line[SIZE_OF_COMMAND_LINE];

  int error;

  snprintf(command_line, SIZE_OF_COMMAND_LINE, "cd %s && coeos %s&\n", themis.dirname, basename(themis.filename));
  printf("Execute: %s", command_line);
  error = system(command_line);
  if (error != 0)
  {
    PRINT_WARNING("Error launching coeos.\n\t Check that %s is correct.", command_line);
  }
}

void select_pandora_file()
{
  gchar *filename;
  GtkWidget *dialog;
  GtkFileFilter *file_filter, *generic_file_filter;

  dialog = gtk_file_chooser_dialog_new("Choose a pandora perspective", GTK_WINDOW(themis_ui.window), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  file_filter = gtk_file_filter_new();
  generic_file_filter = gtk_file_filter_new();

  gtk_file_filter_add_pattern(file_filter, "*.pandora");
  gtk_file_filter_add_pattern(generic_file_filter, "*");

  gtk_file_filter_set_name(file_filter, "pandora (.pandora)");
  gtk_file_filter_set_name(generic_file_filter, "all types");

  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), file_filter);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), generic_file_filter);

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
  {
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    strcpy(themis.pandora, filename);
    g_free(filename);
  }

  gtk_widget_destroy(dialog);
}

void on_menu_pandora_activate()
{
  char command_line[SIZE_OF_COMMAND_LINE];

  int error;

 /* if (themis.pandora[0] == 0) select_pandora_file(); On ne demande pas le fichier par defaut */

  set_relative_path_from_gfile(themis.pandora, themis.dirname, g_file_new_for_path(themis.pandora));

  snprintf(command_line, SIZE_OF_COMMAND_LINE, "pandora -i%s -b%s  %s&\n", themis.id, themis.ip, themis.pandora);
  printf("Execute: %s", command_line);
  error = system(command_line);
  if (error != 0)
  {
    PRINT_WARNING("Error launching pandora.\n\t Check that %s is correct.", command_line);
  }
}


void on_epimethe_button_clicked()
{
  char command_line[SIZE_OF_COMMAND_LINE];
  int error;

  snprintf(command_line, SIZE_OF_COMMAND_LINE, "epimethe %s&\n", themis.id);
  printf("Execute: %s", command_line);
  error = system(command_line);
  if (error != 0)  PRINT_WARNING("Error launching epimethe.\n\t Check that '%s' is correct.", command_line);
}

/************** Preferences *****************/
const char * whitespace_cb_preferences(mxml_node_t *node, int where)
{
  const char *name;

  if (node == NULL)
  {
    fprintf(stderr, "%s \n", __FUNCTION__);
    return NULL;
  }

  name = node->value.element.name;

  if (where == MXML_WS_AFTER_CLOSE || where == MXML_WS_AFTER_OPEN)
  {
    return ("\n");
  }
  if (!strcmp(name, "oscillo_kernel"))
  {
    if (where == MXML_WS_BEFORE_OPEN || where == MXML_WS_BEFORE_CLOSE) return ("  ");
  }
  else if (!strcmp(name, "script_oscillo"))
  {
    if ((where == MXML_WS_BEFORE_OPEN) || (where == MXML_WS_BEFORE_CLOSE)) return ("    ");
  }
  else if (!strcmp(name, "preferences") || !strcmp(name, "script"))
  {
    if (where == MXML_WS_BEFORE_OPEN) return ("  ");
  }
  else if (!strcmp(name, "group"))
  {
    if (where == MXML_WS_BEFORE_OPEN) return ("    ");
  }

  return (NULL);
}

void on_menu_item_save_preferences_activate()
{
  gchar *filename = NULL;
  GtkWidget *dialog;
  GtkFileFilter *file_filter, *generic_file_filter;
 /* Node *script_node; */

  dialog = gtk_file_chooser_dialog_new("Save preferences", GTK_WINDOW(themis_ui.window), GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);

  file_filter = gtk_file_filter_new();
  generic_file_filter = gtk_file_filter_new();

  gtk_file_filter_add_pattern(file_filter, "*.themis");
  gtk_file_filter_add_pattern(generic_file_filter, "*");

  gtk_file_filter_set_name(file_filter, "themis preferences (.themis)");
  gtk_file_filter_set_name(generic_file_filter, "all types");

  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), file_filter);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), generic_file_filter);

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
  {
    strcpy(themis.preferences, filename);
    g_free(filename);
    preferences_save();
  }

  gtk_widget_destroy(dialog);
}

void on_menu_item_load_preferences_activate()
{
  char *filename_the;
  GtkWidget *dialog;
  GtkFileFilter *file_filter, *generic_file_filter;

  dialog = gtk_file_chooser_dialog_new("Load preferences", GTK_WINDOW(themis_ui.window), GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  file_filter = gtk_file_filter_new();
  generic_file_filter = gtk_file_filter_new();

  gtk_file_filter_add_pattern(file_filter, "*.the");
  gtk_file_filter_add_pattern(generic_file_filter, "*");

  gtk_file_filter_set_name(file_filter, "themis preferences (.the)");
  gtk_file_filter_set_name(generic_file_filter, "all types");

  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), file_filter);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), generic_file_filter);

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
  {
    filename_the = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    strncpy(themis.preferences, filename_the, PATH_MAX);
    preferences_load(filename_the);
  }

  gtk_widget_destroy(dialog);
}

void on_menu_item_add_script_activate(GObject *object, gpointer user_data)
{
  (void) object;
  (void) user_data;

  add_new_script("untitled");
}

int on_quit_requested(GtkObject *object, gpointer user_data)
{
  (void) object;
  (void) user_data;

  quit();
  return TRUE;
}

/**
 * Send an ivy message with the content of the entry widget
 */
void on_prom_bus_entry_activate(GtkEntry *entry, gpointer data)
{
  const gchar *command;
  (void) data;

  command = gtk_entry_get_text(entry);
  prom_bus_send_message("%s\n", command);
  gtk_entry_set_text(entry, "");

}

/*** Ivy commands ****/
void on_standby_all_button_clicked(GtkWidget *widget, gpointer user_data)
{
  (void) widget;
  (void) user_data;

  gtk_entry_set_text(themis_ui.prom_bus_entry, "cancel");
  on_prom_bus_entry_activate(themis_ui.prom_bus_entry, NULL);
}


void on_quit_all_promethe_button_clicked(GtkWidget *widget, gpointer user_data)
{
  (void) widget;
  (void) user_data;

  gtk_entry_set_text(themis_ui.prom_bus_entry, "quit");
  on_prom_bus_entry_activate(themis_ui.prom_bus_entry, NULL);
}

void on_refresh_button_clicked(GtkWidget *widget, gpointer user_data)
{
  (void) widget;
  (void) user_data;

  remove_scripts();

  gtk_entry_set_text(themis_ui.prom_bus_entry, "Hello");
  on_prom_bus_entry_activate(themis_ui.prom_bus_entry, NULL);

  if (themis.filename[0] != 0) load(themis.filename);
}


/**************************** Run all scripts *************************************************************************************************/
void on_run_all_button_clicked(GtkWidget *widget, gpointer user_data)
{
	int i;

  (void) widget;
  (void) user_data;
  for ( i = 0; i < themis_ui.number_of_scripts; i++)
  {
	  script_ui_launch(&themis_ui.script_uis[i], 0);

	  sleep((float) atoll(gtk_entry_get_text(themis_ui.waiting_time)));/* a suppimer si on veux enlever le temps d'attente entre lancements des promethes*/
  }

}

/**************************************************** END CALL_BACKS  **************************************************************************/
type_script_ui *ui_get_script_ui_with_id(char *prom_id)
{
  int i;

  for (i = 0; i < themis_ui.number_of_scripts; i++)
  {
    if (strcmp(themis_ui.script_uis[i].data->logical_name, prom_id) == 0)
    {
      return &themis_ui.script_uis[i];
    }
  }
  return NULL;
}

void themis_set_broadcast(type_themis_ui *themis, char *broadcast)
{
  gchar *msg;

  msg = g_strdup_printf("%s", broadcast);
  gtk_statusbar_push(GTK_STATUSBAR(themis->statusbar), STATUSBAR_BROADCAST, msg);
  g_free(msg);
}

/*** Main Window ***/

void ui_set_filename(char *filename)
{
  gtk_window_set_title(GTK_WINDOW(themis_ui.window), filename);
}

void ui_display_prom_bus_message(char *prom_id, char *message)
{
  char time_string[SIZE_OF_TIME];
  time_t now;

  now = time(NULL);
  strftime(time_string, 16, "%Hh%M:", localtime(&now));

  gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(themis_ui.prom_bus_text_buffer), time_string, -1);
  gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(themis_ui.prom_bus_text_buffer), prom_id, -1);
  gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(themis_ui.prom_bus_text_buffer), ": ", -1);
  gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(themis_ui.prom_bus_text_buffer), message, -1);
  gtk_text_buffer_insert_at_cursor(GTK_TEXT_BUFFER(themis_ui.prom_bus_text_buffer), "\n", -1);
}

void ui_init()
{
  char builder_file_name[PATH_MAX];
  GError *g_error = NULL;
  GtkBuilder *builder;
  GtkToolItem *epimethe_button;

  themis_ui.number_of_scripts = 0;

  builder = gtk_builder_new();
  snprintf(builder_file_name, PATH_MAX, "%s/glades/themis.glade", bin_leto_prom_path);
  gtk_builder_add_from_file(builder, builder_file_name, &g_error);
  if (g_error != NULL) EXIT_ON_ERROR("%s", g_error->message);

  themis_ui.window = (GtkWidget *) gtk_builder_get_object(builder, "window");
  themis_ui.display_of_scripts = GTK_BOX(gtk_builder_get_object(builder, "display_of_scripts"));
  themis_ui.prom_bus_entry = GTK_ENTRY(gtk_builder_get_object(builder, "prom_bus_entry"));
  themis_ui.waiting_time = GTK_ENTRY(gtk_builder_get_object(builder, "waiting_time"));
  themis_ui.statusbar = GTK_STATUSBAR((gtk_builder_get_object(builder, "status_bar")));
  themis_ui.prom_bus_text_buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "prom_bus_text_buffer"));
  themis_ui.toolbar = GTK_WIDGET(gtk_builder_get_object(builder, "toolbar"));
  epimethe_button = gtk_tool_button_new (NULL, "Epimethe");
  gtk_container_add(GTK_CONTAINER(themis_ui.toolbar), GTK_WIDGET(epimethe_button));

  gtk_builder_connect_signals(builder, NULL);


  gtk_widget_show_all(themis_ui.window);
  g_signal_connect(epimethe_button, "clicked", on_epimethe_button_clicked, NULL);

  g_object_unref(G_OBJECT(builder));
}
