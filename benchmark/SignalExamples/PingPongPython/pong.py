import os
from ctypes import *
import time


libempire_api = cdll.LoadLibrary(os.environ['EMPIRE_API_LIBSO_ON_MACHINE'])
print "This is the pong!" 

libempire_api.EMPIRE_API_Connect("pong.xml");

signal1 = (c_double * 5)(1,2,3,4,5);
size1 = c_int(5);
libempire_api.EMPIRE_API_recvSignal_double("signal1", size1, signal1);

signal2 = (c_double * 10)(1,2,3,4,5,6,7,8,9,10);
size2 = c_int(10);
libempire_api.EMPIRE_API_sendSignal_double("signal2", size2, signal2);

libempire_api.EMPIRE_API_Disconnect();


