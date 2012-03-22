/**
 *  Created on: Sep 28, 2010
 *      Author: Arnaud Blanchard
 */

#include <stdio.h>
#include <stdlib.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <vte/vte.h>

#include "themis_gtk.h"

gboolean filter(GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data)
{
  const char *text_of_filter = user_data;
  gchar *name;

  gtk_tree_model_get(model, iter, 1, &name, -1);
  if (name != NULL) return (strstr(name, text_of_filter) != NULL); /* Si je trouve text_of_filter dans name je renvoie TRUE */
  else return FALSE;
}

void on_promethe_exit(VteTerminal *terminal, type_script_ui *script_ui)
{
  GdkColor color;

  if (vte_terminal_get_child_exit_status(terminal) != 0)
  {
    gdk_color_parse("red", &color);
    gtk_widget_modify_bg(script_ui->state_displays[No_Quit], GTK_STATE_INSENSITIVE, &color);
  }
  else
  {
    gdk_color_parse("yellow", &color);
    gtk_widget_modify_bg(script_ui->state_displays[No_Quit], GTK_STATE_INSENSITIVE, &color);
  }

  gtk_widget_show(GTK_WIDGET(script_ui->launch_button));
  gtk_widget_hide(GTK_WIDGET(script_ui->quit_button));
  gtk_widget_hide(script_ui->launched_widget);
}

void on_script_prom_bus_entry_activate(GtkEntry *entry, type_script_ui* script_ui)
{
  const char *command;

  command = gtk_entry_get_text(entry);
  prom_bus_send_message("%s %s", command, script_ui->data->logical_name);
  gtk_entry_set_text(entry, "");
}

void on_file_entry_activate(GObject *object, type_script_ui *script_ui)
{
  char *filename;
  char fullname[PATH_MAX];

  GtkFileChooser *file_chooser;

  file_chooser = g_object_get_data(object, "file_chooser");
  filename = g_object_get_data(object, "filename");

  strncpy(filename, gtk_entry_get_text(GTK_ENTRY(object)), PATH_MAX);
  snprintf(fullname, PATH_MAX, "%s/%s/%s", themis.dirname, script_ui->data->path_prom_deploy, filename);
  gtk_file_chooser_set_filename(file_chooser, fullname);
}

void on_file_chooser_set(GObject *object, type_script_ui *script_ui)
{
  GFile *file;
  GtkEntry *entry;
  char *filename;
  char reference_path[PATH_MAX];

  entry = g_object_get_data(object, "entry");
  filename = g_object_get_data(object, "filename");

  file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(object));

  if (file != NULL)
  {
    snprintf(reference_path, PATH_MAX, "%s/%s", themis.dirname, script_ui->data->path_prom_deploy);
    if (set_relative_path_from_gfile(filename, reference_path, file))
    {
      gtk_entry_set_text(entry, filename);
    }
  }
}

void on_path_chooser_current_folder_changed(GtkFileChooser *path_chooser, type_script_ui *script_ui)
{
  char readme_path_name[PATH_MAX];
  char *readme_buffer;
  GFile *working_dir_file;
  GError *g_error = NULL;

  working_dir_file = gtk_file_chooser_get_file(path_chooser);

  if (set_relative_path_from_gfile(script_ui->data->path_prom_deploy, themis.dirname, working_dir_file))
  {
    gtk_entry_set_text(script_ui->path_entry, script_ui->data->path_prom_deploy);
  }

  gtk_file_chooser_set_current_folder_file(script_ui->script_chooser, working_dir_file, NULL);
  gtk_file_chooser_set_current_folder_file(script_ui->config_chooser, working_dir_file, NULL);
  gtk_file_chooser_set_current_folder_file(script_ui->res_chooser, working_dir_file, NULL);
  gtk_file_chooser_set_current_folder_file(script_ui->dev_chooser, working_dir_file, NULL);
  gtk_file_chooser_set_current_folder_file(script_ui->gcd_chooser, working_dir_file, NULL);
  gtk_file_chooser_set_current_folder_file(script_ui->prt_chooser, working_dir_file, NULL);

  snprintf(readme_path_name, PATH_MAX, "%s/%s/README", themis.dirname, script_ui->data->path_prom_deploy);
  if (g_file_get_contents(readme_path_name, &readme_buffer, NULL, &g_error) != FALSE)
  {
    gtk_text_buffer_set_text(script_ui->readme_text_buffer, readme_buffer, -1);
  }
  else gtk_text_buffer_set_text(script_ui->readme_text_buffer, "", -1);

  g_free(readme_buffer);
}

