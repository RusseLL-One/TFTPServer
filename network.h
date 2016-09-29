//
// Created by root on 26.09.16.
//


#ifndef TFTPSERVER_SENDFILE_H
#define TFTPSERVER_SENDFILE_H

#define MAX_FILE_NAME 20

#include "packets.h"

int CreateSocket(int port);

int sendFile(int socket, struct sockaddr *clntAddr, char filename[MAX_FILE_NAME], pthread_mutex_t *mutex);

int sendError(int socket, struct sockaddr *clntAddr, u_int16_t errorCode, char *error_message);

#endif //TFTPSERVER_SENDFILE_H
