QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webenginewidgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    customlabel.cpp \
    customwebengineview.cpp \
    frame.cpp \
    funwidget.cpp \
    html_renderer.cpp \
    imagelabel.cpp \
    itemwidget.cpp \
    listwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    markdown_block_element.cpp \
    markdown_inline_element.cpp \
    markdown_parser.cpp \
    messagethread.cpp \
    messagewidget.cpp \
    pushbutton.cpp \
    sendbutton.cpp \
    splitter.cpp \
    textedit.cpp \
    texteditfull.cpp \
    textshow.cpp \
    webenginepage.cpp \
    webengineview.cpp \
    widget.cpp

HEADERS += \
    customlabel.h \
    custommenu.h \
    customwebengineview.h \
    frame.h \
    funwidget.h \
    html_renderer.h \
    imagelabel.h \
    itemwidget.h \
    listwidget.h \
    mainwindow.h \
    markdown_block_element.h \
    markdown_inline_element.h \
    markdown_parser.h \
    messagethread.h \
    messagewidget.h \
    pushbutton.h \
    sendbutton.h \
    splitter.h \
    textedit.h \
    texteditfull.h \
    textshow.h \
    webenginepage.h \
    webengineview.h \
    widget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
