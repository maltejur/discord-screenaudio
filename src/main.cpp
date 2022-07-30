#include "mainwindow.h"
#include "virtmic.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("discord-screenaudio");
  QApplication::setWindowIcon(
      QIcon(":assets/de.shorsh.discord-screenaudio.png"));
  QApplication::setApplicationVersion(DISCORD_SCEENAUDIO_VERSION_FULL);
  QApplication::setDesktopFileName("de.shorsh.discord-screenaudio");

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

  qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
          "--enable-features=WebRTCPipeWireCapturer " +
              qgetenv("QTWEBENGINE_CHROMIUM_FLAGS"));

  MainWindow w;
  w.show();

  return app.exec();
}
