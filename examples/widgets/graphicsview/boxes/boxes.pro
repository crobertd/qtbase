QT += opengl widgets

HEADERS += 3rdparty/fbm.h \
           glbuffers.h \
           glextensions.h \
           gltrianglemesh.h \
           qtbox.h \
           roundedbox.h \
           scene.h \
           trackball.h
SOURCES += 3rdparty/fbm.c \
           glbuffers.cpp \
           glextensions.cpp \
           main.cpp \
           qtbox.cpp \
           roundedbox.cpp \
           scene.cpp \
           trackball.cpp

RESOURCES += boxes.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/graphicsview/boxes
INSTALLS += target

wince*: {
    DEPLOYMENT_PLUGIN += qjpeg
}

win32-msvc* {
    QMAKE_CXXFLAGS -= -Zm200
    QMAKE_CFLAGS -= -Zm200
    QMAKE_CXXFLAGS += -Zm500
    QMAKE_CFLAGS += -Zm500
}
