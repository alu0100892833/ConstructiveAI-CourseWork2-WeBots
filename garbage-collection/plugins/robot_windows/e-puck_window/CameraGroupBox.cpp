#include "CameraGroupBox.hpp"
#include "EPuckRepresentation.hpp"

#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>

// constructor
CameraGroupBox::CameraGroupBox(QWidget *parent):
  QGroupBox(parent)
{
  mHBox = new QHBoxLayout;
  mLabel = new QLabel;
  mHBox->addWidget(mLabel);
  setLayout(mHBox);

  setTitle("Camera");
}

// destructor
CameraGroupBox::~CameraGroupBox() {
  delete mHBox;
  delete mLabel;
}

// set the image if any into the mLabel
void CameraGroupBox::updateValues() {
  EPuckRepresentation *epuck = EPuckRepresentation::instance();

  bool enable = epuck->isCameraEnabled();
  setEnabled(enable);
  if (!enable)
    return;

  const unsigned char *raw = epuck->cameraValues();
  int width = epuck->cameraWidth();
  int height = epuck->cameraHeight();

  // display the image as it is twice bigger
  if (raw) { // an image is there
    QImage image(raw, width, height, QImage::Format_ARGB32);
    mLabel->setPixmap(QPixmap::fromImage(image).scaledToWidth(2*width));
  } else { // the device is not enabled or not defined
    int size = 3*width*height;
    const unsigned char *raw[size];
    memset(raw, 0, size);
    QImage image((const unsigned char *)raw, width, height, QImage::Format_RGB888);
    mLabel->setPixmap(QPixmap::fromImage(image).scaledToWidth(2*width));
  }
}
