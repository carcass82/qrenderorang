CONFIG      *= qt debug

TEMPLATE     = app
TARGET       = QRenderOrang
QT          += core gui widgets opengl

HEADERS     += src/MainWidget.h         \
               src/PreviewWidget.h      \
               src/Shader.h             \
               src/Mesh.h
               

SOURCES     += src/MainWidget.cpp       \
               src/PreviewWidget.cpp    \
               src/Shader.cpp           \
               src/Mesh.cpp             \
               src/main.cpp
             
FORMS       += src/qrenderorang.ui
RESOURCES    = src/qrenderorang.qrc

OBJECTS_DIR  = build
UI_DIR       = build/ui
MOC_DIR      = build/moc
RCC_DIR      = build/rc
DESTDIR      = bin

CONFIG      += c++11

!win32:LIBS        += -lGLEW
win32:!contains(QMAKE_HOST.arch, x86_64):LIBS += $$PWD/../SharedLibs/GLEW/lib/Release/Win32/glew32.lib
win32:contains(QMAKE_HOST.arch, x86_64):LIBS  += $$PWD/../SharedLibs/GLEW/lib/Release/x64/glew32.lib
win32:INCLUDEPATH += $$PWD/../SharedLibs/GLEW/include
win32:INCLUDEPATH += $$PWD/../SharedLibs/
