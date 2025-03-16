# FreeCamera

[![Minecraft - Version](https://img.shields.io/badge/minecraft-v1.21.60_(Bedrock)-black)](https://feedback.minecraft.net/hc/en-us/sections/360001186971-Release-Changelogs)
![GitHub Tag](https://img.shields.io/github/v/tag/GroupMountain/FreeCamera)
![GitHub License](https://img.shields.io/github/license/GroupMountain/FreeCamera)

[ [简体中文](README_zh.md) | [English](README.md) ]

A high-performance free camera mod for Minecraft Bedrock Dedicated Server (BDS).

## 🎇 Features

- **No Fake Players** - Pure packet-based solution ensures server performance.

- **Safety First** - Automatically reverts to survival mode upon taking damage.

- **Client-Side Magic** - Manipulates client state without server-side hacks.

- **Simple Commands** - Toggle with /freecamera or /fc.

## 📦 Installation

1. 📋 Install Dependencies:
    - Ensure [`LeviLamina`](https://github.com/LiteLDev/LeviLamina) is installed.
    - Download [`GMLIB`](https://github.com/GroupMountain/GMLIB-Release) and install it.
2. 🎯 Install FreeCamera
    - Download the latest release from [Releases](https://github.com/GroupMountain/FreeCamera/releases).
    - Extract the `FreeCamera` folder into `./plugins/` directory.

## 🧭 Usage

- **Enter Free Camera**: Run  `/freecamera` or `/fc` in-game.
- **Exit Free Camera**: Use the same command or take damage to auto-exit.

## 🧪 Principle

This plugin tricks the client into Spectator Mode via fake packets, while the server maintains the actual game mode.  
However, the server still operates in the original game mode, and players will still take damage normally.

## 📝 Feedback and Contributions

- Found a bug? Open an [Issue](https://github.com/GroupMountain/FreeCamera/issues).
- Contributions welcome! Fork and submit a Pull Request.

## 📜 License

Distributed under the GPL-V3.0 License.  
See [LICENSE](LICENSE) for details.