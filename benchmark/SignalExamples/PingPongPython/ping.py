import os
from ctypes import cdll
from ctypes import *
import time

# This tests basic communication and runTimeModifiableUserDefinedText userDefinedText

libempire_api = cdll.LoadLibrary(os.environ['EMPIRE_API_LIBSO_ON_MACHINE'])
EMPIRE_API_getUserDefinedText=libempire_api.EMPIRE_API_getUserDefinedText
EMPIRE_API_getUserDefinedText.restype = c_char_p

print "This is the ping!" 

libempire_api.EMPIRE_API_Connect("ping.xml")


print EMPIRE_API_getUserDefinedText('myMessage')
time.sleep(10)# modify the ping.xml file now

signal2 = (c_double * 10)(1,2,3,4,5,6,7,8,9,10)
size2 = c_int(10)
libempire_api.EMPIRE_API_recvSignal_double("signal2", size2, signal2)

signal1 = (c_double * 5)(1,2,3,4,5)
size1 = c_int(5)
libempire_api.EMPIRE_API_sendSignal_double("signal1", size1, signal1)


print EMPIRE_API_getUserDefinedText('myMessage')
libempire_api.EMPIRE_API_Disconnect()
