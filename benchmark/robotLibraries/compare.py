import os
import math

def compare_floats(refFile,compFile,eps):
    with open(refFile) as refF:
         refValues = map(float, refF)
    refF.close()
    with open(compFile) as compF:
        compValues = map(float, compF)
    compF.close()
    for i in range(len(refValues)):
        absValue = abs(refValues[i]-compValues[i])
        print absValue
        if (absValue >= float(eps)) :
          raise Exception('ERROR deviation too high')