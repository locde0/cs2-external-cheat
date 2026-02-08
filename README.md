# 🛡️ CS2 External Kernel ESP

---

## ⚡ Overview
**CS2 External Kernel ESP** - is possibly a high-performance external software for Counter-Strike 2.
the goal of this project is to explore Windows Kernel Driver development, DirectX 11 rendering techniques, and memory manipulation in a modern game environment without injecting code into the target process

## 🚀 Features
- **Kernel Level Access** - reads memory via a custom Kernel Mode Driver (`kmd`) deployed via manual mapping
- **Stealth Injection** - utilizes `kdmapper` to load the driver without creating a system service
- **Zero Input Lag** - uses DirectComposition for a transparent overlay that does not interfere with mouse input or game performance
- **Auto-Update** - automatically fetches the latest game offsets from GitHub on startup
- **Hot Reload Config** - settings are monitored in a separate thread; changes in `settings.ini` apply instantly without restarting

## 👁️ Visuals (ESP)
- **Player ESP**
  - **Box:** outlined 2d boxes with customizable thickness
  - **Health Bar:** dynamic gradient (green → yellow → red) based on HP
  - **Team Check:** configurable filtering for enemies and teammates

---

## ⚙️ Configuration (`settings.ini`)
configuration file is automatically generated. you can modify it while the software is running

**🖥️ System & Overlay:**
- `delay` - loop sleep time in ms (lower = smoother, higher = less CPU usage)

**🎨 ESP Settings:**
- `box_thickness` - thickness of the ESP box lines (e.g., `1` or `2`)
- `bar_width` - width of the health bar in pixels
- `color` - normalized RGBA format (e.g., `1.0, 0.0, 0.0, 1.0` for red)

---

## 🛠️ Technologies
- **Language:** C++20
- **Render:** DirectX 11 + DirectComposition
- **System:** Windows Kernel Driver (Manual Map)
- **Dependencies:** nlohmann/json, kdmapper

---

## 🏗️ How to Build & Run

### System Preparation (Important)
before building, configure Windows Security to prevent interference

1. **Disable Core Isolation & Blocklists:**
   - go to **Windows Security** → **Device Security** → **Core Isolation Details**
   - turn **OFF** `Memory Integrity`
   - turn **OFF** `Microsoft Vulnerable Driver Blocklist` (CRITICAL: required for `kdmapper` to load the vulnerable driver)
2. **Add Exclusions:**
   - go to **Windows Security** → **Virus & threat protection** → **Manage settings** → **Exclusions**
   - add the project folder path to prevent Windows Defender from deleting `kdmapper.exe`

### Prerequisites
install tools in the following **strict order**:

1. **Visual Studio 2022**
   - install via **VS Installer** with workload: **Desktop development with C++**
2. **Windows Driver Kit (WDK)**
   - download the standalone **WDK Installer** from Microsoft (match version with your Windows SDK) and install it
3. **Visual Studio Components**
   - open **VS Installer** again → **Modify** → **Individual Components** tab
   - search and install/verify these are checked:
     - `Windows Driver Kit`
     - `MSVC v143 - VS 2022 C++ x64/x86 Spectre-mitigated libs (Latest)`
4. **Restart PC**
   - reboot to apply environment variables and driver frameworks

### Building
1. open the solution file (`.sln`) in Visual Studio 2022
2. select **Release** configuration and **x64** platform
3. build the Solution (`Ctrl+Shift+B`)
    - all binaries (`kmd.sys`, `kdmapper.exe`, `client.exe`) will be output to the `build` folder

### Usage
> **Note:** It is recommended to restart your PC before running to ensure a clean memory state for the mapper.

1. **Run Loader:** execute `load.bat` as Administrator
   - this script will automatically map the driver and launch the client
2. **Start Game:** run Counter-Strike 2
3. **Enjoy:** overlay should appear immediately. edit `settings.ini` to customize colors and features in real-time

---

## ⚠️ Disclaimer
> **Educational Purposes Only**
> this software is designed strictly for educational purposes to demonstrate reverse engineering and OS architecture concepts. the author is not responsible for any misuse of this software or game bans