import os
from ctypes import *
import time


libempire_api = cdll.LoadLibrary(os.environ['EMPIRE_API_LIBSO_ON_MACHINE'])
print "This is the OpenFOAM!" 

libempire_api.EMPIRE_API_Connect("dummyOF.xml");

timeSteps=10
for i in range(1,timeSteps+1):
    cellZoneBlade3RadialVelocity = (c_double * 3)(0,0,0);
    cellZoneBlade3RadialVelocitySize = c_int(3);
    libempire_api.EMPIRE_API_recvSignal_double("cellZoneBlade3RadialVelocity", cellZoneBlade3RadialVelocitySize, cellZoneBlade3RadialVelocity);
    print 'Receive cellZoneBlade3RadialVelocity: {}'.format(cellZoneBlade3RadialVelocity[0])
    cellZoneBlade1RadialVelocity = (c_double * 3)(0,0,0);
    cellZoneBlade1RadialVelocitySize = c_int(3);
    libempire_api.EMPIRE_API_recvSignal_double("cellZoneBlade1RadialVelocity", cellZoneBlade1RadialVelocitySize, cellZoneBlade1RadialVelocity);
    print 'Receive cellZoneBlade1RadialVelocity: {}'.format(cellZoneBlade1RadialVelocity[0])
    forcesRotorMoment     = (c_double * 10)(10,700,20);
    forcesRotorMomentSize = c_int(3);
    cellZoneRotorAllRadialVelocity = (c_double * 3)(0,0,0);
    cellZoneRotorAllRadialVelocitySize = c_int(3);
    libempire_api.EMPIRE_API_recvSignal_double("cellZoneRotorAllRadialVelocity", cellZoneRotorAllRadialVelocitySize, cellZoneRotorAllRadialVelocity);
    print 'Receive cellZoneRotorAllRadialVelocity: {}'.format(cellZoneRotorAllRadialVelocity[1])
    libempire_api.EMPIRE_API_sendSignal_double("forcesRotorMoment", forcesRotorMomentSize, forcesRotorMoment);

libempire_api.EMPIRE_API_Disconnect();


