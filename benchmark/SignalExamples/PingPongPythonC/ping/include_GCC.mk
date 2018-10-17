CC  = mpicc  
CXX = mpicxx #If static EMPIRE_API is used mpifort has to be used for linking
FC  = gfortran
LINKER = $(CXX)

CFLAGS    = -g -O3 -std=c99
CXXFLAGS  = -g -O3
FCFLAGS   = 
LFLAGS    =
DEFINES   =
INCLUDES = -I$(EMPIRE_API_INC_ON_MACHINE)
LIBS      = $(EMPIRE_API_LIBSO_ON_MACHINE)

 