void on_change_filter(GtkWidget *widget, gpointer user_data)
{
  const gchar *text_of_filter;
  GtkTreeModelFilter *local_model = user_data;

  text_of_filter = gtk_entry_get_text(GTK_ENTRY(widget));
  gtk_tree_model_filter_set_visible_func(local_model, filter, (gpointer) text_of_filter, NULL);
  gtk_tree_model_filter_refilter(local_model);
}

void display_widget_callback(GtkWidget *button, gpointer *user_data)
{
  GtkWidget *widget = (GtkWidget*) user_data;

  (void) button;
  gtk_widget_show_all(widget);
}

void hide_widget_callback(GtkWidget *button, gpointer *user_data)
{
  GtkWidget *widget = (GtkWidget*) user_data;
  (void) button;

  gtk_widget_hide_all(widget);
}

void on_name_entry_activate(GtkEntry *name_entry, type_script_ui* script_ui)
{
  strncpy(script_ui->data->logical_name, gtk_entry_get_text(name_entry), MAX_LOGICAL_NAME);
  gtk_frame_set_label(script_ui->frame, script_ui->data->logical_name);
  gtk_window_set_title(script_ui->detail_window, script_ui->data->logical_name);
}

void on_learn_button_clicked(GtkWidget *widget, type_script_ui *script_ui)
{
  (void) widget;
  gtk_entry_set_text(script_ui->prom_bus_entry, "learn");
  on_script_prom_bus_entry_activate(script_ui->prom_bus_entry, script_ui);
}

void on_continue_learning_button_clicked(GtkWidget *widget, type_script_ui *script_ui)
{
  (void) widget;
  gtk_entry_set_text(script_ui->prom_bus_entry, "continue_learning");
  on_script_prom_bus_entry_activate(script_ui->prom_bus_entry, script_ui);
}

void on_use_only_button_clicked(GtkWidget *widget, type_script_ui *script_ui)
{
  (void) widget;
  gtk_entry_set_text(script_ui->prom_bus_entry, "use");
  on_script_prom_bus_entry_activate(script_ui->prom_bus_entry, script_ui);
}

void on_continue_using_button_clicked(GtkWidget *widget, type_script_ui *script_ui)
{
  (void) widget;
  gtk_entry_set_text(script_ui->prom_bus_entry, "continue_using");
  on_script_prom_bus_entry_activate(script_ui->prom_bus_entry, script_ui);
}

void on_cancel_button_clicked(GtkWidget *widget, type_script_ui *script_ui)
{
  (void) widget;
  gtk_entry_set_text(script_ui->prom_bus_entry, "cancel");
  on_script_prom_bus_entry_activate(script_ui->prom_bus_entry, script_ui);
}

void on_save_res_button_clicked(GtkWidget *widget, type_script_ui *script_ui)
{
  (void) widget;
  gtk_entry_set_text(script_ui->prom_bus_entry, "save");
  on_script_prom_bus_entry_activate(script_ui->prom_bus_entry, script_ui);
}

/** Activated when we launch the script */
void on_launch_button_clicked(GtkWidget *widget, type_script_ui *script_ui)
{
  (void) widget;

  script_ui_launch(script_ui, 0);
}

/** Activated to launch the script in debug mode
 * @todo Mutualiser avec le lancement normale.
 * */
void on_launch_debug_button_clicked(GtkWidget *widget, type_script_ui *script_ui)
{
  (void) widget;
  script_ui_launch(script_ui, 1); /*debug*/
}

