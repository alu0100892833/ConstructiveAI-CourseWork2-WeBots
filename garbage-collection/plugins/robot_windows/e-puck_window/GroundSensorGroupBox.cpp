#include "GroundSensorGroupBox.hpp"
#include "EPuckRepresentation.hpp"

#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>

// components
enum {X, Y, Z};

// constructor
GroundSensorGroupBox::GroundSensorGroupBox(QWidget *parent):
  QGroupBox(parent)
{
  // set the mLabels into this widget
  mHBox = new QHBoxLayout;
  for (int i=0; i<3; i++) {
    mLabels[i] = new QLabel;
    mLabels[i]->setFixedSize(20, 20);
    mLabels[i]->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mHBox->addWidget(mLabels[i]);
  }
  setLayout(mHBox);

  // set the title
  setTitle("Ground Sensors");
}

// destructor
GroundSensorGroupBox::~GroundSensorGroupBox() {
  delete mHBox;
  for (int i=0; i<3; i++)
    delete mLabels[i];
}

// update the values
void GroundSensorGroupBox::updateValues() {
  EPuckRepresentation *epuck = EPuckRepresentation::instance();

  bool enable = false;

  for (int i=0; i<3; i++) {
    if (epuck->isGroundSensorEnabled(i))
      enable = true;
    double v = epuck->groundSensorValue(i);
    if (v!=-1.0) {
      int c = (v - 300.0)*255.0/700.0;
      if (c > 255)
        c = 255;
      else if (c < 0)
        c = 0;
      QPalette pal = mLabels[i]->palette();
      pal.setColor(mLabels[i]->backgroundRole(), QColor(c,c,c));
      mLabels[i]->setPalette(pal);
      mLabels[i]->setAutoFillBackground(true);
    }
  }

  setEnabled(enable);
}
