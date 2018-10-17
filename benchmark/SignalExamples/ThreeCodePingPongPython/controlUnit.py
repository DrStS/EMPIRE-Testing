import os
from ctypes import *
import time


libempire_api = cdll.LoadLibrary(os.environ['EMPIRE_API_LIBSO_ON_MACHINE'])
print "This is the controlUnit!" 

libempire_api.EMPIRE_API_Connect("controlUnit.xml");

timeSteps=10
for i in range(1,timeSteps+1):
    cellZoneRotorAllRadialVelocity = (c_double * 3)(0,0,0);
    cellZoneRotorAllRadialVelocitySize = c_int(3);
    libempire_api.EMPIRE_API_recvSignal_double("cellZoneRotorAllRadialVelocity", cellZoneRotorAllRadialVelocitySize, cellZoneRotorAllRadialVelocity);
    print 'Receive cellZoneRotorAllRadialVelocity: {}'.format(cellZoneRotorAllRadialVelocity[1])
    cellZoneBlade3RadialVelocity = (c_double * 3)(-90,0,0);
    cellZoneBlade3RadialVelocitySize = c_int(3);
    libempire_api.EMPIRE_API_sendSignal_double("cellZoneBlade3RadialVelocity", cellZoneBlade3RadialVelocitySize, cellZoneBlade3RadialVelocity);

libempire_api.EMPIRE_API_Disconnect();


