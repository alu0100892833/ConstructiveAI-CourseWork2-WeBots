# Webots Makefile system 
#
# You may add some variable definitions hereafter to customize the build process
# See documentation in $(WEBOTS_HOME_PATH)/resources/Makefile.include


CXX_SOURCES = \
  core/Automaton.cpp \
  core/AutomatonObject.cpp \
  core/AutomatonObjectRepresentation.cpp \
  core/AutomatonScene.cpp \
  core/AutomatonWidget.cpp \
  core/BotStudioWindow.cpp \
  core/BotStudioPaths.cpp \
  core/CommonProperties.cpp \
  core/Model.cpp \
  core/RemoteControlWidget.cpp \
  core/RobotConditionWidget.cpp \
  core/RobotObjectFactory.cpp \
  core/RobotStateWidget.cpp \
  core/RobotViewWidget.cpp \
  core/State.cpp \
  core/StateRepresentation.cpp \
  core/SwitchWidget.cpp \
  core/Tokenizer.cpp \
  core/Transition.cpp \
  core/TransitionRepresentation.cpp \
  core/entry_points.cpp \
  e-puck/EPuckActuatorCommand.cpp \
  e-puck/EPuckConditionWidget.cpp \
  e-puck/EPuckDrawingHelper.cpp \
  e-puck/EPuckFacade.cpp \
  e-puck/EPuckLedButton.cpp \
  e-puck/EPuckObjectFactory.cpp \
  e-puck/EPuckSensorCondition.cpp \
  e-puck/EPuckSlider.cpp \
  e-puck/EPuckStateWidget.cpp \
  e-puck/EPuckViewWidget.cpp

HPP_FILES_TO_MOC = \
  core/Automaton.hpp \
  core/AutomatonObject.hpp \
  core/AutomatonObjectRepresentation.hpp \
  core/AutomatonScene.hpp \
  core/AutomatonWidget.hpp \
  core/BotStudioWindow.hpp \
  core/Model.hpp \
  core/RemoteControlWidget.hpp \
  core/RobotFacade.hpp \
  core/State.hpp \
  core/StateRepresentation.hpp \
  core/SwitchWidget.hpp \
  core/TransitionRepresentation.hpp \
  e-puck/EPuckConditionWidget.hpp \
  e-puck/EPuckLedButton.hpp \
  e-puck/EPuckSlider.hpp \
  e-puck/EPuckStateWidget.hpp

QT = core gui widgets
USE_C_API = true

QT_UTILS = $(WEBOTS_HOME)/resources/projects/libraries/qt_utils
INCLUDE = -I"$(QT_UTILS)"
LIBRARIES = -L"$(QT_UTILS)" -lqt_utils

space :=
space +=
WEBOTS_HOME_PATH=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.include
