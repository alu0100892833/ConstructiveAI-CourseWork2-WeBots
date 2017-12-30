/*
 * Description:  Class defining the model of an automaton object
 */

#ifndef AUTOMATON_OBJECT_HPP
#define AUTOMATON_OBJECT_HPP

#include <QtCore/QObject>
#include <QtCore/QPointF>

class AutomatonObject : public QObject
{
  Q_OBJECT

public:
  AutomatonObject(const QPointF &position);
  virtual ~AutomatonObject() {}

  int uniqueId() const { return mUniqueId; }
  const QString &name() const { return mName; }
  const QPointF &position() const { return mPosition; }
  bool isSelected() const { return mIsSelected; }
  static void resetCurrentGlobalId() { cCurrentGlobalId = 0; }

  void setName(const QString &name);
  void setSelected(bool selected);
  void setPosition(const QPointF &position);

signals:
  void nameUpdated();
  void selectionChanged();
  void positionUpdated();

protected:
  static void computeCurrentGlobalId(int id);
  void setUniqueId(int id) { mUniqueId = id; }

private:
  static int cCurrentGlobalId;

  QPointF mPosition;
  QString mName;
  int mUniqueId;
  bool mIsSelected;
};

#endif
