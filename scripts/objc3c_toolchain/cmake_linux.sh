#!/usr/bin/env bash
# Private, checksum-pinned CMake for the Linux development and archive consumers.
set -euo pipefail
version=4.4.5
[[ $# == 1 ]] || { echo 'usage: cmake_linux.sh INSTALL_ROOT' >&2; exit 2; }
root=$1
if [[ ! -x "$root/bin/cmake" ]]; then
  [[ ! -e "$root" ]] || { echo "incomplete CMake installation: $root" >&2; exit 1; }
  parent=$(dirname -- "$root")
  mkdir -p "$parent"
  scratch=$(mktemp -d "$parent/.cmake-install.XXXXXX")
  trap 'rm -rf -- "$scratch"' EXIT
  archive="$scratch/cmake.tar.gz"
  curl -fsSL --retry 3 --max-filesize 100000000 \
    "https://cmake.org/files/v4.4/cmake-$version-linux-x86_64.tar.gz" -o "$archive"
  printf '%s  %s\n' ed85135c5a5135b45e50423cb406dea04ca3338da0b8ef3c9ede963559680be0 "$archive" | sha256sum -c -
  tar -xzf "$archive" -C "$scratch"
  mv -- "$scratch/cmake-$version-linux-x86_64" "$root"
fi
[[ $("$root/bin/cmake" --version | head -n 1) == "cmake version $version" ]] || {
  echo 'CMake installation differs from the admitted release' >&2; exit 1;
}
for tool in cmake ctest cpack; do test -x "$root/bin/$tool"; done
