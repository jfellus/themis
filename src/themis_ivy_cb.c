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
  t_prom_script *script;
  (void) argc;
  (void) user_data;
  
  prom_id = IvyGetApplicationName(app);
  hostname = IvyGetApplicationIp(app);
  
  
  if (sscanf(argv[0], "%d,%d,%d,%d",&state, &kernel_port, &console_port, &debug_port) != 4) PRINT_WARNING("Wrong message format: %s\n\tYour prom_kernel is probably to old (<244)", argv[0]);
  
  script_ui = ui_get_script_ui_with_id(prom_id);
  
  if (script_ui == NULL)
  {
    add_new_script(prom_id);
    script_ui = &themis_ui.script_uis[themis_ui.number_of_scripts - 1];
    script_ui_set_prom_id(script_ui, prom_id, hostname);
  }
  script=script_ui->data;
  
  if (state != No_Quit)
  {
    gtk_widget_hide(GTK_WIDGET(script_ui->launch_button));
    gtk_widget_show(GTK_WIDGET(script_ui->quit_button));
    gtk_widget_show_all(script_ui->launched_widget);
    
  }
  
  /** if state tells that promethe was not already connected */
  if(script->state>=No_Undef) {
    
    script->debug_port = debug_port;
    script->kernel_port = kernel_port;
    script->console_port = console_port;
    
    script_ui_set_state(script_ui, state);
/*    script_ui_connect_consoles(script_ui);*/
    /** Envoi d'eventuelles commande clavier une fois que promethe vient de demmarer
    vte_terminal_feed_child(script_ui->console_terminal, script->keyboard_input, -1);*/
  }
  else {
    if(script_ui->data->state != state) {
      printf("The prom state of %s has changed. Update state\n",script->logical_name);
      script_ui_set_state(script_ui, state);
    }
    if(script->debug_port != debug_port || script->kernel_port != kernel_port || script->console_port!= console_port) {
      PRINT_WARNING("(script %s) : One of the telnet ports seems to have changed while promethe was connected.\n Assume failure of notification to themis and open new telnet ports now",script->logical_name);
      script->debug_port = debug_port;
      script->kernel_port = kernel_port;
      script->console_port = console_port;
      
      script_ui_set_state(script_ui, state);
      /*script_ui_connect_consoles(script_ui);*/
      /** Envoi d'eventuelles commande clavier une fois que promethe vient de demmarer
      vte_terminal_feed_child(script_ui->console_terminal, script->keyboard_input, -1);*/
    }
  }
}
