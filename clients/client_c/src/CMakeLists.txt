#------------------------------------------------------------------------------------#
file(GLOB SOURCES "*.cpp" "*.c")
#------------------------------------------------------------------------------------#
include_directories($ENV{EMPIRE_API_INC_ON_MACHINE})
#------------------------------------------------------------------------------------#
add_executable(client_c ${SOURCES})
target_link_libraries(client_c $ENV{EMPIRE_API_LIB_ON_MACHINE})
#------------------------------------------------------------------------------------#
add_dependencies(client_c EMPIRE_API_Static EMPIRE_API_Shared Emperor)
