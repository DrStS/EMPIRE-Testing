import os
from ctypes import cdll
from ctypes import *
import time

# This tests basic communication and runTimeModifiableUserDefinedText userDefinedText

libempire_api = cdll.LoadLibrary(os.environ['EMPIRE_API_LIBSO_ON_MACHINE'])
EMPIRE_API_getUserDefinedText=libempire_api.EMPIRE_API_getUserDefinedText
EMPIRE_API_getUserDefinedText.restype = c_char_p

print "This is the generatorUnit!" 

libempire_api.EMPIRE_API_Connect("generatorUnit.xml")

timeSteps=10
for i in range(1,timeSteps+1):
    forcesRotorMoment = (c_double * 3)(0,0,0);
    forcesRotorMomentSize = c_int(3);
    libempire_api.EMPIRE_API_recvSignal_double("forcesRotorMoment", forcesRotorMomentSize, forcesRotorMoment);
    print 'Receive forcesRotorMoment: {}'.format(forcesRotorMoment[1])
    cellZoneRotorAllRadialVelocity = (c_double * 3)(0,432,0);
    cellZoneRotorAllRadialVelocitySize = c_int(3);
    libempire_api.EMPIRE_API_sendSignal_double("cellZoneRotorAllRadialVelocity", cellZoneRotorAllRadialVelocitySize, cellZoneRotorAllRadialVelocity);

libempire_api.EMPIRE_API_Disconnect();
