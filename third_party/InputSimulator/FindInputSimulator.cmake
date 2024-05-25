if (TARGET InputSimulator)
    return()
endif()

set(_InputSimulator_IncludeDir "${third_party_dir}/InputSimulator")
set(_InputSimulator_BinaryDir "${third_party_dir}/InputSimulator")

set(_InputSimulator_Inlcudes
    ${_InputSimulator_IncludeDir}/InputSimulator.hpp
)

add_library(InputSimulator UNKNOWN IMPORTED)
set_target_properties(InputSimulator PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES   "${_InputSimulator_IncludeDir}"
)
