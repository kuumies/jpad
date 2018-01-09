#-------------------------------------------------
#
# Project created by QtCreator 2018-01-02T17:05:03
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = J-pad
TEMPLATE = app

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

macx:ICON = $${PWD}/resource/icons/jpad.png.icns

SOURCES += \
        main.cpp \
    jmdict/jmdict_parser.cpp \
    jmdict/jmdict.cpp \
    ui/text_editor.cpp \
    ui/text_editor_key_converter.cpp \
    ui/main_window.cpp \
    ui/text_editor_side_area.cpp \
    ui/text_editor_reading_to_kanji_area.cpp \
    ui/dictionary_dialog.cpp \
    ui/preferences_dialog.cpp \
    ui/about_dialog.cpp \
    settings.cpp

HEADERS += \
    jmdict/jmdict_parser.h \
    jmdict/jmdict.h \
    ui/text_editor.h \
    ui/text_editor_key_converter.h \
    ui/main_window.h \
    ui/text_editor_side_area.h \
    ui/text_editor_reading_to_kanji_area.h \
    ui/dictionary_dialog.h \
    ui/preferences_dialog.h \
    ui/about_dialog.h \
    settings.h

FORMS += \
    ui/main_window.ui \
    ui/dictionary_dialog.ui \
    ui/preferences_dialog.ui \
    ui/about_dialog.ui

RESOURCES += \
    resource/jpad.qrc
