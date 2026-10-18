#!/usr/bin/env bash
# Builds M-Cowork and matricX-cowork on Linux using CMake
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"

echo "==> Configuring with CMake..."
cmake -B "${BUILD_DIR}" -S "${ROOT_DIR}" -DCMAKE_BUILD_TYPE=Release

echo "==> Building targets..."
cmake --build "${BUILD_DIR}" -- -j"$(nproc)"

echo "==> Build finished successfully."
echo "UI Executable: ${BUILD_DIR}/m_cowork_ui"
echo "Tool Binary:   ${ROOT_DIR}/tools/matricX-cowork/matricX-cowork"
