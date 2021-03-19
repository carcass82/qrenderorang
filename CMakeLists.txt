cmake_minimum_required(VERSION 3.16)

project(QRenderOrang LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${PROJECT_SOURCE_DIR}/ui/")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)

include_directories(./include ./ext)

set(INCLUDES "include/Common.h"
             "include/MainWidget.h"
             "include/PreviewWidget.h"
             "include/UniformWidget.h"
             "include/Mesh.h"
             "include/Texture.h"
             "include/ShaderEditor.h"
             "include/SyntaxHighlighter.h")
			 
set(SOURCES "src/MainWidget.cpp"
            "src/PreviewWidget.cpp"
            "src/UniformWidget.cpp"
            "src/Mesh.cpp"
            "src/Texture.cpp"
            "src/ShaderEditor.cpp"
            "src/SyntaxHighlighter.cpp"
            "src/main.cpp")

set(FORMS "ui/qrenderorang.ui"
          "ui/uniform.ui")

set(RESOURCES "ui/qrenderorang.qrc")

qt6_add_executable(QRenderOrang ${SOURCES} ${INCLUDES} ${FORMS} ${RESOURCES})
target_link_libraries(QRenderOrang PRIVATE Qt6::Widgets Qt6::OpenGLWidgets)
