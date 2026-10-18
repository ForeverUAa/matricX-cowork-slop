# M-Cowork

An AI app that lets people use their AI like OpenCode. Native Windows desktop UI built with Qt 6 Widgets (C++17).

## Project layout

```
ai app/
├── assets/
│   ├── icons/            SVG icons (app, gear, mic, mic-hover)
│   └── images/           Design prototype (prototype.png)
├── packages/
│   └── msys2-packages.txt   MSYS2 packages required to build
├── scripts/
│   ├── install-packages.ps1 Installs the packages above
│   ├── build.ps1            Incremental build -> build\m_cowork_ui.exe
│   └── run.ps1              Runs the app (builds first if needed)
├── src/
│   ├── main.cpp
│   ├── core/             Theme.h (colors/sizes), Assets (asset path + SVG loader)
│   ├── widgets/          OutlinedLabel, CircleButton, WindowControls, IconButton
│   └── ui/               MainWindow, Sidebar, ChatArea
└── build/                Build output (git-ignored)
```

## Requirements

- Windows
- [MSYS2](https://www.msys2.org/) installed at `C:\msys64`

## Setup

From PowerShell in the project folder, install the build packages (GCC, pkgconf, Qt 6 base + SVG):

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\install-packages.ps1
```

## Build

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1          # incremental
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Clean   # full rebuild
```

## Run

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\run.ps1
```

## Notes

- The window is frameless and rounded, but still behaves like a normal Windows window
  (drag from the top strip, Aero Snap, resize from any edge, maximize, taskbar minimize).
- Assets are located at runtime by walking up from the executable's folder to `assets/`.
## AI tool (matricX-cowork)
Run `scripts\sync-cowork-tool.ps1` to build and copy the computer-control tool into `tools/`. In the chat box: `/tools` lists tools, `/tool mouse_move {"x":500,"y":300}` runs one. The AI provider will call the same interface.

## Status

### Done
- Qt 6 Widgets UI matching the prototype: sidebar, chat area, outlined "M-Cowork" title, input bar, gear and mic buttons.
- Frameless rounded window with macOS-style hover symbols on the min/max/close buttons (yellow/green/red).
- Native Windows behavior: drag, Aero Snap, resize edges, maximize, taskbar minimize.
- Project structure: `src/` modules, `assets/`, `packages/`, `scripts/` (install, build, run, sync-cowork-tool).
- `matricX-cowork` v0.4.0 rewritten as a JSON-lines computer-control tool (screenshot, mouse, keyboard, wait, end_session, `--describe` schemas, Esc to abort, idle timeout).
- Client integration: `src/tools/CoworkTool` wrapper, `/tools` and `/tool <name> {json}` in the chat box.
- AI provider: settings panel behind the gear (provider selection: Gemini, OpenAI, Claude, Groq, Ollama; API key, model, custom baseUrl; persisted via QSettings).

### Left
- Test mouse_click, drag, scroll and keyboard_type through the client (only screen_info, mouse_move, screenshot and error paths were tested).
- Test `/tool` end to end from the chat box.
- Agent loop: send `definitions()` to the model, run its tool calls through `CoworkTool`, feed results back (screenshots as images).
- Real chat history in the sidebar, multiple chats, message persistence.
- Streaming responses, markdown rendering, error and retry UI.
- Mic button: voice input.
- Permission prompt before the AI takes control; stop button in the client.
- Installer or packaged release; commit and push (nothing committed yet).
