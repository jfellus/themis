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
 * themis_gtk.h
 *
 *  Created on: 15 avr. 2011
 *      Author: arnablan
 */

#ifndef THEMIS_GTK_H
#define THEMIS_GTK_H

#include <gio/gio.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <vte/vte.h>

#include "themis.h"
#include <prom_tools/include/oscillo_kernel_display.h>
#include <prom_tools/include/xml_tools.h>

#define NO_BORDER 0
#define THIN_BORDER 3

#define GROUP_NAME_SIZE 64
#define VALUE_NAME_SIZE 64
#define MAXIMUM_SIZE_OF_PROM_BUS_COMMAND 64
#define SIZE_OF_TITLE 128
#define SIZE_OF_STATUS_MESSAGE 128


#define STATUSBAR_BROADCAST 0
#define STATUSBAR_INFO 1

enum {
	RUN_DEBUG=1,
	RUN_VALGRIND
};

typedef struct script_ui
{
	t_prom_script *data;

  GtkWidget *launch_dialog;
  GtkWidget *state_displays[NUMBER_OF_POSSIBLE_STATES];
  GtkWidget *display_of_commands;
  GtkWidget *display_of_name;
  GtkWidget *display_of_status;
  GtkWidget *rt_state_button, *net_state_button;
  GtkWidget *color_status;
  GtkWidget *promethe_button, *network_text_view;
  GtkListStore *stat_store;
  GtkFrame *frame;
  GtkWindow *detail_window;
  GtkTextBuffer *buffer;
  GtkWidget *entry;
  GtkWidget *stat_notebook;
  GtkWidget *launch_widget;
  GtkLabel  *computer_label;

  GtkButton *launch_button, *quit_button;
  GtkToggleButton *detail_button;

  GtkFileChooser *script_chooser, *config_chooser, *res_chooser, *dev_chooser, *gcd_chooser, *prt_chooser;
  GtkEntry *script_entry, *config_entry, *res_entry, *dev_entry,*gcd_entry, *prt_entry, *broadcast_entry;

  GtkFileChooser *path_chooser;
  GtkEntry *path_entry;
  GtkEntry *arguments_entry, *blind_arguments_entry;

  GtkWidget *launched_widget;
  GtkEntry *name_entry, *binary_entry;

  GtkEntry *computer_entry, *login_entry, *distant_directory_entry;
  VteTerminal *terminal/*, *debug_terminal, *kernel_terminal, *console_terminal*/;
  GtkTextBuffer *keyboard_input_text_buffer;


  GtkEntry *prom_bus_entry;
  GtkTextBuffer *prom_bus_text_buffer, *readme_text_buffer, *command_text_buffer;

  GtkCheckButton *overwrite_res_check_button;

  GtkNotebook *notebook;

  GtkTextBuffer *synchronize_files_text_buffer;
  GtkTextBuffer *synchronize_directories_text_buffer;

  GtkVBox *synchronize_files_vbox;

  GtkButton *upload_button;

  GtkStatusbar *statusbar;
/*  type_plot *plot; */

}type_script_ui;

typedef struct themis_ui {
  /*** Promethe ***/
  type_script_ui script_uis[MAX_NUMBER_OF_SCRIPTS];
  GtkBox *display_of_scripts;
  int number_of_scripts;

  /*** Themis ***/
  GtkWidget *window;
  GtkEntry *prom_bus_entry;
  GtkEntry *waiting_time;/* a suppimer si on veux enlever le temps d'attente entre lancements des promethes*/
  GtkStatusbar *statusbar;
  GtkBuilder *builder;

  GtkTextBuffer *prom_bus_text_buffer;
  GtkWidget *toolbar;

} type_themis_ui;

extern type_themis_ui themis_ui;

/* Tools */
char *text_view_get_all_text(GtkTextView *text_view);
char *text_buffer_get_all_text(GtkTextBuffer *text_buffer);
int set_relative_path_from_gfile(char *pathname, char *reference_path, GFile *file);
void widget_set_warning(GtkWidget *widget);
void load_graphics_preferences(Node *tree);

/*script_ui */
void script_ui_display_prom_bus_message(type_script_ui *script_ui, char *message);
void ui_script_init(type_script_ui *script_ui, t_prom_script *script);
void script_ui_display_state(type_script_ui *ui, int new_state);
void script_ui_set_state(type_script_ui *script_ui, int state);
void set_warning(GtkWidget *state_button);

void script_ui_set_prom_id(type_script_ui *script_ui, char *prom_id, char *hostname);
void script_ui_update_data(type_script_ui *script_ui, char *reference_pathname);
void script_ui_connect_consoles(type_script_ui *script_ui);
void script_ui_launch(type_script_ui *script_ui, int is_debug);


void window_get_size(int *width, int *height);
void window_set_size(int width, int height);


type_script_ui *get_script_ui_with_id(char *id)  ;
void ui_set_filename(char *filename);
void ui_init();
void ui_add_computer(t_computer *computer);
void ui_display_prom_bus_message(char *prom_id, char *message);
type_script_ui *ui_get_script_ui_with_id(char *prom_id);

/* script_ui_callbacks */
void on_vte_terminal_beep(GObject *object, type_script_ui *script_ui);
void on_compile_button_clicked(GtkButton *button, type_script_ui  *script_ui);
void on_file_entry_activate(GObject *object, type_script_ui  *script_ui);
void on_file_chooser_set(GObject *object, type_script_ui  *script_ui);
void on_launch_button_clicked(GtkWidget *widget, type_script_ui *script_ui);
void on_show_log_button_clicked(GtkWidget *widget, type_script_ui *script_ui);
void on_run_all_button_clicked(GtkWidget *widget, gpointer user_data);


#endif /* THEMIS_GTK_H */
