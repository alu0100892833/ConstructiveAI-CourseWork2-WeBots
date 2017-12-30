#include "BotStudioPaths.hpp"

#include <core/StandardPaths.hpp>

#include <QtCore/QDir>

using namespace webotsQtUtils;

const QString &BotStudioPaths::getBotStudioPath() {
  static QString path(StandardPaths::getWebotsHomePath() + "projects/robots/e-puck/plugins/robot_windows/botstudio/");
  return path;
};

const QString &BotStudioPaths::getIconsPath() {
  static QString path(getBotStudioPath() + "icons/");
  return path;
};
