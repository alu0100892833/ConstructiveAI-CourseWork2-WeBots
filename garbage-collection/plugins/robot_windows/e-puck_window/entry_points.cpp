#include "entry_points.hpp"

#include <core/MainApplication.hpp>
#include "Gui.hpp"
#include "EPuckRepresentation.hpp"

using namespace webotsQtUtils;

static MainApplication *gApplication = NULL;
static Gui *gGui = NULL;

bool wbw_init() {
  gApplication = new MainApplication;
  if (gApplication->isInitialized())
    gGui = new Gui;
  return gApplication->isInitialized();
}

void wbw_cleanup() {
  if (gGui) {
    delete gGui;
    gGui = NULL;
  }
  if (gApplication) {
    delete gApplication;
    gApplication = NULL;
  }
}

void wbw_pre_update_gui() {
  if (gApplication && gApplication->isInitialized())
    gApplication->preUpdateGui();
}

void wbw_update_gui() {
  if (gApplication && gApplication->isInitialized())
    gApplication->updateGui();
}

void wbw_read_sensors() {
  if (gGui && gGui->isVisible()) {
    EPuckRepresentation::instance()->update();
    gGui->updateValues();
  }
}

void wbw_write_actuators() {
  // nop: this gui is only a viewer
}

void wbw_show() {
  if (gGui)
    gGui->showWindow();
}
