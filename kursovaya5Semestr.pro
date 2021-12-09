QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
	main.cpp \
	mainwindow.cpp \
	tableModel.cpp

HEADERS += \
	BacktrackingThread.h \
	SequentialWalkThread.h \
	mainwindow.h \
	nearestPeakThread.h \
	tableModel.h

FORMS += \
	mainwindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
