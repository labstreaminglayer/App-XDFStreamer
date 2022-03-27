cmake_minimum_required(VERSION 3.10)

project(XDFStreamer
	LANGUAGES CXX
	VERSION 1.14.0
	DESCRIPTION "Application that streams signals from XDF files to lab streaming layer."
	HOMEPAGE_URL https://github.com/labstreaminglayer/App-XDFStreamer
)

find_package(LSL REQUIRED
	HINTS ${LSL_INSTALL_ROOT}
	PATH_SUFFIXES share/LSL
)

find_package(libxdf REQUIRED
	HINTS ${XDF_INSTALL_ROOT}
	PATH_SUFFIXES share
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 REQUIRED COMPONENTS Widgets)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE #WIN32
	XdfStreamer/main.cpp
	XdfStreamer/xdfstreamer.h
	XdfStreamer/xdfstreamer.cpp
	XdfStreamer/xdfstreamer.ui
)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	Qt5::Widgets
	Threads::Threads
	LSL::lsl
	XDF::xdf
)

installLSLApp(${PROJECT_NAME})
installLSLAuxFiles(${PROJECT_NAME}
	LICENSE README.md  # ${PROJECT_NAME}.cfg
)


set(CPACK_PACKAGE_CONTACT "yl3842@columbia.edu")
if(APPLE)
	set(CPACK_GENERATOR TBZ2)
elseif(WIN32)
	set(CPACK_GENERATOR ZIP)
elseif(UNIX)
	set(CPACK_GENERATOR DEB)
	set(CPACK_DEBIAN_LIBLSL_PACKAGE_SHLIBDEPS ON)
	set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
endif()
include(CPack)

if (WIN32)
	get_target_property(QT_QMAKE_EXECUTABLE Qt::qmake IMPORTED_LOCATION)
	get_filename_component(QT_WINDEPLOYQT_EXECUTABLE ${QT_QMAKE_EXECUTABLE} PATH)
	set(QT_WINDEPLOYQT_EXECUTABLE "${QT_WINDEPLOYQT_EXECUTABLE}/windeployqt.exe")

	add_custom_command(
			TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${QT_WINDEPLOYQT_EXECUTABLE}
			--no-translations --no-system-d3d-compiler
			--qmldir ${CMAKE_CURRENT_SOURCE_DIR}
			$<TARGET_FILE_DIR:${PROJECT_NAME}>)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:LSL::lsl>
			# $<TARGET_FILE:XDF::xdf>  # not needed as it is statically linked.
			$<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()
