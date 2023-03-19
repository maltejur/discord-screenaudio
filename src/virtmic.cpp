#include "virtmic.h"
#include "log.h"

QThread virtmicThread;
std::unique_ptr<pipewire::sender<Virtmic::set_target, Virtmic::terminate>>
    senderr;
std::unique_ptr<cr::receiver<Virtmic::new_targets>> receiverr;

void Virtmic::instance() {
  if (!virtmicThread.isRunning()) {
    auto [main_sender, main_receiver] = cr::channel<new_targets>();
    auto [pw_sender, pw_receiver] = pipewire::channel<set_target, terminate>();
    Virtmic *virtmic =
        new Virtmic(std::move(pw_receiver), std::move(main_sender));
    virtmic->moveToThread(&virtmicThread);
    virtmicThread.start();
    QMetaObject::invokeMethod(virtmic, "run");
    receiverr = std::make_unique<cr::receiver<Virtmic::new_targets>>(
        std::move(main_receiver));
    senderr = std::make_unique<
        pipewire::sender<Virtmic::set_target, Virtmic::terminate>>(
        std::move(pw_sender));
  }
}

void Virtmic::setTarget(QString target) {
  senderr.get()->send<Virtmic::set_target>({target});
}

void Virtmic::getTargets() { senderr.get()->send<Virtmic::get_targets>(); }

Virtmic::Virtmic(pipewire::receiver<set_target, terminate> receiver,
                 cr::sender<new_targets> sender) {
  m_receiver = std::make_unique<pipewire::receiver<set_target, terminate>>(
      std::move(receiver));
  m_sender = std::make_unique<cr::sender<new_targets>>(std::move(sender));
  virtual_mic = std::make_unique<pipewire::proxy>(
      std::move(*core.create("adapter",
                             {{"node.name", "discord-screenaudio-virtmic"},
                              {"media.class", "Audio/Source/Virtual"},
                              {"factory.name", "support.null-audio-sink"},
                              {"audio.channels", "2"},
                              {"audio.position", "FL,FR"}},
                             pipewire::node::type, pipewire::node::version,
                             pipewire::update_strategy::none)
                     .get()));
  metadata_listener.on<pipewire::registry_event::global>(
      [&](const auto &global) { globalEvent(global); });
  metadata_listener.on<pipewire::registry_event::global_removed>(
      [&](const std::uint32_t id) { globalRemovedEvent(id); });
}

void Virtmic::run() {
  while (true) {
    main_loop.run();
  }
}

void Virtmic::link() {
  for (const auto &[port_id, port] : ports) {
    auto port_info = port.info();
    if (!virt_fl || !virt_fr)
      continue;

    if (links.count(port_id))
      continue;

    if (port_info.direction == pipewire::port_direction::input)
      continue;

    if (!port_info.props.count("node.id"))
      continue;

    auto parent_id = std::stoul(port_info.props["node.id"]);

    if (!nodes.count(parent_id))
      continue;

    auto &parent = nodes[parent_id];
    QString name;
    if (parent.props.count("application.name") &&
        parent.props["application.name"] != "")
      name = QString::fromStdString(parent.props["application.name"]);
    else
      name = QString::fromStdString(parent.props["application.process.binary"]);

    if (name == target ||
        (target == "[All Desktop Audio]" && !EXCLUDE_TARGETS.contains(name))) {
      auto fl = port_info.props["audio.channel"] == "FL";
      links.emplace(port_id,
                    *core.create_simple<pipewire::link>(fl ? virt_fl->info().id
                                                           : virt_fr->info().id,
                                                        port_id)
                         .get());
      qDebug(virtmicLog) << QString("Link: %1:%2 -> %3")
                                .arg(name)
                                .arg(port_id)
                                .arg(fl ? virt_fl->info().id
                                        : virt_fr->info().id)
                                .toUtf8()
                                .data();
    }
  }
}

void Virtmic::unlink() { links.clear(); }

void Virtmic::globalEvent(const pipewire::global &global) {
  if (global.type == pipewire::node::type) {
    auto node = *reg.bind<pipewire::node>(global.id).get();
    auto info = node.info();
    std::string name;
    if (info.props.count("application.name") &&
        info.props["application.name"] != "")
      name = info.props["application.name"];
    else if (info.props.count("application.process.binary")) {
      name = info.props["application.process.binary"];
    } else
      return;
    qDebug(virtmicLog) << QString("Added: %1")
                              .arg(QString::fromStdString(name))
                              .toUtf8()
                              .data();

    if (!nodes.count(global.id)) {
      nodes.emplace(global.id, node.info());
      link();
    }
  }
  if (global.type == pipewire::port::type) {
    auto port = *reg.bind<pipewire::port>(global.id).get();
    auto info = port.info();

    if (info.props.count("node.id")) {
      auto node_id = std::stoul(info.props["node.id"]);

      if (node_id == virtual_mic.get()->id() &&
          info.direction == pipewire::port_direction::input) {
        if (info.props["audio.channel"] == "FL") {
          virt_fl = std::make_unique<pipewire::port>(std::move(port));
        } else {
          virt_fr = std::make_unique<pipewire::port>(std::move(port));
        }
      } else {
        ports.emplace(global.id, std::move(port));
      }

      link();
    }
  }
}

void Virtmic::globalRemovedEvent(const std::uint32_t id) {
  if (nodes.count(id)) {
    auto info = nodes.at(id);
    std::string name;
    if (info.props.count("application.name") &&
        info.props["application.name"] != "")
      name = info.props["application.name"];
    else
      name = info.props["application.process.binary"];
    qDebug(virtmicLog) << QString("Removed: %1")
                              .arg(QString::fromStdString(name))
                              .toUtf8()
                              .data();
    nodes.erase(id);
  }
  if (ports.count(id)) {
    ports.erase(id);
  }
  if (links.count(id)) {
    links.erase(id);
  }
}
