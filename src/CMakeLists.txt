set(SOURCES 
        buoy_detector.cpp
        gate_detector.cpp
        buoy_estimation_filter.cpp
        buoy_pos_estimation.cpp
        buoy_paradise_filter.cpp
        command_creation.cpp
)

set(HEADERS
        buoy_interface.h
        buoy_detector.h
        gate_detector.h
        buoy_estimation_filter.h
        buoy_pos_estimation.h
        buoy_paradise_filter.h
        command_creation.h
        Types.hpp
)

rock_library(visual_detectors
    SOURCES ${SOURCES}
    DEPS_PKGCONFIG opencv base-types frame_helper
    HEADERS ${HEADERS}
)

rock_executable(visual_detectors_bin main.cpp
    DEPS visual_detectors
    DEPS_PKGCONFIG opencv base-types frame_helper
)

