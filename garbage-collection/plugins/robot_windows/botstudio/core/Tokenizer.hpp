/*
 * Description:  Class defining a simple tokenizer
 */

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <QtCore/QString>
#include <QtCore/QChar>

class Tokenizer
{
public:
  Tokenizer(const QString &string, const QChar &separator=';');
  virtual ~Tokenizer() {}

  QString nextToken();
  bool hasMoreToken() const;
  QString remainingString() const;

private:
  QString mString;
  QChar mSeparator;
  int mCurrentPos;
};

#endif
