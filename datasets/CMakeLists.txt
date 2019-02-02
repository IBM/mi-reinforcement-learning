# Create variable containing all dataset file names.
file(GLOB DatasetFiles ${CMAKE_SOURCE_DIR}/datasets/mazes/*.csv)

#message("${CMAKE_SOURCE_DIR}/dataset/mazes: ${DatasetFiles}")
#message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}/dataset/mazes/")

# Create custom target for copying the daatsets
add_custom_target(datasets)
add_custom_command(TARGET datasets COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/dataset/mazes/)
foreach(file ${DatasetFiles})
  add_custom_command(TARGET datasets
                     COMMAND ${CMAKE_COMMAND} -E copy ${file} ${CMAKE_INSTALL_PREFIX}/dataset/mazes/
                     COMMENT "Copying ${file} to ${CMAKE_INSTALL_PREFIX}/dataset/mazes"
                     )
endforeach()

