TEMPLATE = app
TARGET = quill-autoclean
DEPENDPATH += .
INCLUDEPATH += .
LIBS += -lquill
QMAKE_LIBDIR += ../src/

CONFIG += debug

# Input
HEADERS += autoclean.h
SOURCES += autoclean.cpp main.cpp

target.path = /usr/bin/quill-utils
INSTALLS += target