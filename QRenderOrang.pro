!equals(QT_MAJOR_VERSION, 6) {
    error("Need Qt6, found Qt$${QT_VERSION}")
}

CONFIG             *= c++17 qt debug_and_release

TEMPLATE            = app

TARGET              = QRenderOrang

QT                 += core gui widgets opengl openglwidgets

HEADERS            += include/Common.h             \
                      include/MainWidget.h         \
                      include/PreviewWidget.h      \
                      include/UniformWidget.h      \
                      include/Mesh.h               \
                      include/Texture.h            \
                      include/ShaderEditor.h       \
                      include/SyntaxHighlighter.h

SOURCES            += src/MainWidget.cpp         \
                      src/PreviewWidget.cpp      \
                      src/UniformWidget.cpp      \
                      src/Mesh.cpp               \
                      src/Texture.cpp            \
                      src/ShaderEditor.cpp       \
                      src/SyntaxHighlighter.cpp  \
                      src/main.cpp

FORMS              += ui/qrenderorang.ui \
                      ui/uniform.ui

RESOURCES           = ui/qrenderorang.qrc

OBJECTS_DIR         = build
UI_DIR              = build/ui
MOC_DIR             = build/moc
RCC_DIR             = build/rc
DESTDIR             = bin

INCLUDEPATH        += $$PWD/ext
INCLUDEPATH        += $$PWD/include
