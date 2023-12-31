#ifndef COMM_CONTROLLER_H
#define COMM_CONTROLLER_H

#include "configuration.h"
#include "vehicle.h"s


typedef struct {
    byte RxData[SIZE_OF_RX_DATA];
    byte TxData[SIZE_OF_TX_DATA];
    int comm_baud_rate = 115200;
} CommController;


enum struct {
    POSITION,
    VELOCITY
} DesiredControl ;



// Function prototypes
void receiveData(CommController *comm, Vehicle *Vehicle);
void sendData(CommController *comm, Vehicle *Vehicle);
void comm_controller_init(CommController *comm);

#endif // COMM_CONTROLLER_H
