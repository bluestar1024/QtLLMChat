QT       += core gui 3dcore

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
    chatrecordswidget.cpp \
    codeeditor.cpp \
    codeshow.cpp \
    copybutton.cpp \
    customlabel.cpp \
    custommenu.cpp \
    customwebengineview.cpp \
    cxxhighlighter.cpp \
    doublespinbox.cpp \
    frame.cpp \
    funwidget.cpp \
    glslhighlighter.cpp \
    html_renderer.cpp \
    imagelabel.cpp \
    itemwidget.cpp \
    label.cpp \
    language.cpp \
    lineedit.cpp \
    linenumberarea.cpp \
    listwidget.cpp \
    loadingwidget.cpp \
    luahighlighter.cpp \
    main.cpp \
    mainwindow.cpp \
    markdown_block_element.cpp \
    markdown_inline_element.cpp \
    markdown_parser.cpp \
    messagethread.cpp \
    messagewidget.cpp \
    printlabel.cpp \
    pushbutton.cpp \
    pythonhighlighter.cpp \
    sendbutton.cpp \
    settingedit.cpp \
    settingwidget.cpp \
    slider.cpp \
    spinbox.cpp \
    splitter.cpp \
    stylesyntaxhighlighter.cpp \
    syntaxstyle.cpp \
    textboxwidget.cpp \
    textedit.cpp \
    texteditfull.cpp \
    textshow.cpp \
    textwidget.cpp \
    thinkbackwidget.cpp \
    thinkingbutton.cpp \
    thinkwidget.cpp \
    titlewidget.cpp \
    webenginepage.cpp \
    webengineview.cpp \
    widget.cpp

HEADERS += \
    chatrecordswidget.h \
    codeeditor.h \
    codeshow.h \
    copybutton.h \
    customlabel.h \
    custommenu.h \
    customwebengineview.h \
    cxxhighlighter.h \
    doublespinbox.h \
    frame.h \
    funwidget.h \
    globalvariables.h \
    glslhighlighter.h \
    html_renderer.h \
    imagelabel.h \
    itemwidget.h \
    label.h \
    language.h \
    lineedit.h \
    linenumberarea.h \
    listwidget.h \
    loadingwidget.h \
    luahighlighter.h \
    mainwindow.h \
    markdown_block_element.h \
    markdown_inline_element.h \
    markdown_parser.h \
    messagethread.h \
    messagewidget.h \
    printlabel.h \
    pushbutton.h \
    pythonhighlighter.h \
    sendbutton.h \
    settingedit.h \
    settingwidget.h \
    slider.h \
    spinbox.h \
    splitter.h \
    stylesyntaxhighlighter.h \
    syntaxstyle.h \
    textboxwidget.h \
    textedit.h \
    texteditfull.h \
    textshow.h \
    textwidget.h \
    thinkbackwidget.h \
    thinkingbutton.h \
    thinkwidget.h \
    titlewidget.h \
    webenginepage.h \
    webengineview.h \
    widget.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
