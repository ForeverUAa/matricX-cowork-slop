#!/usr/bin/env bash
# Packages M-Cowork for Linux distribution (creates a portable tarball in dist/)
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DIST_DIR="${ROOT_DIR}/dist"
PKG_NAME="m-cowork-linux-x86_64"
STAGE_DIR="${DIST_DIR}/${PKG_NAME}"

echo "==> Building targets in Release mode..."
"${ROOT_DIR}/scripts/build.sh"

echo "==> Staging distribution files..."
rm -rf "${STAGE_DIR}"
mkdir -p "${STAGE_DIR}/bin" "${STAGE_DIR}/assets" "${STAGE_DIR}/tools/matricX-cowork"

# Binaries
cp "${ROOT_DIR}/build/m_cowork_ui" "${STAGE_DIR}/bin/m_cowork_ui"
cp "${ROOT_DIR}/tools/matricX-cowork/matricX-cowork" "${STAGE_DIR}/tools/matricX-cowork/matricX-cowork"
chmod +x "${STAGE_DIR}/bin/m_cowork_ui" "${STAGE_DIR}/tools/matricX-cowork/matricX-cowork"

# Assets & cursor
cp -r "${ROOT_DIR}/assets"/* "${STAGE_DIR}/assets/"
if [[ -f "${ROOT_DIR}/tools/matricX-cowork/akar-icons_cursor.png" ]]; then
    cp "${ROOT_DIR}/tools/matricX-cowork/akar-icons_cursor.png" "${STAGE_DIR}/tools/matricX-cowork/"
fi

# Portable Launcher Script
cat << 'EOF' > "${STAGE_DIR}/m-cowork"
#!/usr/bin/env bash
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export PATH="${HERE}/bin:${HERE}/tools/matricX-cowork:${PATH}"
exec "${HERE}/bin/m_cowork_ui" "$@"
EOF
chmod +x "${STAGE_DIR}/m-cowork"

# Create Tarball
echo "==> Creating tarball..."
tar -C "${DIST_DIR}" -czf "${DIST_DIR}/${PKG_NAME}.tar.gz" "${PKG_NAME}"
echo "==> Packaged distribution ready: ${DIST_DIR}/${PKG_NAME}.tar.gz"
