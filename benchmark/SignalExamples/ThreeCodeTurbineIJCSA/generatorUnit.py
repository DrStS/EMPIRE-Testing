import os
from ctypes import cdll
from ctypes import *
import time
print "This is the generatorUnit!" 
############################################################################
# EMPIRE_API & connect
libempire_api = cdll.LoadLibrary(os.environ['EMPIRE_API_LIBSO_ON_MACHINE'])
libempire_api.EMPIRE_API_Connect("generatorUnit.xml")
############################################################################
# This is a linear mass-spring system integrated with BE 
# A Dirichlet Neumann coupling of spring vs spring example
# Please see: "Interface-Jacobian based Co-Simulation" Sicklinger et al.
#             International Journal for Numerical Methods in Engineering
# Problem parameters
# Mass
m=0.45
# Stiffness
k=0.5
# Time information
timeSteps=10
deltaT=0.1
############################################################################
interfaceForceU        = (c_double * 3)(1,1,1)
interfaceDisplacementY = (c_double * 3)(1,1,1)
size                   = c_int(3)
isConvergent  = c_int(0)
# Initial condition v0=1 u0=0
u_n_m_1=[-1*deltaT]
u_n    =[0]
u_n_p_1=[0]
# Iteration counter
iter=1
############################################################################
def doSolve(input,m,k,deltaT,u_n_p_1,u_n,u_n_m_1):
    fac=1/(m+deltaT*deltaT*k)
    u_n_p_1[0]=deltaT*deltaT*fac*input+m*fac*(2*u_n-u_n_m_1)
    return u_n_p_1[0]
############################################################################
def goToNextTimeStep(u_n_p_1,u_n,u_n_m_1):
    u_n_m_1[0]=u_n[0]
    u_n[0]=u_n_p_1[0]
    return
############################################################################
for i in range(1,timeSteps+1):
      print '################## Current time step: {}'.format(i)
      while 1:
         print '--------------Iteration: {}'.format(iter)
         print 'Receiving ...' 
         libempire_api.EMPIRE_API_recvSignal_double("forcesRotorMoment", size, interfaceForceU)
         print 'Receive forcesRotorMoment: {}'.format(interfaceForceU[1])

         interfaceDisplacementY[1] = doSolve(interfaceForceU[1],m,k,deltaT,u_n_p_1,u_n[0],u_n_m_1[0])

         print 'Sending ...'
         libempire_api.EMPIRE_API_sendSignal_double("cellZoneRotorAllRadialVelocity", size, interfaceDisplacementY)
         print 'Sent interfaceDisplacementY: {}'.format(interfaceDisplacementY[1])
         #io_history.append(interfaceDisplacementY[2])
         
         isConvergent=libempire_api.EMPIRE_API_recvConvergenceSignal()
         print 'isConvergent: {}'.format(isConvergent)
         if isConvergent!=0:
          break
         iter=iter+1       
      goToNextTimeStep(u_n_p_1,u_n,u_n_m_1);
      iter=1;
      print 'u_n_p_1: %f' % u_n_p_1[0]
      print 'u_n: %f' % u_n[0]
      print 'u_n_m_1: %f' % u_n_m_1[0]

############################################################################
# Generate data for compare
# EMPIRE disconnect
libempire_api.EMPIRE_API_Disconnect()
############################################################################
exit();
