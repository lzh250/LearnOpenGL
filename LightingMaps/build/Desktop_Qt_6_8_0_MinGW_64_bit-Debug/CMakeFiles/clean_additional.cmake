# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\LightingMaps_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\LightingMaps_autogen.dir\\ParseCache.txt"
  "LightingMaps_autogen"
  )
endif()
