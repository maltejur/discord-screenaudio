#include "dependencycheck.h"
#include "pipewire-0.3/pipewire/version.h"

#include <QMessageBox>

void checkDependencies() {
  QString versionPipewire(pw_get_library_version());
  QMessageBox::information(nullptr, "Dependency Check",
                           QString("Pipewire: v%1").arg(versionPipewire));
}
