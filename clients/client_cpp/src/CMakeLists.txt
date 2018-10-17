#------------------------------------------------------------------------------------#
IF (${USE_INTEL_COMPILERS_AND_MKL})
	SET (CMAKE_C_COMPILER "icc")
	SET (CMAKE_CXX_COMPILER "icpc")
ENDIF()
#------------------------------------------------------------------------------------#
file(GLOB SOURCES "*.cpp" "*.c")
add_executable(client_cpp ${SOURCES})
target_link_libraries(client_cpp $ENV{EMPIRE_API_LIBSO_ON_MACHINE})
#------------------------------------------------------------------------------------#
include_directories($ENV{EMPIRE_API_INC_ON_MACHINE})
#------------------------------------------------------------------------------------#
add_dependencies(client_cpp EMPIRE_API_Static EMPIRE_API_Shared Emperor)