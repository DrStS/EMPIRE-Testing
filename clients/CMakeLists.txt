#------------------------------------------------------------------------------------#
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#------------------------------------------------------------------------------------#
FILE(GLOB TestClients_src_subdirectories "*")
FOREACH(f ${TestClients_src_subdirectories})
  IF(NOT f MATCHES "\\.txt" AND IS_DIRECTORY ${f} )
    add_subdirectory(${f})
  ENDIF()
ENDFOREACH()
#------------------------------------------------------------------------------------#
#------------------------------------------------------------------------------------#
