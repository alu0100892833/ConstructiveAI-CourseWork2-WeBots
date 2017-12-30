#include "EncoderGroupBox.hpp"
#include "EPuckRepresentation.hpp"

#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

// prefixes of the mLabels
static const QString prefixes[2] = { QString("left = "), QString("right = ") };

// constructor
EncoderGroupBox::EncoderGroupBox(QWidget *parent):
  QGroupBox(parent)
{
  // set the mLabels into this widget
  mVBox = new QVBoxLayout;
  for (int i=0; i<2; i++) {
    mLabels[i] = new QLabel;
    mLabels[i]->setObjectName("blueLabel");
    mVBox->addWidget(mLabels[i]);
  }
  setLayout(mVBox);

  // set the title
  setTitle("Encoders");
}

// destructor
EncoderGroupBox::~EncoderGroupBox() {
  delete mVBox;
  for (int i=0; i<2; i++)
    delete mLabels[i];
}

// update the mLabels
void EncoderGroupBox::updateValues() {
  EPuckRepresentation *epuck = EPuckRepresentation::instance();

  bool enable = epuck->areEncodersEnabled();
  setEnabled(enable);
  if (!enable)
    return;

  double encoders[] = {
    epuck->leftEncoderValue(),
    epuck->rightEncoderValue()
  };
  for (int i=0; i<2; i++)
    mLabels[i]->setText(prefixes[i] + QString::number((int) encoders[i]));
}