void on_upload_button_clicked(GtkWidget *widget, type_script_ui *script_ui)
{
  char command_line[SIZE_OF_COMMAND_LINE];
  char working_directory[PATH_MAX];
  t_prom_script *script;
  GdkColor color;
  (void) widget;


  script_ui_update_data(script_ui, themis.dirname);
  script = script_ui->data;
  script_create_makefile(script);

  snprintf(working_directory, PATH_MAX, "%s/%s", themis.dirname, script->path_prom_deploy);

  gdk_color_parse("grey", &color);
  gtk_widget_modify_bg(GTK_WIDGET(script_ui->state_displays[No_Quit]), GTK_STATE_INSENSITIVE, &color);

  snprintf(command_line, SIZE_OF_COMMAND_LINE, "cd %s && make --jobs --file=%s all_upload||echo -e \"\\a\"\n", working_directory, script->path_makefile);
  vte_terminal_feed_child(script_ui->terminal, command_line, -1);

  gtk_notebook_set_current_page(script_ui->notebook, 1);
}


void on_path_entry_activate(GtkEntry *path_entry, type_script_ui *script_ui)
{
  char dirname[PATH_MAX];

  strncpy(script_ui->data->path_prom_deploy, gtk_entry_get_text(path_entry), PATH_MAX);
  snprintf(dirname, PATH_MAX, "%s/%s", themis.dirname, script_ui->data->path_prom_deploy);
  gtk_file_chooser_set_current_folder(script_ui->path_chooser, dirname);
}

void on_quit_button_clicked(GtkWidget *widget, type_script_ui *script_ui)
{
  (void) widget;
  prom_bus_send_message("quit %s", script_ui->data->logical_name);
}

void on_show_log_button_clicked(GtkWidget *widget, type_script_ui *script_ui)
{
  char command_line[SIZE_OF_COMMAND_LINE];
  (void) widget;

  vte_terminal_reset(script_ui->terminal, TRUE, TRUE);

  snprintf(command_line, SIZE_OF_COMMAND_LINE, "make --file=%s show_log\n", script_ui->data->path_makefile);
  vte_terminal_feed_child(script_ui->terminal, command_line, -1);
}

void on_default_names_button_clicked(GtkButton *button, type_script_ui *script_ui)
{
  (void)button;

  script_ui_update_data(script_ui, themis.dirname);
}

void on_leto_button_clicked(GtkButton *button, type_script_ui *script_ui)
{
  const char *argv[MAX_NUMBER_OF_ARGS];
  char command_line[SIZE_OF_COMMAND_LINE];
  char working_directory[PATH_MAX];
  t_prom_script *script;

  int error;

  (void) button;

  script = script_ui->data;

  argv[0] = "leto";
  argv[1] = script->path_file_script;
  argv[2] = script->path_file_res;
  argv[3] = NULL;

  snprintf(working_directory, PATH_MAX, "%s/%s", themis.dirname, script->path_prom_deploy);

  snprintf(command_line, SIZE_OF_COMMAND_LINE, "cd %s; %s/bin_leto_prom/%s %s %s&", working_directory, getenv("HOME"), argv[0], argv[1], argv[2]);
  error = system(command_line);
  if (error != 0)
  {
    PRINT_WARNING("Error launching leto.\n\t Check that %s is correct.", command_line);
  }
}

void on_edit_config_button_clicked(GtkButton *button, type_script_ui *script_ui)
{
  char command_line[SIZE_OF_COMMAND_LINE];
  const char *editor;
  int error;
  t_prom_script *script;
  (void) button;

  script = script_ui->data;
  editor = getenv("EDITOR");
  if (editor == NULL) editor = "gedit";

  snprintf(command_line, SIZE_OF_COMMAND_LINE, "%s %s/%s/%s &", editor, themis.dirname, script->path_prom_deploy, script->path_file_config);
  error = system(command_line);
  printf("%s\n", command_line);
  if (error != 0)
  {
    PRINT_WARNING("Error launching gedit.\n\t Check that %s is correct.", command_line);
  }
}

