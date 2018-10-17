#include "EMPIRE_API.h"
#include <assert.h>
#include "GiDFileIO.h"
#include "FluxCreator.h"
#include "FieldCreator.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace std;
bool doIterativeCoupling(string text) {
    if (text == "iterativeCoupling")
        return true;
    else if (text == "looseCoupling")
        return false;
    else
        assert(false);
    return false;
}

int getNumTimeSteps(string text) {
    stringstream ss(text);
    int numTimeSteps;
    ss >> numTimeSteps;
    return numTimeSteps;
}

int getDirection(string text) {
    if (text == "x")
        return 0;
    else if (text == "y")
        return 1;
    else if (text == "z")
        return 2;
    else
        assert(false);
    return 10;
}

double getFloatPointNummer(string text) {
    stringstream ss(text);
    double d;
    ss >> d;
    return d;
}

class MechanicalSystem { // M*ddq + K*q = p(t)
public:
    MechanicalSystem(double _M, double _C, double _K, double _h, double _gamma, double _beta) :
            M(_M), C(_C), K(_K), h(_h), gamma(_gamma), beta(_beta) {
    }
    virtual ~MechanicalSystem() {
    }
    void solveNewTimestep(double p) {
        double ddq0 = ddq;
        double dq0 = dq;
        double q0 = q;
        ddq = 1 / (M + gamma * h * C + beta * h * h * K)
                * (p - C * (dq0 + (1 - gamma) * h * ddq0)
                        - K * (q0 + h * dq0 + (0.5 - beta) * h * h * ddq0));
        dq = dq0 + (1 - gamma) * h * ddq0 + gamma * h * ddq;
        q = q0 + h * dq0 + h * h * (0.5 - beta) * ddq0 + h * h * beta * ddq;
    }
    void set_q(double _q, double _dq, double _ddq) {
        q = _q;
        dq = _dq;
        ddq = _ddq;
    }
    void get_q(double &_q, double &_dq, double &_ddq) {
        _q = q;
        _dq = dq;
        _ddq = ddq;
    }

private:
    const double M, C, K; // mass, damping and stiffness
    const double h; // time step length
    const double gamma, beta; // Newmark-Beta coefficient
    double q, dq, ddq; // displacement, velocity and acceleration
};

/*
 * A dummy CSM solver doing loose coupling with OpenFoam. The number of time steps is 10.
 * It can generate the TUM logo which is punched into OpenFoam.
 */
int main(int argc, char **argv) {
    // connect
    EMPIRE_API_Connect("mechanicalClient.xml");

    // send mesh
    string meshfile = EMPIRE_API_getUserDefinedText("GiDMeshFile");
    int numNodes = -1;
    int numElems = -1;
    double *nodeCoors;
    int *nodeIDs;
    int *numNodesPerElem;
    int *elemTable;
    int *elemIDs;
    GiDFileIO::readDotMsh(meshfile, numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem,
            elemTable, elemIDs);
    EMPIRE_API_sendMesh("", numNodes, numElems, nodeCoors, nodeIDs, numNodesPerElem, elemTable);

    // set up fsi
    int numTimeSteps = getNumTimeSteps(EMPIRE_API_getUserDefinedText("numTimeSteps"));
    bool todoIterativeCoupling = doIterativeCoupling(EMPIRE_API_getUserDefinedText("couplingType"));

    // set up the mechanical system
    double M, C, K, h, gamma, beta;
    M = getFloatPointNummer(EMPIRE_API_getUserDefinedText("mass"));
    C = getFloatPointNummer(EMPIRE_API_getUserDefinedText("damping"));
    K = getFloatPointNummer(EMPIRE_API_getUserDefinedText("stiffness"));
    h = getFloatPointNummer(EMPIRE_API_getUserDefinedText("timeStepLength"));
    gamma = getFloatPointNummer(EMPIRE_API_getUserDefinedText("gamma"));
    beta = getFloatPointNummer(EMPIRE_API_getUserDefinedText("beta"));
    MechanicalSystem mechanicalSystem(M, C, K, h, gamma, beta);

    int direction = getDirection(EMPIRE_API_getUserDefinedText("direction"));

    double *dofsRecv = new double[numNodes * 3];
    double *dofsSend = new double[numNodes * 3];

    // optimization loop
    int currentNumOfOptInterations = 0;
    while (true) {
        currentNumOfOptInterations++;
        cout << "==============================================================================="
                << endl;
        cout << "===============  optimization step: " << currentNumOfOptInterations
                << "  ==============================" << endl;
        cout << "==============================================================================="
                << endl;

        double time = 0.0;
        cout << "time" << "\t" << "q" << endl;
        // time loop
        for (int i = 1; i <= numTimeSteps; i++) {
            time += h;
            if (i == 1)
                mechanicalSystem.set_q(0.0, 0.0, 0.0);
            double q0, dq0, ddq0;
            mechanicalSystem.get_q(q0, dq0, ddq0);
            do {
                EMPIRE_API_recvDataField("", numNodes * 3, dofsRecv);
                double liftDrag[3] = { 0.0, 0.0, 0.0 };
                for (int j = 0; j < numNodes; j++) {
                    liftDrag[0] += dofsRecv[j * 3 + 0];
                    liftDrag[1] += dofsRecv[j * 3 + 1];
                    liftDrag[2] += dofsRecv[j * 3 + 2];
                }

                mechanicalSystem.set_q(q0, dq0, ddq0); // have to be reset
                mechanicalSystem.solveNewTimestep(liftDrag[direction]);

                for (int j = 0; j < numNodes * 3; j++) {
                    dofsSend[j] = 0.0;
                }

                double q, dq, ddq;
                mechanicalSystem.get_q(q, dq, ddq);
                cout << time << "\t" << q << endl;
                for (int j = 0; j < numNodes; j++)
                    dofsSend[j * 3 + direction] = q;

                EMPIRE_API_sendDataField("", numNodes * 3, dofsSend);
                //EMPIRE_API_printDataField("-----send-----", numNodes * 3, dofsSend);

                //EMPIRE_API_printDataField("-----receive-----", numNodes * 3, dofsRecv);
                if (!todoIterativeCoupling)
                    break;
            } while (EMPIRE_API_recvConvergenceSignal() == 0);
        }
        if (EMPIRE_API_recvConvergenceSignal() == 1) {
            break;
        }
    }
    delete[] nodeCoors;
    delete[] nodeIDs;
    delete[] numNodesPerElem;
    delete[] elemTable;
    delete[] elemIDs;
    delete[] dofsRecv;
    delete[] dofsSend;

    EMPIRE_API_Disconnect();

    // used for testing Newmark-Beta
    /*double numTimeSteps;
     double M, K, h, gamma, beta, p;
     M = 1.0;
     K = 6.283 * 6.283;
     h = 0.1;
     numTimeSteps = 10;
     gamma = 0.5;
     beta = 0.25;

     MechanicalSystem mechanicalSystem(M, K, h, gamma, beta);

     double time = 0.0;
     for (int i = 1; i <= numTimeSteps; i++) {
     time += h;
     if (i == 1)
     mechanicalSystem.set_q(1.0, 0.0, 0.0);
     double q0, dq0, ddq0;
     mechanicalSystem.get_q(q0, dq0, ddq0);
     for (int j=0; j<3; j++) {
     mechanicalSystem.set_q(q0, dq0, ddq0); // have to be reset
     mechanicalSystem.solveNewTimestep(0.0);
     double q, dq, ddq;
     mechanicalSystem.get_q(q, dq, ddq);
     cout << time << " " << q << endl;
     }
     }*/

    return (0);
}

