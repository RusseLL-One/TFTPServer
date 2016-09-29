#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "network.h"

#define PORT 69

pthread_mutex_t mutex;

void *ClientHandler(void *thrStr);

int main(int argc, char *argv[]) {
    threadStr *thrStr;
    int sock;
    socklen_t addrStrLen;
    pthread_t threadID;
    sock = CreateSocket(PORT);
    pthread_mutex_init(&mutex, NULL);

    for (; ;) {
        thrStr = malloc(sizeof(threadStr));
        addrStrLen = sizeof(thrStr->addrStr);

        if ((recvfrom(sock, thrStr->buffer, BUFSIZE, 0, &thrStr->addrStr, &addrStrLen)) < 0) {
            perror("recvfrom()");
            continue;
        }
        pthread_create(&threadID, NULL, ClientHandler, thrStr);
    }
}

void *ClientHandler(void *thrStr) {
    threadStr *threadData = (threadStr *) thrStr;
    int socket;
    tftpPacket *packet;

    sleep(1);
    packet = DeserializePacket(threadData->buffer);
    socket = CreateSocket(0);

    if (packet->optcode == TFTP_OPTCODE_RRQ) {
        sendFile(socket, &threadData->addrStr, packet->read_request.filename, &mutex);
    } else {
        sendError(socket, &threadData->addrStr, TFTP_ERRCODE_ILLEGAL_OPERATION, "You can't put files on this server");
        printf("Client wants to put\n");
    }

    free(threadData);
    free(packet);
    return (void *) 0;
}