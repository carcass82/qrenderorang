CONFIG             *= c++17 qt debug_and_release

TEMPLATE            = app
TARGET              = QRenderOrang
QT                 += core gui widgets opengl

HEADERS            += src/MainWidget.h         \
                      src/PreviewWidget.h      \
                      src/Shader.h             \
                      src/Mesh.h               \
					  src/Material.h


SOURCES            += src/MainWidget.cpp       \
                      src/PreviewWidget.cpp    \
                      src/Shader.cpp           \
                      src/Mesh.cpp             \
					  src/Material.cpp         \
                      src/main.cpp

FORMS              += src/qrenderorang.ui
RESOURCES           = src/qrenderorang.qrc

OBJECTS_DIR         = build
UI_DIR              = build/ui
MOC_DIR             = build/moc
RCC_DIR             = build/rc
DESTDIR             = bin

win32-msvc*:LIBS   += -lOpenGL32
win32-g++:LIBS     += -lopengl32
!win32:LIBS        += -lGL

#
# add "ext" libs includepath
#
INCLUDEPATH        += ext

#
# GLEW - statically linked
#
SOURCES            += ext/glew/glew.c
INCLUDEPATH        += ext/glew/include
DEFINES            += GLEW_STATIC
