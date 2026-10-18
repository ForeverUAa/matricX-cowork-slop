#!/usr/bin/env bash
# Runs M-Cowork on Linux (builds first if needed)
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
EXE="${ROOT_DIR}/build/m_cowork_ui"

if [[ ! -f "${EXE}" ]]; then
    "${ROOT_DIR}/scripts/build.sh"
fi

exec "${EXE}" "$@"
