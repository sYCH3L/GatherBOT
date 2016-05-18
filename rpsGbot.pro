#-------------------------------------------------
#
# Project created by QtCreator 2012-06-26T14:32:27
#
#-------------------------------------------------

QT       += core network sql

QT       -= gui

TARGET = rpsGbot
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    handler.cpp \
    commands.cpp \
    statshandler.cpp \
    csserver.cpp

HEADERS += \
    handler.h \
    commands.h \
    statshandler.h \
    csserver.h
