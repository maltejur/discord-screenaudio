#pragma once

#include <cr/channel.hpp>
#include <iostream>
#include <rohrkabel/channel/channel.hpp>
#include <rohrkabel/main_loop.hpp>
#include <rohrkabel/registry/registry.hpp>

#include <QMap>
#include <QScopedPointer>
#include <QStringList>
#include <QThread>

class Virtmic : public QObject {
  Q_OBJECT
public:
  static void setTarget(QString target);
  static void getTargets();

public:
  struct set_target {
    QString name;
  };
  struct get_targets {};
  struct terminate {};
  struct new_targets {
    QStringList targets;
  };

protected:
  static void instance();

protected:
  Virtmic(pipewire::receiver<set_target, terminate> receiver,
          cr::sender<new_targets> sender);
  void run();

private:
  std::unique_ptr<pipewire::receiver<set_target, terminate>> m_receiver;
  std::unique_ptr<cr::sender<new_targets>> m_sender;

  const QStringList EXCLUDE_TARGETS{"Chromium input", "discord-screenaudio"};
  QString target;

  pipewire::main_loop main_loop = pipewire::main_loop();
  pipewire::context context = pipewire::context(main_loop);
  pipewire::core core = pipewire::core(context);
  pipewire::registry reg = pipewire::registry(core);

  pipewire::registry_listener metadata_listener =
      reg.listen<pipewire::registry_listener>();
  std::unique_ptr<pipewire::proxy> virtual_mic;

  std::map<uint32_t, pipewire::port> ports;
  std::unique_ptr<pipewire::port> virt_fl, virt_fr;
  std::map<uint32_t, pipewire::node_info> nodes;
  std::map<uint32_t, pipewire::link> links;

  void link();
  void unlink();
  void globalEvent(const pipewire::global &global);
  void globalRemovedEvent(const std::uint32_t id);
};
