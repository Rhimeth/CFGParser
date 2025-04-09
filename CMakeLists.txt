cmake_minimum_required(VERSION 3.10)
project(CFGParser)

# Disallow in-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please create a separate build directory.")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt auto-generation settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/src/gui)

# LLVM/Clang configuration
set(LLVM_DIR "/usr/lib/llvm-18/lib/cmake/llvm")
set(Clang_DIR "/usr/lib/llvm-18/lib/cmake/clang")
find_package(LLVM 18.1.3 REQUIRED CONFIG)
find_package(Clang 18.1.3 REQUIRED CONFIG)

llvm_map_components_to_libnames(LLVM_LIBS support core)

execute_process(
    COMMAND llvm-config-18 --includedir
    OUTPUT_VARIABLE LLVM_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(CLANG_INCLUDE_DIR "${LLVM_INCLUDE_DIR}/clang")

# Find Qt5 packages
find_package(Qt5 COMPONENTS 
    Core Gui Widgets PrintSupport Concurrent Svg OpenGL
    REQUIRED)

# Check for Graphviz
find_program(DOT_EXECUTABLE dot)
if(NOT DOT_EXECUTABLE)
    message(WARNING "Graphviz 'dot' not found - visualization features will be limited")
endif()

# Source files (explicit listing to avoid picking up DOT files)
set(SOURCES
    src/gui/mainwindow.cpp
    src/gui/customgraphview.cpp
    src/cfg_generation_action.cpp
    src/cfg_graph.cpp
    src/cfg_analyzer.cpp
    src/graph_generator.cpp
    src/parser.cpp
    src/visualizer.cpp
    src/ast_extractor.cpp
    src/main.cpp
)

# Header files
set(HEADERS
    include/analysis_results.h
    include/ast_extractor.h
    include/customgraphview.h
    include/cfg_analyzer.h
    include/graph_generator.h
    include/wsl_fallback.h
    include/parser.h
    include/visualizer.h
    include/mainwindow.h
)

# UI files
set(UI_FILES
    src/gui/mainwindow.ui
)

# Files that need moc processing
set(MOC_HEADERS
    include/customgraphview.h
    include/mainwindow.h
)

# Generate moc files and UI headers
qt5_wrap_cpp(MOC_FILES ${MOC_HEADERS})
qt5_wrap_ui(UI_HEADERS ${UI_FILES})

# Create executable
add_executable(CFGParser 
    ${SOURCES} 
    ${HEADERS}
    ${MOC_FILES}
    ${UI_HEADERS}
)

# Include directories
target_include_directories(CFGParser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LLVM_INCLUDE_DIR}
    ${CLANG_INCLUDE_DIR}
)

# Add definitions
target_compile_definitions(CFGParser PRIVATE
    ${LLVM_DEFINITIONS}
    ${CLANG_DEFINITIONS}
)

# Link libraries
target_link_libraries(CFGParser PRIVATE
    Qt5::Core 
    Qt5::Gui 
    Qt5::Widgets 
    Qt5::PrintSupport
    Qt5::Svg
    Qt5::Concurrent
    Qt5::OpenGL
    ${LLVM_LIBS}
    clangTooling
    clangFrontend
    clangDriver
    clangSerialization
    clangParse
    clangSema
    clangAnalysis
    clangEdit
    clangAST
    clangLex
    clangBasic
)

# Add custom command for DOT file processing if Graphviz is available
if(DOT_EXECUTABLE)
    file(GLOB DOT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/output/*.dot")
    foreach(dot_file ${DOT_FILES})
        get_filename_component(dot_name ${dot_file} NAME_WE)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${dot_name}.png
            COMMAND ${DOT_EXECUTABLE} -Tpng ${dot_file} -o ${CMAKE_CURRENT_BINARY_DIR}/${dot_name}.png
            DEPENDS ${dot_file}
            COMMENT "Generating visualization: ${dot_name}.png"
        )
        list(APPEND DOT_OUTPUTS ${CMAKE_CURRENT_BINARY_DIR}/${dot_name}.png)
    endforeach()

    if(DOT_OUTPUTS)
        add_custom_target(generate_visualizations ALL DEPENDS ${DOT_OUTPUTS})
    endif()
endif()

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Install target
install(TARGETS CFGParser 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)