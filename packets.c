//
// Created by root on 27.09.16.
//

#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include "packets.h"

tftpPacket *DeserializePacket(char *buffer) {
    u_int16_t errCode;
    tftpPacket *packet;
    char *dataOffset = buffer;

    packet = malloc(sizeof(tftpPacket));

    memcpy(&packet->optcode, dataOffset, sizeof(u_int16_t));
    packet->optcode = ntohs(packet->optcode);
    printf("opcode: %d", packet->optcode);

    dataOffset += sizeof(u_int16_t);
    switch (packet->optcode) {
        case TFTP_OPTCODE_RRQ:
            strncpy(packet->read_request.filename, dataOffset, MAX_STRING_SIZE + 1);
            dataOffset += strlen(packet->read_request.filename) + 1;
            strncpy(packet->read_request.mode, dataOffset, MAX_MODE_SIZE + 1);
            printf(", filename: %s, mode: %s\n", packet->read_request.filename, packet->read_request.mode);
            break;
        case TFTP_OPTCODE_WRQ:
            strncpy(packet->write_request.filename, dataOffset, MAX_STRING_SIZE + 1);
            dataOffset += strlen(packet->write_request.filename) + 1;
            strncpy(packet->write_request.mode, dataOffset, MAX_MODE_SIZE + 1);
            printf(", filename: %s, mode: %s\n", packet->write_request.filename, packet->write_request.mode);
            break;
        case TFTP_OPTCODE_DATA:
            memcpy(&packet->data.blockNumber, dataOffset, sizeof(u_int16_t));
            packet->data.blockNumber = ntohs(packet->data.blockNumber);
            dataOffset += sizeof(u_int16_t);
            strncpy(packet->data.data, dataOffset, MAX_DATA_SIZE + 1);
            packet->data.dataSize = strlen(packet->data.data);
            printf(", block №: %d, data: %s\n", packet->data.blockNumber, packet->data.data);
            break;
        case TFTP_OPTCODE_ACK:
            memcpy(&packet->ack.blockNumber, dataOffset, sizeof(u_int16_t));
            packet->ack.blockNumber = ntohs(packet->ack.blockNumber);
            printf(", block №: %d\n", packet->ack.blockNumber);
            break;
        case TFTP_OPTCODE_ERR:
            memcpy(&errCode, dataOffset, sizeof(u_int16_t));
            packet->error.errorCode = ntohs(packet->error.errorCode);
            dataOffset += sizeof(u_int16_t);
            strncpy(packet->error.message, dataOffset, MAX_STRING_SIZE + 1);
            printf("\nError! errCode: %d, errMsg: %s\n", packet->error.errorCode, packet->error.message);
            break;
        default:
            break;
    }
    return packet;
}

int SerializePacket(tftpPacket packet, char *buffer) {
    int size = 0;
    char *dataOffset = buffer;

    switch (packet.optcode) {
        case TFTP_OPTCODE_RRQ:
            packet.optcode = htons(packet.optcode);
            memcpy(dataOffset, &packet.optcode, sizeof(u_int16_t));
            dataOffset += sizeof(u_int16_t);
            size += sizeof(u_int16_t);

            strncpy(dataOffset, packet.read_request.filename, strlen(packet.read_request.filename) + 1);
            size += strlen(packet.read_request.filename) + 1;

            strncpy(dataOffset, packet.read_request.mode, strlen(packet.read_request.mode) + 1);
            size += strlen(packet.read_request.mode) + 1;
            break;
        case TFTP_OPTCODE_WRQ:
            packet.optcode = htons(packet.optcode);
            memcpy(dataOffset, &packet.optcode, sizeof(u_int16_t));
            dataOffset += sizeof(u_int16_t);
            size += sizeof(u_int16_t);

            strncpy(dataOffset, packet.write_request.filename, strlen(packet.write_request.filename) + 1);
            size += strlen(packet.write_request.filename) + 1;

            strncpy(dataOffset, packet.write_request.mode, strlen(packet.write_request.mode) + 1);
            size += strlen(packet.write_request.mode) + 1;
            break;
        case TFTP_OPTCODE_DATA:
            packet.optcode = htons(packet.optcode);
            memcpy(dataOffset, &packet.optcode, sizeof(u_int16_t));
            dataOffset += sizeof(u_int16_t);
            size += sizeof(u_int16_t);

            packet.data.blockNumber = htons(packet.data.blockNumber);
            memcpy(dataOffset, &packet.data.blockNumber, sizeof(u_int16_t));
            dataOffset += sizeof(u_int16_t);
            size += sizeof(u_int16_t);

            strncpy(dataOffset, packet.data.data, packet.data.dataSize);
            size += packet.data.dataSize;
            break;
        case TFTP_OPTCODE_ACK:
            packet.optcode = htons(packet.optcode);
            memcpy(dataOffset, &packet.optcode, sizeof(u_int16_t));
            dataOffset += sizeof(u_int16_t);
            size += sizeof(u_int16_t);

            packet.ack.blockNumber = htons(packet.ack.blockNumber);
            memcpy(dataOffset, &packet.ack.blockNumber, sizeof(u_int16_t));
            size += sizeof(u_int16_t);
            break;
        case TFTP_OPTCODE_ERR:
            packet.optcode = htons(packet.optcode);
            memcpy(dataOffset, &packet.optcode, sizeof(u_int16_t));
            dataOffset += sizeof(u_int16_t);
            size += sizeof(u_int16_t);

            packet.error.errorCode = htons(packet.error.errorCode);
            memcpy(dataOffset, &packet.error.errorCode, sizeof(u_int16_t));
            dataOffset += sizeof(u_int16_t);
            size += sizeof(u_int16_t);

            strncpy(dataOffset, packet.error.message, strlen(packet.error.message) + 1);
            size += strlen(packet.error.message) + 1;
            break;
        default:
            break;
    }
    return size;
}