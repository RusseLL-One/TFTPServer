//
// Created by russell on 26.09.16.
//

#ifndef TFTPSERVER_PACKETS_H
#define TFTPSERVER_PACKETS_H

#define MAX_STRING_SIZE 1024
#define MAX_MODE_SIZE 8
#define MAX_DATA_SIZE 512

#define BUFSIZE 2048

//optcodes
#define TFTP_OPTCODE_RRQ  1
#define TFTP_OPTCODE_WRQ  2
#define TFTP_OPTCODE_DATA 3
#define TFTP_OPTCODE_ACK  4
#define TFTP_OPTCODE_ERR  5

//Error Codes
#define TFTP_ERRCODE_FILE_NOT_FOUND      1
#define TFTP_ERRCODE_ILLEGAL_OPERATION   4

#include <sys/types.h>

typedef struct {
    char filename[MAX_STRING_SIZE];
    char mode[MAX_MODE_SIZE];
} RRQ, WRQ;

typedef struct {
    u_int16_t blockNumber;
    unsigned int dataSize;
    char data[MAX_DATA_SIZE];
} DATA;

typedef struct {
    u_int16_t blockNumber;
} ACK;

typedef struct {
    u_int16_t errorCode;
    char message[MAX_STRING_SIZE];
} ERROR;

typedef struct {
    u_int16_t optcode;
    union {
        RRQ read_request;
        WRQ write_request;
        DATA data;
        ACK ack;
        ERROR error;
    };
} tftpPacket;

typedef struct {
    char buffer[BUFSIZE];
    struct sockaddr addrStr;
} threadStr;

tftpPacket *DeserializePacket(char *buffer);

int SerializePacket(tftpPacket packet, char *buffer);

#endif //TFTPSERVER_PACKETS_H
