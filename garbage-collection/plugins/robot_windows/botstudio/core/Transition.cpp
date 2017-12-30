/*
 * Description:  Implementation of the Transition.hpp functions
 */

#include "Automaton.hpp"
#include "Model.hpp"
#include "Transition.hpp"
#include "State.hpp"
#include "RobotObjectFactory.hpp"
#include "RobotSensorCondition.hpp"
#include "Tokenizer.hpp"

Transition::Transition(const QPointF &position, State *startState, State *endState) :
    AutomatonObject(position),
    mStartState(startState),
    mEndState(endState)
{
  mSensorCondition = RobotObjectFactory::instance()->createRobotSensorCondition();
  setName(tr("Transition %1").arg(name()));
}

Transition::~Transition() {
  delete mSensorCondition;
}

void Transition::fromString(const QString &string) {
  Tokenizer tokenizer(string);
  QStringList tokens;

  for (int i=0; i<7; i++) {
    if (tokenizer.hasMoreToken())
      tokens.append(tokenizer.nextToken());
    else
      throw tr("Corrupted Transition (wrong number of parameters)");
  }

  QString sChar = tokens[0];
  int id = tokens[1].toInt();
  QString name = tokens[2].replace("\\n", "\n");
  int posX = tokens[3].toDouble();
  int posY = tokens[4].toDouble();
  int startId = tokens[5].toInt();
  int endId = tokens[6].toInt();

  State *startState = Model::instance()->automaton()->findStateById(startId);
  State *endState = Model::instance()->automaton()->findStateById(endId);


  if (sChar.size() != 1 || sChar[0]!='T')
    throw tr("Corrupted Transition (first parameter isn't a T)");
  if (startState == NULL || endState == NULL)
    throw tr("Corrupted transition (bad reference to state)");

  setPosition(QPointF(posX, posY));
  setName(name);
  setStartState(startState);
  setEndState(endState);
  setUniqueId(id);
  computeCurrentGlobalId(id);

  sensorCondition()->fromString(tokenizer.remainingString());
}

QString Transition::toString() const {
  QString out;
  QString lName = name();
  lName.replace('\n', "\\n");
  lName.replace(';', ':');
  out += "T;";
  out += QString::number(uniqueId()) + ";";
  out += lName + ";";
  out += QString::number(position().x()) + ";";
  out += QString::number(position().y()) + ";";
  out += QString::number(startState()->uniqueId()) + ";";
  out += QString::number(endState()->uniqueId()) + ";";
  out += mSensorCondition->toString();
  return out;
}

// backward compatibility code
void Transition::fromStringVersion3(const QString &string) {
  Tokenizer tokenizer(string);
  QStringList tokens;

  for (int i=0; i<3; i++) {
    if (tokenizer.hasMoreToken())
      tokens.append(tokenizer.nextToken());
    else
      throw tr("Corrupted Transition (wrong number of parameters)");
  }

  QString name = tokens[0];
  int startId = tokens[1].toInt();
  int endId = tokens[2].toInt();

  State *startState = Model::instance()->automaton()->findStateById(startId);
  State *endState = Model::instance()->automaton()->findStateById(endId);

  if (startState == NULL || endState == NULL)
    throw tr("Corrupted transition (bad reference to state)");

  setPosition((startState->position() + endState->position()) / 2);
  setName(name);
  setStartState(startState);
  setEndState(endState);

  sensorCondition()->fromStringVersion3(tokenizer.remainingString());
}
