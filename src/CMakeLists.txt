set(SOURCES 
        buoy_detector.cpp
        gate_detector.cpp
)

set(HEADERS
        buoy_detector.h
        gate_detector.h
)

rock_library(visual_detectors
    SOURCES ${SOURCES}
    DEPS_PKGCONFIG opencv
    DEPS_PKGCONFIG avalon_base-types
    HEADERS ${HEADERS}
)

#rock_executable(visual_detectors_bin main.cpp
#    DEPS visual_detectors)

