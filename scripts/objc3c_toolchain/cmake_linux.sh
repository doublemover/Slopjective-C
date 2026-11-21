#!/usr/bin/env bash
# Private, checksum-pinned CMake for native Linux development and archive consumers.
set -euo pipefail
version=4.4.5
[[ $# == 1 || $# == 2 ]] || { echo 'usage: cmake_linux.sh INSTALL_ROOT [LINUX_HOST_PINS]' >&2; exit 2; }
root=$1
pins=${2:-$(dirname -- "${BASH_SOURCE[0]}")/../../toolchains/linux-hosts.json}
mapfile -t asset < <(python3 -B - "$pins" "$version" <<'PY'
import json, platform, re, sys
from urllib.parse import urlsplit
hosts = {'x86_64': 'linux-x86_64', 'amd64': 'linux-x86_64',
         'aarch64': 'linux-aarch64', 'arm64': 'linux-aarch64'}
if platform.system() != 'Linux' or platform.machine().lower() not in hosts:
    raise SystemExit('CMake installation requires native Linux x86-64 or AArch64')
document = json.load(open(sys.argv[1]))
if document.get('schema') != 'objc3c.linux-native-hosts.v1' or document['versions']['cmake'] != sys.argv[2]:
    raise SystemExit('CMake host pins differ from the admitted release')
selected = document['hosts'][hosts[platform.machine().lower()]]
asset = selected['assets']['cmake']
arch = 'x86_64' if selected['elf_machine'] == 62 else 'aarch64'
prefix = 'cmake-' + sys.argv[2] + '-linux-' + arch
expected = 'https://github.com/Kitware/CMake/releases/download/v' + sys.argv[2] + '/' + prefix + '.tar.gz'
if (asset['url'] != expected or selected['prefixes']['cmake'] != prefix or
        not re.fullmatch('[0-9a-f]{64}', asset['sha256']) or
        type(asset['asset_id']) is not int or asset['asset_id'] <= 0):
    raise SystemExit('CMake host asset identity is invalid')
print(asset['url']); print(asset['sha256']); print(prefix); print(selected['elf_machine']); print(asset['asset_id'])
PY
)
[[ ${#asset[@]} == 5 ]] || { echo 'Cannot select pinned native CMake asset' >&2; exit 1; }
check_elf() {
  python3 -B - "$1" "${asset[3]}" <<'PY'
import sys
with open(sys.argv[1], 'rb') as stream: header = stream.read(64)
if len(header) != 64 or header[:6] != b'\x7fELF\x02\x01' or int.from_bytes(header[18:20], 'little') != int(sys.argv[2]):
    raise SystemExit('CMake executable differs from the native ELF architecture: ' + sys.argv[1])
PY
}
if [[ ! -x "$root/bin/cmake" ]]; then
  [[ ! -e "$root" ]] || { echo "incomplete CMake installation: $root" >&2; exit 1; }
  parent=$(dirname -- "$root")
  mkdir -p "$parent"
  scratch=$(mktemp -d "$parent/.cmake-install.XXXXXX")
  trap 'rm -rf -- "$scratch"' EXIT
  archive="$scratch/cmake.tar.gz"
  if ! curl -fsSL --retry 3 --max-filesize 100000000 "${asset[0]}" -o "$archive"; then
    curl -fsSL --retry 3 --max-filesize 100000000 -H 'Accept: application/octet-stream' \
      "https://api.github.com/repos/Kitware/CMake/releases/assets/${asset[4]}" -o "$archive"
  fi
  printf '%s  %s\n' "${asset[1]}" "$archive" | sha256sum -c -
  tar -xzf "$archive" -C "$scratch"
  for tool in cmake ctest cpack; do check_elf "$scratch/${asset[2]}/bin/$tool"; done
  mv -- "$scratch/${asset[2]}" "$root"
fi
for tool in cmake ctest cpack; do check_elf "$root/bin/$tool"; done
[[ $("$root/bin/cmake" --version | head -n 1) == "cmake version $version" ]] || {
  echo 'CMake installation differs from the admitted release' >&2; exit 1;
}
