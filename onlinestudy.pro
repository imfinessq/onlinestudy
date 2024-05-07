QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qaesencryption.cpp \
    threadonline.cpp \
    threadrunclient.cpp

HEADERS += \
    aesni/aesni-enc-cbc.h \
    aesni/aesni-enc-ecb.h \
    aesni/aesni-key-exp.h \
    aesni/aesni-key-init.h \
    mainwindow.h \
    qaesencryption.h \
    threadonline.h \
    threadrunclient.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

msvc{
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

INCLUDEPATH += C:\Users\SSQ\AppData\Local\GitHubDesktop\app-3.3.13\resources\app\git\cmd\vcpkg\packages\tesseract_x64-windows\include


INCLUDEPATH += C:\Users\SSQ\AppData\Local\GitHubDesktop\app-3.3.13\resources\app\git\cmd\vcpkg\packages\leptonica_x64-windows\include


CONFIG( debug, debug|release ) {
    # debug
    LIBS+= C:\Users\SSQ\AppData\Local\GitHubDesktop\app-3.3.13\resources\app\git\cmd\vcpkg\packages\tesseract_x64-windows\debug\lib\tesseract53d.lib
    LIBS+= C:\Users\SSQ\AppData\Local\GitHubDesktop\app-3.3.13\resources\app\git\cmd\vcpkg\packages\leptonica_x64-windows\debug\lib\leptonica-1.84.1d.lib

} else {
    # release
    LIBS+= C:\Users\SSQ\AppData\Local\GitHubDesktop\app-3.3.13\resources\app\git\cmd\vcpkg\packages\tesseract_x64-windows\lib\tesseract53.lib
    LIBS+= C:\Users\SSQ\AppData\Local\GitHubDesktop\app-3.3.13\resources\app\git\cmd\vcpkg\packages\leptonica_x64-windows\lib\leptonica-1.84.1.lib
}

CONFIG(debug, debug|release) {
        QMAKE_CXXFLAGS_DEBUG += /MTd
    }

CONFIG(release, debug|release) {
        QMAKE_CXXFLAGS_RELEASE += /MT
    }
QMAKE_LFLAGS_DEBUG      = /DEBUG /NODEFAULTLIB:msvcrtd.lib
QMAKE_LFLAGS_RELEASE      = /RELEASE /NODEFAULTLIB:msvcrt.lib
