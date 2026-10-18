#!/usr/bin/env bash
set -euo pipefail

version="${OBJC3C_CI_LLVM_VERSION:-23.1.3}"
if [[ "$version" != 23.1.3 ]]; then
  echo "no verified Linux release checksum configured for LLVM $version" >&2
  exit 1
fi
repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
install_parent="${OBJC3C_CI_LLVM_INSTALL_PARENT:-${RUNNER_TEMP:-$repo_root/tmp/toolchains}}"
llvm_root="$install_parent/llvm-$version"
mkdir -p "$install_parent"
if [[ ! -x "$llvm_root/bin/llvm-config" ]]; then
  archive=$(mktemp "$install_parent/llvm-release.XXXXXX.tar.xz")
  trap 'rm -f -- "$archive"' EXIT
  curl -fsSL --retry 3 --max-filesize 2300000000 \
    "https://github.com/llvm/llvm-project/releases/download/llvmorg-$version/LLVM-$version-Linux-X64.tar.xz" -o "$archive"
  printf '%s  %s\n' ca9fe385a5d9e2d88132dc0e23a00e9f87f38fa75f67207b5e457d208e71d013 "$archive" | sha256sum -c -
  mkdir -p "$llvm_root"
  tar -xJf "$archive" -C "$llvm_root" --strip-components=1
  rm -f "$archive"
fi
[[ "$("$llvm_root/bin/llvm-config" --version)" == "$version" ]] || {
  echo "LLVM install does not match the requested release $version" >&2
  exit 1
}

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
    echo "LLVM ${version} install is missing required tool: ${llvm_root}/bin/${tool}" >&2
    exit 1
  fi
done

if [[ ! -d "${llvm_root}/include" || ! -d "${llvm_root}/lib" ]]; then
  echo "LLVM ${version} install is missing include/lib roots under ${llvm_root}" >&2
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
