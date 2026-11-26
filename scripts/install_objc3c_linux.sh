#!/bin/sh
set -eu
# The consumer installer uses the distribution Python; compiler development pins
# its own Python separately. No checkout, npm, PowerShell or bootstrap LLVM needed.
fail() {
  printf 'objc3c install: %s\n' "$1" >&2
  exit 2
}

installer_dir=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
case ${0##*/} in
  install_objc3c_linux.sh) installer="$installer_dir/objc3c_toolchain/install_sdk.py" ;;
  *) installer="$installer_dir/install-sdk.py" ;;
esac
[ -f "$installer" ] && [ -r "$installer" ] || fail "missing readable installer companion: $installer"

check_archive() (
  archive=
  checksum=
  while [ "$#" -gt 0 ]; do
    case $1 in
      --archive|--sha256|--root)
        case ${2:-} in ''|-*) fail "$1 needs a value" ;; esac
        option=$1
        value=$2
        shift
        ;;
      --archive=*|--sha256=*|--root=*)
        option=${1%%=*}
        value=${1#*=}
        [ -n "$value" ] || fail "$option needs a value"
        ;;
      *) fail "unsupported argument for --provision: $1" ;;
    esac
    case $option in
      --archive) archive=$value ;;
      --sha256) checksum=$value ;;
    esac
    shift
  done
  [ -f "$archive" ] && [ -r "$archive" ] || fail '--provision needs a readable --archive file'
  [ "${#checksum}" -eq 64 ] || fail '--provision needs a 64-digit --sha256'
  case $checksum in *[!0-9a-fA-F]*) fail '--provision needs a 64-digit --sha256' ;; esac
  checksum=$(printf '%s' "$checksum" | tr 'A-F' 'a-f')
  actual=$(sha256sum < "$archive")
  [ "${actual%% *}" = "$checksum" ] || fail 'archive SHA-256 does not match the release receipt'
)

provision() {
  if installed=$(dpkg-query -W -f='${db:Status-Status}\n' "$@" 2>/dev/null) &&
     printf '%s\n' "$installed" | awk -v count="$#" '$0 == "installed" { ready++ } END { exit (NR != count || ready != count) }'; then
    return
  fi
  apt-get update
  DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends "$@"
}

if [ "${1:-}" = --provision ]; then
  shift
  if [ "$#" = 1 ] && { [ "$1" = --help ] || [ "$1" = -h ]; }; then
    printf 'Usage: sh %s --provision --archive FILE --sha256 SHA256 [--root DIRECTORY]\n' "${0##*/}"
    exit 0
  fi
  [ "$(id -u)" = 0 ] || fail '--provision requires root to install distribution packages'
  # The base system can validate the requested bytes before modifying apt state.
  check_archive "$@"
  provision python3 zstd g++ libc6-dev libzstd-dev zlib1g-dev libssl-dev
fi
exec python3 "$installer" "$@"
