CONFIG             *= c++17 qt debug_and_release

TEMPLATE            = app
TARGET              = QRenderOrang
QT                 += core gui widgets opengl

HEADERS            += include/Common.h             \
                      include/MainWidget.h         \
                      include/PreviewWidget.h      \
                      include/Mesh.h               \
                      include/SyntaxHighlighter.h

SOURCES            += src/MainWidget.cpp         \
                      src/PreviewWidget.cpp      \
                      src/Mesh.cpp               \
                      src/SyntaxHighlighter.cpp  \
                      src/main.cpp

FORMS              += ui/qrenderorang.ui

RESOURCES           = ui/qrenderorang.qrc


OBJECTS_DIR         = build
UI_DIR              = build/ui
MOC_DIR             = build/moc
RCC_DIR             = build/rc
DESTDIR             = bin

win32-msvc*:LIBS   += -lOpenGL32
win32-g++:LIBS     += -lopengl32
!win32:LIBS        += -lGL

INCLUDEPATH        += $$PWD/ext
INCLUDEPATH        += $$PWD/include
