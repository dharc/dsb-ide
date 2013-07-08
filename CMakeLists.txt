cmake_minimum_required (VERSION 2.8)
project (dsbide)
FIND_PACKAGE(Qt5Widgets REQUIRED)

# Need to include staged files and libs
include_directories(${PROJECT_SOURCE_DIR}/source)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_BINARY_DIR})
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

# BISON and FLEX compile
ADD_CUSTOM_TARGET(DSBParser)

ADD_CUSTOM_COMMAND(
	SOURCE ${CMAKE_SOURCE_DIR}/views/ScriptView/lexer.l
	COMMAND lex
	ARGS -o${CMAKE_BINARY_DIR}/lexer.c ${CMAKE_SOURCE_DIR}/views/ScriptView/lexer.l
	TARGET DSBParser
	OUTPUTS ${CMAKE_BINARY_DIR}/lexer.c)
	
ADD_CUSTOM_COMMAND(
	SOURCE ${CMAKE_SOURCE_DIR}/views/ScriptView/parser.y
	COMMAND yacc
	ARGS -d -o ${CMAKE_BINARY_DIR}/parser.cpp ${CMAKE_SOURCE_DIR}/views/ScriptView/parser.y
	TARGET DSBParser
	DEPENDS ${CMAKE_BINARY_DIR}/lexer.c
	OUTPUTS ${CMAKE_BINARY_DIR}/parser.cpp)
	
	

set(DSBIDESRC source/main.cpp source/connectdiag.cpp source/errlog.cpp source/msglog.cpp source/msggen.cpp views/ScriptView/scriptsyntax.cpp views/ScriptView/script.cpp ${CMAKE_BINARY_DIR}/parser.cpp views/Assembler/assembler.cpp views/Assembler/view.cpp views/Assembler/asmsyntax.cpp source/treeview.cpp source/ide.cpp source/view.cpp views/ObjectView/view.cpp views/DisplayView/view.cpp)
set(DSBIDEHEADERS source/connectdiag.h source/msglog.h source/errlog.h source/msggen.h views/ScriptView/script.h views/Assembler/assembler.h source/treeview.h include/dsb/ide/ide.h include/dsb/ide/view.h views/ObjectView/objectview.h views/DisplayView/displayview.h)
set(DSBIDERCCS data/icons.qrc data/splash.qrc)

SET_SOURCE_FILES_PROPERTIES(${CMAKE_BINARY_DIR}/parser.cpp GENERATED)

# configure_file( ${CMAKE_SOURCE_DIR}/common/include/dsb/config.h.in ${CMAKE_BINARY_DIR}/config.h )

QT5_WRAP_CPP(DSBIDEHEADERSMOC ${DSBIDEHEADERS})
QT5_ADD_RESOURCES(DSBIDERCCSRCS ${DSBIDERCCS})

INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})

add_executable(dsb-ide ${DSBIDESRC} ${DSBIDEHEADERSMOC} ${DSBIDERCCSRCS})
target_link_libraries(dsb-ide dsbcommon ${QT_LIBRARIES})

qt5_use_modules(dsb-ide Widgets)

