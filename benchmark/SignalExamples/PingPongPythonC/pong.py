import os
from ctypes import *
import time


libempire_api = cdll.LoadLibrary(os.environ['EMPIRE_API_LIBSO_ON_MACHINE'])


libempire_api.EMPIRE_API_Connect("pong.xml");

signal1 = (c_double * 1)(-1);
size1 = c_int(1);
print 'Receving ...' 
libempire_api.EMPIRE_API_recvSignal_double("signal1", size1, signal1);
print 'Received: {}'.format(signal1[0]);



signal2 = (c_double * 1)(10000);
size2 = c_int(1);
print 'Sending ...'
libempire_api.EMPIRE_API_sendSignal_double("signal2", size2, signal2);
print 'Sent: {}'.format(signal2[0]);


libempire_api.EMPIRE_API_Disconnect();


