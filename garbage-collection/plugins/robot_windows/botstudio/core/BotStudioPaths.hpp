/*
 * Description:  Class defining the standard paths
 */

#ifndef BOTSTUDIO_PATHS_HPP
#define BOTSTUDIO_PATHS_HPP

#include <QtCore/QString>

class BotStudioPaths
{
public:
  static const QString &getBotStudioPath();
  static const QString &getIconsPath();

private:
  BotStudioPaths() {}
  ~BotStudioPaths() {}
};

#endif
