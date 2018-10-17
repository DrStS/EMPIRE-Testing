// This is the client
// Using Intel MPI 4.0.3
// Starting the server with (mpdboot running) 
//   mpiexec -np 1 ./server

#include "EMPIRE_API.h"
#include <assert.h>
#include <stdio.h>

int main(int argc, char **argv) {
    EMPIRE_API_Connect("ping.xml");

    double toReceive = -1;
    printf("Receiving ... \n");
    EMPIRE_API_recvSignal_double("signal2", 1, &toReceive);
    printf("Received: %f \n", toReceive);

    double toSend = 9999;
    printf("Sending ... \n");
    EMPIRE_API_sendSignal_double("signal1", 1, &toSend);
    printf("Sent: %f \n", toSend);

    EMPIRE_API_Disconnect();

    return (0);
}

