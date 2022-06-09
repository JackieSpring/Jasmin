#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "capstone::capstone" for configuration ""
set_property(TARGET capstone::capstone APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(capstone::capstone PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "C"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libcapstone.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS capstone::capstone )
list(APPEND _IMPORT_CHECK_FILES_FOR_capstone::capstone "${_IMPORT_PREFIX}/lib/libcapstone.a" )

# Import target "capstone::cstool" for configuration ""
set_property(TARGET capstone::cstool APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(capstone::cstool PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/bin/cstool"
  )

list(APPEND _IMPORT_CHECK_TARGETS capstone::cstool )
list(APPEND _IMPORT_CHECK_FILES_FOR_capstone::cstool "${_IMPORT_PREFIX}/bin/cstool" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
