# Create variable containing all config names.
file(GLOB ConfigFiles ${CMAKE_SOURCE_DIR}/configs/*.json)

#message("${CMAKE_SOURCE_DIR}/configs configs: ${ConfigFiles}")
#message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}/bin/")

# Create custom target for copying the configs
add_custom_target(configs)
foreach(ConfigFile ${ConfigFiles})
  add_custom_command(TARGET configs
                     COMMAND ${CMAKE_COMMAND} -E copy ${ConfigFile} ${CMAKE_INSTALL_PREFIX}/bin/
                     COMMENT "Copying ${ConfigFile} to ${CMAKE_INSTALL_PREFIX}/bin"
                     )
endforeach()
