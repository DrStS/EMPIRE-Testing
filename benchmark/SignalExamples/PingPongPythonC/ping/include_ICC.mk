CC  = mpiicc 
CXX = mpiicpc #If static EMPIRE_API is used mpifort has to be used for linking
FC  = ifort
LINKER = $(CXX)

CFLAGS    = -g -O3
CXXFLAGS  = $(CFLAGS)
FCFLAGS   = 
LFLAGS    = 
DEFINES   =
INCLUDES = -I$(EMPIRE_API_INC_ON_MACHINE)
LIBS      = $(EMPIRE_API_LIBSO_ON_MACHINE)


 