void on_compile_button_clicked(GtkButton *button, type_script_ui *script_ui)
{
  char command_line[SIZE_OF_COMMAND_LINE];
  int error;
  t_prom_script *script;
  (void) button;

  script = script_ui->data;

  snprintf(command_line, SIZE_OF_COMMAND_LINE, "echo %d | %s/bin_leto_prom/cc_leto %s/%s/%s %s/%s/%s", g_random_int(), getenv("HOME"), themis.dirname, script->path_prom_deploy, script->path_file_script, themis.dirname, script->path_prom_deploy, script->path_file_res);
  printf("%s\n", command_line);
  error = system(command_line);
  if (error != 0)
  {
    PRINT_WARNING("Error launching cc_leto.\n\t Check that %s is correct.", command_line);
  }
}

void on_edit_dev_button_clicked(GtkButton *button, type_script_ui *script_ui)
{
  char command_line[SIZE_OF_COMMAND_LINE];
  const char *editor;
  int error;
  t_prom_script *script;
  (void) button;

  script = script_ui->data;
  editor = getenv("EDITOR");
  if (editor == NULL) editor = "gedit";
  snprintf(command_line, SIZE_OF_COMMAND_LINE, "%s %s/%s/%s &", editor, themis.dirname, script->path_prom_deploy, script->path_file_dev);
  printf("%s\n", command_line);
  error = system(command_line);
  if (error != 0)
  {
    PRINT_WARNING("Error launching emacs.\n\t Check that %s is correct.", command_line);
  }
}

void on_edit_prt_button_clicked(GtkButton *button, type_script_ui *script_ui)
{
  char command_line[SIZE_OF_COMMAND_LINE];
  const char *editor;
  int error;
  t_prom_script *script;
  (void) button;

  script = script_ui->data;
  editor = getenv("EDITOR");
  if (editor == NULL) editor = "gedit";
  snprintf(command_line, SIZE_OF_COMMAND_LINE, "%s %s/%s/%s &", editor, themis.dirname, script->path_prom_deploy, script->path_file_prt);
  printf("%s\n", command_line);
  error = system(command_line);
  if (error != 0)
  {
    PRINT_WARNING("Error launching the editor.\n\t Check that %s is correct.", command_line);
  }
}

void on_edit_readme_button_clicked(GtkButton *button, type_script_ui *script_ui)
{
  char command_line[SIZE_OF_COMMAND_LINE];
  const char *editor;
  int error;
  t_prom_script *script;
  (void) button;

  script = script_ui->data;
  editor = getenv("EDITOR");
  if (editor == NULL) editor = "gedit";
  snprintf(command_line, SIZE_OF_COMMAND_LINE, "%s %s/%s/README &", editor, themis.dirname, script->path_prom_deploy);
  printf("%s\n", command_line);
  error = system(command_line);
  if (error != 0)
  {
    PRINT_WARNING("Error launching gedit.\n\t Check that %s is correct.", command_line);
  }
}

void on_detail_button_toggled(GtkToggleButton *button, type_script_ui *script_ui)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
  {
    gtk_widget_show_all(GTK_WIDGET(script_ui->detail_window));
    if (script_ui->data->state == No_Undef) gtk_notebook_set_current_page(script_ui->notebook, 0);
  }
  else
  {
    gtk_widget_hide_all(GTK_WIDGET(script_ui->detail_window));
  }
}

int on_detail_window_delete_event(GtkWidget *object, GdkEvent *event, type_script_ui *script_ui)
{
  (void) object;
  (void) event;

  gtk_toggle_button_set_active(script_ui->detail_button, FALSE);
  return TRUE; /* Stop the event (do not destroy the window) */
}

void on_vte_terminal_beep(GObject *object, type_script_ui *script_ui)
{
  (void)object;

  script_ui_set_state(script_ui, No_Error);
}


