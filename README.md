# matricX-cowork

Computer-control **tool** for the M-Cowork AI client (Windows). It is not autonomous: it only does what the client asks, one command at a time. While running it shows a glow border, an `ESC to stop | AI: <status>` pill and the agent cursor. Press the physical **Esc** key to abort at any time.

## Protocol (JSON lines)
stdin: `{"id":1,"cmd":"mouse_move","x":500,"y":300}` -> stdout: `{"id":1,"ok":true,"x":500,"y":300}`
Errors: `{"id":N,"ok":false,"error":"..."}`. Events: `{"event":"ready",...}` on start, `{"event":"stopped","reason":"escape|idle_timeout|input_closed|end_session|console_signal"}` on exit.

Commands: `screen_info`, `screenshot {path?}`, `mouse_move {x,y}`, `mouse_click {x?,y?,button?,count?}`, `mouse_drag {x,y,from_x?,from_y?}`, `mouse_scroll {dx?,dy?,x?,y?}`, `keyboard_type {text,delay_ms?}`, `keyboard_key {keys}`, `wait {ms}`, `end_session`.
`matricX-cowork.exe --describe` prints all of them as AI tool schemas (`name`, `description`, `input_schema`).

## Options
`--speed slow|normal|fast` `--glow green|cyan|rainbow|off` `--idle-timeout <sec>` (default 120, 0 = never) `--no-cursor` `--no-lock` `--describe` `--restore-cursor` `--help` `--version`

## Build
`g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -static matricX-cowork.cpp -o matricX-cowork.exe -luser32 -lgdi32 -lgdiplus -lwinmm`
or `.\matricX-cowork.ps1 -Build`. Test: `.\smoke-test.ps1`.

## Safety
Esc aborts (injected Escape from the tool itself does not). If the cursor looks stuck: `matricX-cowork.exe --restore-cursor`.
