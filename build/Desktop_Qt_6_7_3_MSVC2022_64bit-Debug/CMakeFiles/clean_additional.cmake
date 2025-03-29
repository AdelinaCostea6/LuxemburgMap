# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\DijkstraAlg_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\DijkstraAlg_autogen.dir\\ParseCache.txt"
  "DijkstraAlg_autogen"
  )
endif()
