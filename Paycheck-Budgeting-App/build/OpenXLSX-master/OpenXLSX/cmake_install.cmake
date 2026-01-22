# Install script for directory: /home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OpenXLSX/headers" TYPE FILE FILES "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/build/OpenXLSX-master/OpenXLSX/OpenXLSX-Exports.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OpenXLSX/headers" TYPE FILE FILES
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/IZipArchive.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLCell.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLCellIterator.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLCellRange.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLCellReference.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLCellValue.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLColor.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLColumn.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLCommandQuery.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLComments.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLConstants.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLContentTypes.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLDateTime.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLDocument.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLDrawing.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLException.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLFormula.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLIterator.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLMergeCells.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLProperties.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLRelationships.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLRow.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLRowData.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLSharedStrings.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLSheet.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLStyles.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLTables.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLWorkbook.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLXmlData.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLXmlFile.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLXmlParser.hpp"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/headers/XLZipArchive.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OpenXLSX" TYPE FILE FILES "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/OpenXLSX.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "lib" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/build/output/libOpenXLSX.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX" TYPE FILE FILES
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/OpenXLSX-master/OpenXLSX/OpenXLSXConfig.cmake"
    "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/build/OpenXLSX-master/OpenXLSX/OpenXLSX/OpenXLSXConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX/OpenXLSXTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX/OpenXLSXTargets.cmake"
         "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/build/OpenXLSX-master/OpenXLSX/CMakeFiles/Export/c72cc94553a1a0c9b05f75dae42fb1d7/OpenXLSXTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX/OpenXLSXTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX/OpenXLSXTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX" TYPE FILE FILES "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/build/OpenXLSX-master/OpenXLSX/CMakeFiles/Export/c72cc94553a1a0c9b05f75dae42fb1d7/OpenXLSXTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/OpenXLSX" TYPE FILE FILES "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/build/OpenXLSX-master/OpenXLSX/CMakeFiles/Export/c72cc94553a1a0c9b05f75dae42fb1d7/OpenXLSXTargets-noconfig.cmake")
  endif()
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/daniil-nahliuk/Desktop/Projects/GitHubProjects/Projects/Paycheck-Budgeting-App/build/OpenXLSX-master/OpenXLSX/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
