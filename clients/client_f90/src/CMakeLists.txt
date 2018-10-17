IF(BUILD_FORTRAN_CLIENTS)
#------------------------------------------------------------------------------------#
set(SOURCES "client_f90.f90" "m_empire_api.f90")
#------------------------------------------------------------------------------------#
#include_directories($ENV{EMPIRE_API_INC_ON_MACHINE})
#------------------------------------------------------------------------------------#
add_executable(client_f90 ${SOURCES})
target_link_libraries(client_f90 $ENV{EMPIRE_API_LIBSO_ON_MACHINE})
#------------------------------------------------------------------------------------#
add_dependencies(client_f90 EMPIRE_API_Static EMPIRE_API_Shared Emperor)
#------------------------------------------------------------------------------------#
ENDIF(BUILD_FORTRAN_CLIENTS)
