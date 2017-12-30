/*
 * Description:  Implementation of the Tokenizer.hpp functions
 */

#include "Tokenizer.hpp"

Tokenizer::Tokenizer(const QString &string, const QChar &separator):
    mString(string),
    mSeparator(separator),
    mCurrentPos(0)
{
}

QString Tokenizer::nextToken() {
  QString token;
  QChar c;
  int size = mString.size();
  do {
    if (mCurrentPos == size)
      break;
    c = mString[mCurrentPos];
    mCurrentPos++;
    if (c!=';')
      token.append(c);
  } while (c!=';');

  return token;
}

bool Tokenizer::hasMoreToken() const {
  return mCurrentPos < mString.size();
}

QString Tokenizer::remainingString() const {
  return mString.mid(mCurrentPos);
}
