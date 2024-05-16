if (TARGET libocr)
    return()
endif()

set(_libocr_IncludeDir "${third_party_dir}/libocr")
set(_libocr_BinaryDir "${third_party_dir}/libocr")

set(_libocr_Inlcudes
    ${_libocr_IncludeDir}/libocr.h
)

add_library(libocr UNKNOWN IMPORTED)
set_target_properties(libocr PROPERTIES
    IMPORTED_IMPLIB_DEBUG           "${_libocr_BinaryDir}/libocr.dll"
    IMPORTED_IMPLIB_RELEASE         "${_libocr_BinaryDir}/libocr.dll"
    IMPORTED_LOCATION               "${_libocr_BinaryDir}/libocr.lib"
    IMPORTED_LOCATION_DEBUG         "${_libocr_BinaryDir}/libocr.lib"
    IMPORTED_LOCATION_RELEASE       "${_libocr_BinaryDir}/libocr.lib"
    INTERFACE_INCLUDE_DIRECTORIES   "${_libocr_IncludeDir}"
    INTERFACE_LINK_LIBRARIES        "$<$<CONFIG:Debug>:libocr>"
)
