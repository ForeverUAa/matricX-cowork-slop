# MatricX-Cowork v0.3.0 ⚡

High-performance, visual ambient coworking simulation and mouse automation utility for Windows built in C++17 with Direct Win32 & GDI+.

Designed for coworking simulation, active session keep-alive, or ambient screen automation with organic motion and visual feedback.

---

## ✨ Features (v0.3.0)

- **🖱️ Organic Bézier Motion**: Smooth cubic Bézier trajectories with variable perpendicular offsets and smoothstep easing instead of linear steps.
- **🤏 Human-like Micro-Jitter**: Simulates micro-vibrations during dwell pauses (mimicking a hand resting on a physical mouse).
- **🌟 Animated Pulsing Edge Glow**: Dynamic ~20 FPS sinusoidal breathing border overlay that keeps the center clear.
- **🎨 Multiple Color Themes**: Choose between `green`, `cyan`, `rainbow`, or disable with `off`.
- **⏱️ Live Dynamic HUD**: Sleek anti-aliased pill overlay showing real-time countdown (`ESC to quit │ 45s`) or infinite indicator.
- **🛡️ Bulletproof Cursor Safety**: Automatic system cursor restoration on normal exit, Escape, Ctrl+C / close event, and `std::atexit`.
- **⚙️ Full CLI Suite**: Configurable duration, speed profiles, behavioral patterns, and glow themes.

---

## 🚀 Usage

You can run either the compiled binary directly or use the PowerShell launcher:

### 1. PowerShell Launcher
```powershell
.\matricX-cowork.ps1 [options]
```

### 2. Direct Executable
```powershell
.\matricX-cowork.exe [options]
```

### CLI Parameters Reference

| Flag | Parameter | Description | Default |
| :--- | :--- | :--- | :--- |
| `--duration` | `<seconds>` \| `infinite` | Set runtime in seconds or run indefinitely (`0` or `infinite`) | `60` |
| `--speed` | `slow` \| `normal` \| `fast` | Controls step distribution and velocity | `normal` |
| `--pattern` | `wander` \| `work` \| `edge` | Movement bias (`work` stays central, `edge` hugs screen borders) | `wander` |
| `--glow` | `green` \| `cyan` \| `rainbow` \| `off` | Color theme for animated border glow & HUD | `green` |
| `--no-cursor`| _None_ | Keep normal system cursor instead of `akar-icons_cursor.png` | _Off_ |
| `--no-lock` | _None_ | Do not block manual mouse movement | _Off_ |
| `--restore-cursor` | _None_ | Immediately force-restore default Windows cursors & exit | _N/A_ |
| `--help` | _None_ | Show usage reference | _N/A_ |
| `--version` | _None_ | Print version string | _N/A_ |

### Quick Examples

```powershell
# Run indefinitely with rainbow glow and fast movement
.\matricX-cowork.ps1 -Duration infinite -Glow rainbow -Speed fast

# Work simulation pattern (centered), cyan glow for 5 minutes
.\matricX-cowork.ps1 -Duration 300 -Pattern work -Glow cyan

# Free movement (unlocked), no custom cursor
.\matricX-cowork.ps1 -NoLock -NoCursor
```

---

## 🛠️ Build from Source

### MinGW-w64 (GCC)
```powershell
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -static matricX-cowork.cpp -o matricX-cowork.exe -luser32 -lgdi32 -lgdiplus
```

### MSVC (Visual Studio Developer Command Prompt)
```cmd
cl /std:c++17 /EHsc /O2 /W4 matricX-cowork.cpp user32.lib gdi32.lib gdiplus.lib /Fe:matricX-cowork.exe
```

---

## 🔒 Safety & Recovery

- Press **Escape** at any time to immediately restore manual mouse control and close the overlay.
- If an unexpected termination occurs, restore normal Windows cursors instantly:
  ```powershell
  .\matricX-cowork.ps1 -RestoreCursor
  # or
  .\matricX-cowork.exe --restore-cursor
  ```
