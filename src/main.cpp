#include "mainwindow.h"
#include "virtmic.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("discord-screenaudio");
  QApplication::setApplicationVersion("1.0.0-rc.6");

  QCommandLineParser parser;
  parser.setApplicationDescription(
      "Custom Discord client with the ability to stream audio on Linux");
  parser.addHelpOption();
  parser.addVersionOption();
  QCommandLineOption virtmicOption("virtmic", "Start the Virtual Microphone",
                                   "target");
  parser.addOption(virtmicOption);
#ifdef DEBUG
  parser.addOption(QCommandLineOption(
      "remote-debugging-port", "Chromium Remote Debugging Port", "port"));
#endif
  parser.process(app);

  if (parser.isSet(virtmicOption)) {
    Virtmic::start(parser.value(virtmicOption));
  }

  MainWindow w;
  w.show();

  return app.exec();
}
