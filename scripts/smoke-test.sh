#!/usr/bin/env bash
# Smoke test for matricX-cowork command protocol on Linux
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EXE="${ROOT_DIR}/tools/matricX-cowork/matricX-cowork"

if [[ ! -f "${EXE}" ]]; then
    echo "matricX-cowork binary not found. Run scripts/build.sh first."
    exit 1
fi

echo "Testing matricX-cowork --describe..."
"${EXE}" --describe > /dev/null

echo "Testing JSON command protocol..."
python3 - << EOF
import subprocess, json

proc = subprocess.Popen(["${EXE}", "--idle-timeout", "10"],
                        stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)

ready = proc.stdout.readline()
assert "ready" in ready, f"Unexpected ready event: {ready}"

for cmd in [{"id": 1, "cmd": "screen_info"},
            {"id": 2, "cmd": "wait", "ms": 20},
            {"id": 3, "cmd": "screenshot"},
            {"id": 4, "cmd": "end_session"}]:
    proc.stdin.write(json.dumps(cmd) + "\n")
    proc.stdin.flush()
    line = proc.stdout.readline()
    data = json.loads(line)
    assert data.get("ok") is True, f"Command failed: {line}"

stopped = proc.stdout.readline()
assert "stopped" in stopped, f"Unexpected stopped event: {stopped}"
proc.wait()
print("All smoke tests passed successfully.")
EOF
