# 🛡️ CS2 External Kernel ESP

---

## ⚡ Overview
**CS2 External Kernel ESP** - is possibly a high-performance external software for Counter-Strike 2.
The goal of this project is to explore Windows Kernel Driver development, DirectX 11 rendering techniques, and memory manipulation in a modern game environment without injecting code into the target process.

## 🚀 Features
- **Kernel Level Access** - reads memory via a custom Kernel Mode Driver (`kmd`)
- **Zero Input Lag** - uses DirectComposition for a transparent overlay that does not interfere with mouse input or game performance
- **Auto-Update** - automatically fetches the latest game offsets from GitHub on startup
- **Hot Reload Config** - settings are monitored in a separate thread; changes in `settings.ini` apply instantly without restarting

## 👁️ Visuals (ESP)
- **Player ESP**
  - **Box:** outlined 2d boxes with customizable thickness
  - **Health Bar:** dynamic gradient (green → yellow → red) based on HP
  - **Team Check:** configurable filtering for enemies and teammates
- **Rendering**
  - optimized batch drawing using `LineList` and `TriangleList` topologies

---

## ⚙️ Configuration (`settings.ini`)
configuration file is automatically generated. you can modify it while the software is running

**🖥️ System & Overlay:**
- `driver_name` - service name of the kernel driver
- `delay` - loop sleep time in ms (lower = smoother, higher = less CPU usage)

**🎨 ESP Settings:**
- `box_thickness` - thickness of the ESP box lines (e.g., `1` or `2`)
- `bar_width` - width of the health bar in pixels
- `color` - normalized RGBA format (e.g., `1.0, 0.0, 0.0, 1.0` for red)

---

## 🛠️ Technologies
- **Language:** C++20
- **Render:** DirectX 11 + DirectComposition
- **System:** Windows Kernel Driver (WDM)
- **Dependencies:** nlohmann/json

---

## 🏗️ How to Build & Run

### Prerequisites
- Visual Studio 2022 (C++ Desktop Development)
- Windows Driver Kit (WDK)

### Building
1. open the solution file (`.sln`) in Visual Studio 2022
2. select **Release** configuration and **x64** platform
3. build the Solution (`Ctrl+Shift+B`)
    - this will compile both `kmd.sys` (driver) and `client.exe` (client)

### Usage
1. **Start Game:** run Counter-Strike 2
2. **Run Client:** execute `client.exe`
3. **Enjoy:** overlay should appear immediately. edit `settings.ini` to customize colors and features in real-time

---

## ⚠️ Disclaimer
> **Educational Purposes Only**
> this software is designed strictly for educational purposes to demonstrate reverse engineering and OS architecture concepts. the author is not responsible for any misuse of this software or game bans
