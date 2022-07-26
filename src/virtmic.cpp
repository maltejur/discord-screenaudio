#include "virtmic.h"

#include <rohrkabel/loop/main.hpp>
#include <rohrkabel/registry/registry.hpp>

namespace Virtmic {

QVector<QString> getTargets() {
  auto main_loop = pipewire::main_loop();
  auto context = pipewire::context(main_loop);
  auto core = pipewire::core(context);
  auto reg = pipewire::registry(core);

  QVector<QString> targets;

  auto reg_listener = reg.listen<pipewire::registry_listener>();
  reg_listener.on<pipewire::registry_event::global>(
      [&](const pipewire::global &global) {
        if (global.type == pipewire::node::type) {
          auto node = reg.bind<pipewire::node>(global.id);
          auto info = node.info();

          if (info.props.count("node.name")) {
            auto name = QString::fromStdString(info.props["node.name"]);
            if (!targets.contains(name))
              targets.append(name);
          }
        }
      });
  core.update();

  return targets;
}

void start(QString _target) {
  std::map<std::uint32_t, pipewire::port> ports;
  std::unique_ptr<pipewire::port> virt_fl, virt_fr;

  std::map<std::uint32_t, pipewire::node_info> nodes;
  std::map<std::uint32_t, pipewire::link_factory> links;

  auto main_loop = pipewire::main_loop();
  auto context = pipewire::context(main_loop);
  auto core = pipewire::core(context);
  auto reg = pipewire::registry(core);

  auto link = [&](const std::string &target, pipewire::core &core) {
    for (const auto &[port_id, port] : ports) {
      if (!virt_fl || !virt_fr)
        continue;

      if (links.count(port_id))
        continue;

      if (port.info().direction == pipewire::port_direction::input)
        continue;

      if (!port.info().props.count("node.id"))
        continue;

      auto parent_id = std::stoul(port.info().props["node.id"]);

      if (!nodes.count(parent_id))
        continue;

      auto &parent = nodes.at(parent_id);

      if (parent.props["node.name"].find(target) != std::string::npos) {
        std::cout << "[virtmic] "
                  << "Link   : " << target << ":" << port_id << " -> ";

        if (port.info().props["audio.channel"] == "FL") {
          links.emplace(port_id, core.create_simple<pipewire::link_factory>(
                                     virt_fl->info().id, port_id));
          std::cout << "[virtmic] " << virt_fl->info().id << std::endl;
        } else {
          links.emplace(port_id, core.create_simple<pipewire::link_factory>(
                                     virt_fr->info().id, port_id));
          std::cout << "[virtmic] " << virt_fr->info().id << std::endl;
        }
      }
    }
  };

  std::string target = _target.toLatin1().toStdString();

  auto virtual_mic = core.create("adapter",
                                 {{"node.name", "discord-screenaudio-virtmic"},
                                  {"media.class", "Audio/Source/Virtual"},
                                  {"factory.name", "support.null-audio-sink"},
                                  {"audio.channels", "2"},
                                  {"audio.position", "FL,FR"}},
                                 pipewire::node::type, pipewire::node::version,
                                 pipewire::update_strategy::none);

  if (target == "None") {
    while (true) {
      main_loop.run();
    }
    return;
  }

  auto reg_events = reg.listen<pipewire::registry_listener>();
  reg_events.on<pipewire::registry_event::global>(
      [&](const pipewire::global &global) {
        if (global.type == pipewire::node::type) {
          auto node = reg.bind<pipewire::node>(global.id);
          std::cout << "[virtmic] "
                    << "Added  : " << node.info().props["node.name"]
                    << std::endl;

          if (!nodes.count(global.id)) {
            nodes.emplace(global.id, node.info());
            link(target, core);
          }
        }
        if (global.type == pipewire::port::type) {
          auto port = reg.bind<pipewire::port>(global.id);
          auto info = port.info();

          if (info.props.count("node.id")) {
            auto node_id = std::stoul(info.props["node.id"]);

            if (node_id == virtual_mic.id() &&
                info.direction == pipewire::port_direction::input) {
              if (info.props["audio.channel"] == "FL") {
                virt_fl = std::make_unique<pipewire::port>(std::move(port));
              } else {
                virt_fr = std::make_unique<pipewire::port>(std::move(port));
              }
            } else {
              ports.emplace(global.id, std::move(port));
            }

            link(target, core);
          }
        }
      });

  reg_events.on<pipewire::registry_event::global_removed>(
      [&](const std::uint32_t id) {
        if (nodes.count(id)) {
          auto info = nodes.at(id);
          std::cout << "[virtmic] "
                    << "Removed: " << info.props["node.name"] << std::endl;
          nodes.erase(id);
        }
        if (ports.count(id)) {
          ports.erase(id);
        }
        if (links.count(id)) {
          links.erase(id);
        }
      });

  while (true) {
    main_loop.run();
  }
}

} // namespace Virtmic
