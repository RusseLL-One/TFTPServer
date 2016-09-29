//
// Created by root on 26.09.16.
//
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "packets.h"
#include "network.h"


int CreateSocket(int port) {
    int sock;
    int flag;
    struct sockaddr_in addrStr;

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        fprintf(stderr, "socket() failed\n");
        return -1;
    }

    flag = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *) &flag, sizeof(flag)) < 0) {
        fprintf(stderr, "setsockopt() failed\n");
        return -1;
    }
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *) &flag, sizeof(flag)) < 0) {
        fprintf(stderr, "setsockopt() failed\n");
        return -1;
    }

    memset(&addrStr, 0, sizeof(addrStr));
    addrStr.sin_family = AF_INET;
    addrStr.sin_addr.s_addr = htonl(INADDR_ANY);
    addrStr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *) &addrStr, sizeof(addrStr)) < 0) {
        perror("bind()");
        return -1;
    }

    return sock;
}

int sendFile(int socket, struct sockaddr *clntAddr, char filename[MAX_FILE_NAME], pthread_mutex_t *mutex) {
    FILE *file;
    char *buffer;
    struct sockaddr recvAddr;
    socklen_t len = sizeof recvAddr;
    int bufferSize;
    char data[MAX_DATA_SIZE];
    int remainingData;
    tftpPacket packet;

    if ((file = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "File doesn't exist\n");
        return sendError(socket, clntAddr, TFTP_ERRCODE_FILE_NOT_FOUND, "File not found");
    }
    packet.optcode = TFTP_OPTCODE_DATA;
    packet.data.blockNumber = 1;

    pthread_mutex_lock(mutex);
    do {
        buffer = malloc(BUFSIZE);
        remainingData = fread(data, 1, MAX_DATA_SIZE, file);
        strncpy(packet.data.data, data, MAX_DATA_SIZE);
        packet.data.dataSize = remainingData;
        bufferSize = SerializePacket(packet, buffer);

        if (sendto(socket, buffer, bufferSize, 0, clntAddr, sizeof(struct sockaddr)) != bufferSize) {
            perror("sendto(): ");
        }
        if (recvfrom(socket, buffer, BUFSIZE, 0, &recvAddr, &len) < 0) {
            perror("recvfrom()");
        }
        printf("Bytes sent: %d, Block Number: %d\n", bufferSize, packet.data.blockNumber);
        packet.data.blockNumber++;
        free(buffer);
    } while (remainingData == MAX_DATA_SIZE);
    printf("File \"%s\" sent!\n", filename);
    pthread_mutex_unlock(mutex);
    return 0;
}

int sendError(int socket, struct sockaddr *clntAddr, u_int16_t errorCode, char *error_message) {
    char *buffer = malloc(BUFSIZE);
    int bufferSize;
    int result;
    tftpPacket packet;

    packet.optcode = TFTP_OPTCODE_ERR;
    packet.error.errorCode = errorCode;
    strcpy(packet.error.message, error_message);

    bufferSize = SerializePacket(packet, buffer);

    if ((result = sendto(socket, buffer, bufferSize, 0, clntAddr, sizeof(struct sockaddr))) != bufferSize) {
        perror("sendto(): ");
    }
    free(buffer);
    return result;
}