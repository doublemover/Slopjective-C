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

brew update
brew install cmake ninja

versioned_formula="llvm@${major}"
if brew info "$versioned_formula" >/dev/null 2>&1; then
  brew install "$versioned_formula"
  llvm_root="$(brew --prefix "$versioned_formula")"
else
  brew install llvm
  llvm_root="$(brew --prefix llvm)"
fi

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

installed_version="$("${llvm_root}/bin/llvm-config" --version)"
installed_major="${installed_version%%.*}"
if [[ "$installed_major" != "$major" ]]; then
  echo "expected LLVM major ${major} from OBJC3C_CI_LLVM_VERSION='${version}', but Homebrew resolved LLVM ${installed_version} at ${llvm_root}" >&2
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
