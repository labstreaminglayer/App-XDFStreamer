#-------------------------------------------------
#
# Project created by QtCreator 2018-07-28T10:15:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XdfStreamer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        xdfstreamer.cpp \
    libxdf/pugixml/pugixml.cpp \
    libxdf/xdf.cpp

HEADERS += \
        xdfstreamer.h \
    libxdf/pugixml/pugiconfig.hpp \
    libxdf/pugixml/pugixml.hpp \
    libxdf/xdf.h \
    extern/include/lsl_c.h \
    extern/include/lsl_cpp.h

FORMS += \
        xdfstreamer.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/extern/bin/ -lliblsl32
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/extern/bin/ -lliblsl32-debug
else:unix: LIBS += -L$$PWD/extern/bin/ -lliblsl32

INCLUDEPATH += $$PWD/extern/include
DEPENDPATH += $$PWD/extern/include
