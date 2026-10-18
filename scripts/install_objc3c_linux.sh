#!/bin/sh
set -eu
# The consumer installer uses the distribution Python; compiler development pins
# its own Python separately. No checkout, npm, PowerShell or bootstrap LLVM needed.
if [ "${1:-}" = --provision ]; then
  shift
  if [ "$(id -u)" != 0 ]; then
    echo '--provision requires root to install distribution packages' >&2
    exit 2
  fi
  apt-get update
  DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    python3 zstd g++ libc6-dev libzstd-dev zlib1g-dev
fi
installer_dir=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
if [ -f "$installer_dir/objc3c_toolchain/install_sdk.py" ]; then
  installer="$installer_dir/objc3c_toolchain/install_sdk.py"
else
  installer="$installer_dir/install-sdk.py"
fi
exec python3 "$installer" "$@"
