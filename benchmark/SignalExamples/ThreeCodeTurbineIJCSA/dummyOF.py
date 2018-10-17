import os
from ctypes import *
import time
print "This is the OpenFOAM!" 
############################################################################
# EMPIRE_API & connect
libempire_api = cdll.LoadLibrary(os.environ['EMPIRE_API_LIBSO_ON_MACHINE'])
libempire_api.EMPIRE_API_Connect("dummyOF.xml");
############################################################################
# This is a linear mass-spring system integrated with BE 
# A Dirichlet Neumann coupling of spring vs spring example
# Please see: "Interface-Jacobian based Co-Simulation" Sicklinger et al.
#             International Journal for Numerical Methods in Engineering
# Problem parameters
# Mass
m=0.55
# Stiffness
k=0.5
timeSteps=10
deltaT=0.1
############################################################################

interfaceDisplacementU        = (c_double * 3)(0,0,0)
interfaceForceY = (c_double * 3)(0,0,0)
size            = c_int(3)
isConvergent  = c_int(1)
isConvergent  = 0
# Initial condition v0=1 u0=0
u_n_m_1=[-1*deltaT]
u_n = [0]
u_n_p_1=[0]
# Iteration counter
iter=1
############################################################################
def doSolve(input,m,k,deltaT,u_n_p_1,u_n,u_n_m_1):
    fac=1/(m+deltaT*deltaT*k)
    u_n_p_1[0] = input
    f_n_p_1 = -(u_n_p_1[0] * ((m/(deltaT*deltaT)) +k) - (m/(deltaT*deltaT))* (2 * u_n - u_n_m_1))
    return f_n_p_1
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
          
          cellZoneBlade3RadialVelocity = (c_double * 3)(0,0,0);
          cellZoneBlade3RadialVelocitySize = c_int(3);
          libempire_api.EMPIRE_API_recvSignal_double("cellZoneBlade3RadialVelocity", cellZoneBlade3RadialVelocitySize, cellZoneBlade3RadialVelocity);
          print 'Receive cellZoneBlade3RadialVelocity: {}'.format(cellZoneBlade3RadialVelocity[0])
          cellZoneBlade1RadialVelocity = (c_double * 3)(0,0,0);
          cellZoneBlade1RadialVelocitySize = c_int(3);
          libempire_api.EMPIRE_API_recvSignal_double("cellZoneBlade1RadialVelocity", cellZoneBlade1RadialVelocitySize, cellZoneBlade1RadialVelocity);
          print 'Receive cellZoneBlade1RadialVelocity: {}'.format(cellZoneBlade1RadialVelocity[0])

          
          print 'Receiving ...' 
          libempire_api.EMPIRE_API_recvSignal_double("cellZoneRotorAllRadialVelocity", size, interfaceDisplacementU)
          print 'Received cellZoneRotorAllRadialVelocity: {}'.format(interfaceDisplacementU[1])
          
          interfaceForceY[1] = doSolve(interfaceDisplacementU[1],m,k,deltaT,u_n_p_1,u_n[0],u_n_m_1[0])
          
          print 'Sending ...'
          libempire_api.EMPIRE_API_sendSignal_double("forcesRotorMoment", size, interfaceForceY)
          print 'Sent forcesRotorMoment: {}'.format(interfaceForceY[1])
          
          
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
############################################################################
# EMPIRE disconnect
libempire_api.EMPIRE_API_Disconnect()
exit();
