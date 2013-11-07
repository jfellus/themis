#ifndef THEMIS_IVY_CB_H
#define THEMIS_IVY_CB_H

#include "themis_ivy.h"

void ivy_receive_any_message_callback(IvyClientPtr app, void *user_data, int argc, char **argv);
void ivy_here_callback(IvyClientPtr app, void *user_data, int argc, char **argv);
void ivy_status_callback(IvyClientPtr app, void *user_data, int argc, char **argv);
void ivy_msg_rt_callback(IvyClientPtr app, void *remote_control, int argc, char **argv);
void ivy_net_callback(IvyClientPtr app, void *remote_control, int argc, char **argv);
void ivy_exec_callback(IvyClientPtr app, void *remote_control, int argc, char **argv);



#endif
