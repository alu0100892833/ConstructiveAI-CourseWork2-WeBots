# Webots Makefile system 
#
# You may add some variable definitions hereafter to customize the build process
# See documentation in $(WEBOTS_HOME_PATH)/resources/Makefile.include


CXX_SOURCES = \
  AccelerometerGroupBox.cpp \
  CameraGroupBox.cpp \
  EPuckRepresentation.cpp \
  EncoderGroupBox.cpp \
  GroundSensorGroupBox.cpp \
  Gui.cpp \
  MainWidget.cpp \
  RemoteControlWidget.cpp \
  entry_points.cpp

HPP_FILES_TO_MOC = \
  RemoteControlWidget.hpp

QT = core gui widgets concurrent
USE_C_API = true

QT_UTILS = $(WEBOTS_HOME)/resources/projects/libraries/qt_utils
INCLUDE = -I"$(QT_UTILS)"
LIBRARIES = -L"$(QT_UTILS)" -lqt_utils

space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.include
