QT       += core gui
QT       += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ch341.cpp \
    main.cpp \
    mainwindow.cpp \
    mpu6050.cpp

HEADERS += \
    CH341DLL.H \
    ch341.h \
    mainwindow.h \
    mpu6050.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./ -lCH341DLLA64
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./ -lCH341DLLA64d

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/./libCH341DLLA64.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/./libCH341DLLA64d.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/./CH341DLLA64.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/./CH341DLLA64d.lib

RESOURCES += \
    res.qrc
