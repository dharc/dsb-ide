cmake_minimum_required (VERSION 2.8)
project (dsbide)
FIND_PACKAGE(Qt4 REQUIRED)

# Need to include staged files and libs
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(/srv/stage/include)
link_directories(/srv/stage/lib)

set(dsbide_VERSION_MAJOR "0")
set(dsbide_VERSION_MINOR "1")
set(dsbide_VERSION_PATCH "1")

#option(DSBLOC "DSB Location" OFF)

#IF( DSBLOC )
#	include_directories(${DSBLOC}/common/include)
#	set(CMAKE_LD_FLAGS "${CMAKE_LD_FLAGS} -L${DSBLOC}/
#ENDIF ( DSBLOC )

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-D_DEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 ")

set(DSBIDESRC source/main.cpp source/eventgen.cpp)
set(DSBIDEHEADERS source/eventgen.h)

# configure_file( ${CMAKE_SOURCE_DIR}/common/include/dsb/config.h.in ${CMAKE_BINARY_DIR}/config.h )

QT4_WRAP_CPP(DSBIDEHEADERSMOC ${DSBIDEHEADERS})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

add_executable(dsb-ide ${DSBIDESRC} ${DSBIDEHEADERSMOC})
target_link_libraries(dsb-ide dsbcommon ${QT_LIBRARIES})

