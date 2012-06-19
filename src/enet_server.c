/*
 *
 *  Created on: 9 mars 2011
 *      Author: Arnaud Blanchard
 */

#include <gtk/gtk.h>
#include <enet/include/enet/enet.h>
#include "prom_tools/include/basic_tools.h"
#include "net_message_debug_dist.h"
#include "prom_kernel/include/prom_enet_debug.h"
#include "prom_tools/include/oscillo_kernel_display.h"
#include "reseau.h"
#include "enet_server.h"
#include "themis_ivy.h"
#include "themis.h"

#define ENET_MAX_NUMBER_OF_CLIENTS 32

int nbre_groupe;
ENetHost *enet_server = NULL;

void on_oscillo_kernel_start_button_clicked(GtkWidget *widget, void *data)
{
	(void) widget;
	(void) data;

	if (enet_server== NULL)
	{
		init_oscillo_kernel(1234); /* Port 1234 */
		oscillo_kernel_create_window();
	}

	prom_bus_send_message("distant_debug(%d)", DISTANT_DEBUG_START);
	oscillo_kernel_activated=1;
}

void enet_manager(ENetHost *server)
{
	char ip[HOST_NAME_MAX];
	char *logical_name;
	int running = 1;
	int number_of_groups;
	ENetEvent event;
	type_com_groupe *com_def_groupe;
	type_nn_message *network_data;
	type_profiler *profiler;
	GtkWidget *check_button_promethe;

	while (running)
	{
		/* Wait up to 100 milliseconds for an event. */
		while (enet_host_service(server, &event, 100) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				enet_address_get_host_ip(&event.peer->address, ip, HOST_NAME_MAX);
				printf("A new client connected from ip %s:%i.\n", ip, event.peer->address.port);
				event.peer->data = NULL;
				break;

			case ENET_EVENT_TYPE_RECEIVE:
				switch (event.channelID)
				{
					case ENET_DEF_GROUP_CHANNEL:
						logical_name = (char*)event.packet->data;
						com_def_groupe = (type_com_groupe*)&((char*)event.packet->data)[LOGICAL_NAME_MAX];
						number_of_groups = (event.packet->dataLength - LOGICAL_NAME_MAX) / sizeof(type_com_groupe);

						gdk_threads_enter();

						profiler = oscillo_kernel_add_promethe(com_def_groupe, number_of_groups, logical_name, themis.preferences);
					    check_button_promethe = gtk_toggle_button_new_with_label(profiler->logical_name);
					    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_button_promethe), TRUE);
					    gtk_widget_set_size_request(check_button_promethe, 300, -1);
					    g_signal_connect(check_button_promethe, "toggled", G_CALLBACK(on_check_button_promethe_activate), profiler);
						gtk_box_pack_start(GTK_BOX(profiler->button_box), check_button_promethe, FALSE, FALSE, 0);
						gtk_widget_show_all(profiler->button_box);

						event.peer->data = profiler;

						gdk_threads_leave();
						break;

					case ENET_GROUP_EVENT_CHANNEL:
						if (event.peer->data != NULL)
						{
							network_data = (type_nn_message*)event.packet->data;
							group_profiler_update_info(event.peer->data, network_data->gpe, network_data->type_message, network_data->time_stamp);
						}
						break;
				}
				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(event.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%s disconnected.\n", (char*) event.peer->data);
				/* Reset the peer's client information. */
				event.peer->data = NULL;
				break;

			case ENET_EVENT_TYPE_NONE:
				printf("ENET: none event \n");
				break;
			}
		}
	}
}

void stop_oscillo_kernel()
{
	enet_host_destroy(enet_server);
	enet_deinitialize();
}

void init_oscillo_kernel(int port)
{
	char host_name[HOST_NAME_MAX];
	ENetAddress address;
	int error;
	pthread_t enet_thread;

	if (enet_initialize() != 0)
	{
		EXIT_ON_ERROR("An error occurred while initializing ENet.\n");
	}

	enet_time_set(0);

	address.host = ENET_HOST_ANY;
	address.port = port;

	enet_server = enet_host_create(&address, ENET_MAX_NUMBER_OF_CLIENTS, 	ENET_NUMBER_OF_CHANNELS, ENET_UNLIMITED_BANDWITH, ENET_UNLIMITED_BANDWITH);

	if (enet_server != NULL)
	{
		gethostname(host_name, HOST_NAME_MAX);
		/* prom_bus_send_message("connect_profiler(%s:%d)", host_name, port);*/
		error = pthread_create(&enet_thread, NULL, (void*(*)(void*)) enet_manager, enet_server);
	}
	else EXIT_ON_ERROR("Fail to create a enet server for oscillo kernel !\n\tCheck that there is no other themis running.");

}
