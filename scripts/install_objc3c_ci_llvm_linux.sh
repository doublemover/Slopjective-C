#!/usr/bin/env bash
set -euo pipefail

version="${OBJC3C_CI_LLVM_VERSION:-22.1.6}"
major="${OBJC3C_CI_LLVM_MAJOR:-${version%%.*}}"

if [[ "$major" == "$version" && "$major" == *.* ]]; then
  major="${major%%.*}"
fi

if [[ -z "$major" || "$major" == "$version" && "$version" == *.* ]]; then
  echo "unable to derive LLVM major version from OBJC3C_CI_LLVM_VERSION='$version'" >&2
  exit 1
fi

if [[ ! -r /etc/os-release ]]; then
  echo "unable to identify Linux distribution: /etc/os-release is missing" >&2
  exit 1
fi

# shellcheck disable=SC1091
. /etc/os-release
codename="${VERSION_CODENAME:-}"
if [[ -z "$codename" ]]; then
  echo "unable to identify Ubuntu codename from /etc/os-release" >&2
  exit 1
fi

llvm_root="/usr/lib/llvm-${major}"
repo_suite="llvm-toolchain-${codename}-${major}"
keyring="/usr/share/keyrings/apt.llvm.org.gpg"
source_list="/etc/apt/sources.list.d/objc3c-llvm-${major}.list"

sudo apt-get update
sudo apt-get install -y ca-certificates wget gnupg cmake ninja-build

if [[ ! -f "$keyring" ]]; then
  wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key |
    sudo gpg --dearmor -o "$keyring"
fi

echo "deb [signed-by=${keyring}] http://apt.llvm.org/${codename}/ ${repo_suite} main" |
  sudo tee "$source_list" >/dev/null

sudo apt-get update
sudo apt-get install -y \
  "clang-${major}" \
  "lld-${major}" \
  "llvm-${major}" \
  "llvm-${major}-dev" \
  "llvm-${major}-tools" \
  "libclang-${major}-dev"

required_tools=(
  clang
  clang++
  llc
  llvm-ar
  llvm-ranlib
  llvm-readobj
  llvm-config
)

for tool in "${required_tools[@]}"; do
  if [[ ! -x "${llvm_root}/bin/${tool}" ]]; then
    echo "LLVM ${major} install is missing required tool: ${llvm_root}/bin/${tool}" >&2
    exit 1
  fi
done

if [[ ! -d "${llvm_root}/include" || ! -d "${llvm_root}/lib" ]]; then
  echo "LLVM ${major} install is missing include/lib roots under ${llvm_root}" >&2
  exit 1
fi

{
  echo "LLVM_ROOT=${llvm_root}"
  echo "OBJC3C_LLVM_ROOT=${llvm_root}"
  echo "LLVM_DIR=${llvm_root}/lib/cmake/llvm"
  echo "OBJC3C_NATIVE_EXECUTION_CLANG_PATH=${llvm_root}/bin/clang++"
  echo "OBJC3C_NATIVE_EXECUTION_LLC_PATH=${llvm_root}/bin/llc"
  echo "OBJC3C_NATIVE_EXECUTION_LLVM_READOBJ_PATH=${llvm_root}/bin/llvm-readobj"
  echo "OBJC3C_NATIVE_EXECUTION_LLVM_CONFIG_PATH=${llvm_root}/bin/llvm-config"
} >>"${GITHUB_ENV:-/dev/null}"

if [[ -n "${GITHUB_PATH:-}" ]]; then
  echo "${llvm_root}/bin" >>"$GITHUB_PATH"
fi

export PATH="${llvm_root}/bin:${PATH}"

echo "LLVM_ROOT=${llvm_root}"
"${llvm_root}/bin/clang++" --version | head -n 1
"${llvm_root}/bin/llc" --version | head -n 1
"${llvm_root}/bin/llvm-config" --version
