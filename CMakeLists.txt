cmake_minimum_required (VERSION 2.8)
project (dsbide)
FIND_PACKAGE(Qt4 REQUIRED)

# Need to include staged files and libs
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(/srv/stage/include)
include_directories(../dsb/common/include)
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

set(DSBIDESRC source/main.cpp source/connectdiag.cpp source/errlog.cpp source/msglog.cpp source/msggen.cpp views/Assembler/assembler.cpp views/Assembler/view.cpp views/Assembler/asmsyntax.cpp source/treeview.cpp source/ide.cpp source/view.cpp views/ObjectView/view.cpp views/DisplayView/view.cpp)
set(DSBIDEHEADERS source/connectdiag.h source/msglog.h source/errlog.h source/msggen.h views/Assembler/assembler.h source/treeview.h include/dsb/ide/ide.h include/dsb/ide/view.h views/ObjectView/objectview.h views/DisplayView/displayview.h)
set(DSBIDERCCS data/icons.qrc data/splash.qrc)

# configure_file( ${CMAKE_SOURCE_DIR}/common/include/dsb/config.h.in ${CMAKE_BINARY_DIR}/config.h )

QT4_WRAP_CPP(DSBIDEHEADERSMOC ${DSBIDEHEADERS})
QT4_ADD_RESOURCES(DSBIDERCCSRCS ${DSBIDERCCS})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

add_executable(dsb-ide ${DSBIDESRC} ${DSBIDEHEADERSMOC} ${DSBIDERCCSRCS})
target_link_libraries(dsb-ide dsbcommon ${QT_LIBRARIES})

