# M-Cowork

An AI app that lets people use their AI like OpenCode. Cross-platform desktop UI built with Qt 6 Widgets (C++17) supporting Windows and Linux.

## Requirements

### Windows
- Windows 10/11
- [MSYS2](https://www.msys2.org/) installed at `C:\msys64` (UCRT64 environment)

### Linux
- Modern Linux distribution (Arch/Ubuntu/Fedora/Debian)
- GCC / Clang (C++17)
- CMake (>= 3.16)
- Qt 6 (Widgets, Svg)
- X11 development headers & extensions (`libx11`, `libxtst`, `libxfixes`)
- `libpng`
- Optional: `grim` (for native Wayland screen captures)

## Setup & Build

### Windows (PowerShell)
```powershell
# 1. Install MSYS2 build packages
powershell -ExecutionPolicy Bypass -File .\scripts\install-packages.ps1

# 2. Build the UI and computer-control tool
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1          # incremental
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Clean   # full rebuild

# 3. Run the application
powershell -ExecutionPolicy Bypass -File .\scripts\run.ps1
```

### Linux (Bash / CMake)
```bash
# 1. Build using the build script or CMake
./scripts/build.sh

# 2. Run the application
./scripts/run.sh

# 3. Run the smoke test
./scripts/smoke-test.sh
```

## AI Tool (`matricX-cowork`)
The computer-control engine communicates with the GUI via a JSON-lines protocol over standard I/O:
- In the chat box, type `/tools` to display all available OS automation actions and schemas.
- Run commands manually: `/tool mouse_move {"x":500,"y":300}`, `/tool screen_info`, or `/tool screenshot`.
- The AI provider loop will leverage the same schema interface.

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
