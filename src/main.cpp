#include "localserver.h"
#include "mainwindow.h"
#include "virtmic.h"

#ifdef KXMLGUI
#include <KAboutData>
#endif

#include <QApplication>
#include <QCommandLineParser>
#include <QLocalServer>
#include <QLocalSocket>
#include <QLoggingCategory>
#include <QMessageBox>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QApplication::setApplicationName("discord-screenaudio");
  QApplication::setWindowIcon(
      QIcon(":assets/de.shorsh.discord-screenaudio.png"));
  QApplication::setApplicationVersion(DISCORD_SCEENAUDIO_VERSION_FULL);
  QApplication::setDesktopFileName("de.shorsh.discord-screenaudio");

  qSetMessagePattern("[%{category}] %{message}");

  QCommandLineParser parser;
  parser.setApplicationDescription(
      "Custom Discord client with the ability to stream audio on Linux");
  parser.addHelpOption();
  parser.addVersionOption();
  QCommandLineOption virtmicOption("virtmic", "Start the Virtual Microphone",
                                   "target");
  parser.addOption(virtmicOption);
  QCommandLineOption degubOption("remote-debugging",
                                 "Open Chromium Remote Debugging on port 9222");
  parser.addOption(degubOption);
  QCommandLineOption notifySendOption(
      "notify-send", "Use notify-send instead of QT/KF5 notifications");
  parser.addOption(notifySendOption);

  parser.process(app);

  if (parser.isSet(virtmicOption)) {
    Virtmic::start(parser.value(virtmicOption));
  }

  qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
          "--enable-features=WebRTCPipeWireCapturer " +
              qgetenv("QTWEBENGINE_CHROMIUM_FLAGS"));

  if (parser.isSet(degubOption))
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS",
            "--remote-debugging-port=9222 " +
                qgetenv("QTWEBENGINE_CHROMIUM_FLAGS"));

  MainWindow w(parser.isSet(notifySendOption));

  // Check if discord is already running
  QString program_name = "discord-screenaudio";
  if (isProgramRunning(program_name)) {
    // if running show error message
    showErrorMessage("discord-screenaudio is already running");
    return 1;
  }

  // open server so we can check if discord is running
  QLocalServer server;
  server.listen(program_name);
  QObject::connect(&server, &QLocalServer::newConnection, []() {});

  w.show();

  return app.exec();
}
