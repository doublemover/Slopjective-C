#!/usr/bin/env bash
# Reusable Linux x64 cloud setup. Uses the existing checkout; no worktree needed.
set -euo pipefail

repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
setup_root="${OBJC3C_CLOUD_SETUP_ROOT:-/workspace/.cloud-setup/Slopjective-C}"
cache_limit="${OBJC3C_CLOUD_CCACHE_MAXSIZE:-1G}"
[[ "$cache_limit" =~ ^[1-8]G$ ]] || { echo "Compiler cache limit must be 1G through 8G." >&2; exit 1; }
export UV_CACHE_DIR="$setup_root/uv-cache"
export UV_PYTHON_INSTALL_DIR="$setup_root/python"
export PIP_CACHE_DIR="$setup_root/pip-cache"
export XDG_CACHE_HOME="$setup_root/cache"
export XDG_CONFIG_HOME="$setup_root/config"
export XDG_DATA_HOME="$setup_root/data"

# Fail without deleting user work if any budget is exceeded. All sizes are MiB.
check_disk_budget() {
  local free used directory limit
  free=$(df -Pm "$setup_root" | awk 'NR==2 {print $4}')
  if (( free < ${OBJC3C_CLOUD_MIN_FREE_MIB:-4096} )); then
    echo "Cloud setup: less than 4096 MiB free; stopping to preserve disk space." >&2
    return 1
  fi
  for entry in "$repo_root/tmp:8192" "$repo_root/artifacts:2048" "$setup_root/staging:2560" "$setup_root/logs:32"; do
    directory=${entry%:*}; limit=${entry##*:}
    [[ -d "$directory" ]] || continue
    used=$(du -sm "$directory" | cut -f1)
    if (( used > limit )); then
      echo "Cloud setup: $directory exceeds its $limit MiB budget; stopping without deleting work." >&2
      return 1
    fi
  done
}

# A separate process group lets a disk-budget failure terminate all descendants.
run_with_budget() {
  local child status=0
  check_disk_budget || return 1
  setsid "$@" & child=$!
  while kill -0 "$child" 2>/dev/null; do
    if ! check_disk_budget; then
      kill -TERM -- "-$child" 2>/dev/null || true
      wait "$child" || true
      return 1
    fi
    sleep 2
  done
  wait "$child" || status=$?
  check_disk_budget || return 1
  return "$status"
}

download_file() {
  local url=$1 archive=$2 maximum=$3 release_asset=${4:-}
  if curl -fsSL --retry 3 --max-filesize "$maximum" "$url" -o "$archive"; then
    return 0
  fi
  [[ -n "$release_asset" ]] || return 1
  # GitHub's release-page redirect can fail independently of its official
  # release-asset API. Keep the identical immutable asset and verification.
  curl -fsSL --retry 3 --max-filesize "$maximum" \
    -H 'Accept: application/octet-stream' "$release_asset" -o "$archive"
}

download_verified() {
  local url=$1 archive=$2 digest=$3 release_asset=${4:-}
  download_file "$url" "$archive" 2300000000 "$release_asset" || return 1
  printf '%s  %s\n' "$digest" "$archive" | sha256sum -c -
}

host_version() {
  python3 -c 'import json,sys; print(json.load(open(sys.argv[1]))["host_tools"][sys.argv[2]])' \
    "$repo_root/toolchains/llvm/manifest.json" "$1"
}

install_python() {
  if [[ ! -x "$setup_root/venv315/bin/uv" ]]; then
    python3 -m venv "$scratch/bootstrap"
    "$scratch/bootstrap/bin/python" -m pip install "uv==$(host_version uv)"
    "$scratch/bootstrap/bin/uv" python install "$(host_version python)" --no-bin
    "$scratch/bootstrap/bin/uv" venv --python "$(host_version python)" "$setup_root/venv315"
    "$scratch/bootstrap/bin/uv" pip install --python "$setup_root/venv315/bin/python" "uv==$(host_version uv)"
  fi
  UV_PROJECT_ENVIRONMENT="$setup_root/venv315" "$setup_root/venv315/bin/uv" sync \
    --project "$repo_root" --locked --group dev --python "$(host_version python)"
  "$setup_root/venv315/bin/python" --version
  "$setup_root/venv315/bin/python" -m pip check
}

install_node() {
  local dest="$setup_root/node-v26.13.0-linux-x64" archive="$scratch/node.tar.xz"
  if [[ ! -x "$dest/bin/node" ]]; then
    download_verified https://nodejs.org/dist/v26.13.0/node-v26.13.0-linux-x64.tar.xz \
      "$archive" d28a96af6c38596f6d9d97c0ee531c13d238d757742387d1928de822d959e997
    tar -xJf "$archive" -C "$setup_root"
    rm -f "$archive"
  fi
  "$dest/bin/node" --version
  local npm_version
  npm_version=$(host_version npm)
  if [[ "$(PATH="$dest/bin:$PATH" "$dest/bin/npm" --version)" != "$npm_version" ]]; then
    PATH="$dest/bin:$PATH" "$dest/bin/npm" install --global --prefix "$dest" "npm@$npm_version"
  fi
}


install_cmake() {
  local version dest
  version=$(host_version cmake)
  dest="$setup_root/cmake-$version-linux-x86_64"
  bash "$repo_root/scripts/objc3c_toolchain/cmake_linux.sh" "$dest"
  for tool in cmake ctest cpack; do
    ln -sfn "../cmake-$version-linux-x86_64/bin/$tool" "$setup_root/bin/$tool"
  done
}

# Avoid materializing independent compiler products that pruning would remove.
# The verified archive is transient; retain only the LLVM/Clang/lld/LLDB surface.
extract_llvm() {
  local archive=$1 dest=$2 pattern
  local -a excludes=()
  for pattern in include/flang include/mlir include/mlir-c lib/cmake/flang lib/cmake/mlir \
      bin/bbc bin/f18-parse-demo bin/flang bin/flang-23 bin/flang-new bin/merge-fdata bin/perf2bolt \
      'bin/fir-*' 'bin/mlir-*' bin/tblgen-to-irdl 'bin/llvm-bolt*' 'bin/llvm-boltdiff' \
      'lib/libFortran*' 'lib/libflang*' 'lib/libFIR*' 'lib/libMLIR*' 'lib/libLLVMBOLT*'; do
    excludes+=("--exclude=*/$pattern")
  done
  tar -xJf "$archive" -C "$dest" --strip-components=1 "${excludes[@]}"
}


prune_llvm() {
  local dest=$1 path before after restore_nullglob
  local -a files=()
  [[ $dest == /* && -d $dest ]] || { echo 'LLVM pruning requires an absolute installed root.' >&2; return 1; }
  # The Linux foundation is X86 LLVM/Clang/lld/LLDB. Upstream's monolithic
  # archive also carries separate Flang, MLIR/FIR and BOLT products that are
  # neither selected by the manifest nor referenced by LLVM/Clang's exported
  # project targets. Remove those independent products after verified extract.
  before=$(du -sm "$dest" | cut -f1)
  restore_nullglob=$(shopt -p nullglob || true)
  shopt -s nullglob
  files+=("$dest/bin"/{bbc,f18-parse-demo,flang,flang-23,flang-new,merge-fdata,perf2bolt})
  files+=("$dest/bin"/fir-* "$dest/bin"/mlir-* "$dest/bin"/tblgen-to-irdl)
  files+=("$dest/bin"/llvm-bolt "$dest/bin"/llvm-bolt-* "$dest/bin"/llvm-boltdiff)
  files+=("$dest/lib"/libFortran* "$dest/lib"/libflang* "$dest/lib"/libFIR*)
  files+=("$dest/lib"/libMLIR* "$dest/lib"/libLLVMBOLT*)
  if (( ${#files[@]} )); then rm -f -- "${files[@]}"; fi
  for path in include/flang include/mlir include/mlir-c lib/cmake/flang lib/cmake/mlir; do
    rm -rf -- "$dest/$path"
  done
  eval "$restore_nullglob"
  printf '%s\n' 'X86 LLVM/Clang/lld/LLDB development surface; Flang, MLIR/FIR and BOLT removed.' \
    > "$dest/.objc3c-pruned-components"
  after=$(du -sm "$dest" | cut -f1)
  echo "LLVM component pruning reclaimed $((before-after)) MiB; installed size is $after MiB."
}


install_llvm() {
  local dest="$setup_root/llvm-23.1.5" archive="$scratch/llvm.tar.xz"
  if [[ -f "$dest/.objc3c-install-incomplete" ]]; then rm -rf -- "$dest"; fi
  if [[ ! -x "$dest/bin/llvm-config" ]]; then
    # SHA-256 from LLVM's upstream release provenance, not a rolling APT snapshot.
    download_verified https://github.com/llvm/llvm-project/releases/download/llvmorg-23.1.5/LLVM-23.1.5-Linux-X64.tar.xz \
      "$archive" f72edb0b36580ece7f52e23858f00c18fad4e87395ea7ad05d7e55c39f8fc78a \
      https://api.github.com/repos/llvm/llvm-project/releases/assets/705655313
    mkdir -p "$dest"
    touch "$dest/.objc3c-install-incomplete"
    extract_llvm "$archive" "$dest"
    rm -f "$dest/.objc3c-install-incomplete"
    rm -f "$archive"
  fi
  prune_llvm "$dest"
  [[ $("$dest/bin/llvm-config" --version) == 23.1.5 ]]
  for tool in clang clang++ llc llvm-ar llvm-ranlib llvm-readobj llvm-config ld.lld; do
    test -x "$dest/bin/$tool"
  done
  test -f "$dest/lib/libclang.so"
  "$dest/bin/clang++" --version
}

install_ccache() {
  local dest="$setup_root/ccache-4.14.2-linux-x86_64-glibc" archive="$scratch/ccache.tar.xz"
  if [[ ! -x "$dest/ccache" ]]; then
    download_verified https://deb.debian.org/debian/pool/main/m/minisign/minisign_0.12-1_amd64.deb \
      "$scratch/minisign.deb" 4a3f9611a0e363e5c574129dc0195ba951c6be4b8f7ac13d64880000e3b22ebe
    dpkg-deb -x "$scratch/minisign.deb" "$scratch/minisign"
    # The verification tool is dynamically linked. Clean Debian containers do
    # not necessarily have libsodium; keep its pinned dependency private to the
    # verifier instead of relying on the development image or installing it globally.
    download_verified https://deb.debian.org/debian/pool/main/libs/libsodium/libsodium23_1.0.18-1+deb13u1_amd64.deb \
      "$scratch/libsodium.deb" 9ff9d97bb7ce3aee7f6e6ec3a785cdbe237454a7b9fd84728121bc4ef2bebebe
    dpkg-deb -x "$scratch/libsodium.deb" "$scratch/minisign"
    download_verified https://github.com/ccache/ccache/releases/download/v4.14.2/ccache-4.14.2-linux-x86_64-glibc.tar.xz \
      "$archive" bd1313f5316f594443dcb4113a4191c666ac6450e0cf016ff8cb8b080f46c160 \
      https://api.github.com/repos/ccache/ccache/releases/assets/653390076
    download_file https://github.com/ccache/ccache/releases/download/v4.14.2/ccache-4.14.2-linux-x86_64-glibc.tar.xz.minisig \
      "$archive.minisig" 4096 https://api.github.com/repos/ccache/ccache/releases/assets/653390079
    env LD_LIBRARY_PATH="$scratch/minisign/usr/lib/x86_64-linux-gnu" \
      "$scratch/minisign/usr/bin/minisign" -P RWQX7yXbBedVfI4PNx6FLdFXu9GHUFsr28s4BVGxm4BeybtnX3P06saF -Vm "$archive"
    tar -xJf "$archive" -C "$setup_root"
    rm -rf "$archive" "$archive.minisig" "$scratch/minisign" "$scratch/minisign.deb" "$scratch/libsodium.deb"
  fi
  ln -sfn ../ccache-4.14.2-linux-x86_64-glibc/ccache "$setup_root/bin/ccache"
  "$dest/ccache" --version
}

parallel_installs() {
  local job status=0 index
  local -a jobs=(install_python install_node install_llvm install_ccache install_cmake) pids=()
  for job in "${jobs[@]}"; do
    echo "Starting $job"
    ( set -e; "$job" ) >"$setup_root/logs/$job.log" 2>&1 & pids+=("$!")
  done
  for index in "${!pids[@]}"; do
    if wait "${pids[$index]}"; then
      echo "Completed ${jobs[$index]}"
    else
      status=1
      echo "Failed ${jobs[$index]}" >&2
      tail -30 "$setup_root/logs/${jobs[$index]}.log" >&2
    fi
  done
  return "$status"
}

write_activation() {
  cat >"$setup_root/activate.sh" <<EOF
export OBJC3C_BOOTSTRAP_LLVM_ROOT='$setup_root/llvm-23.1.5'
export OBJC3C_LLVM_ROOT="\$OBJC3C_BOOTSTRAP_LLVM_ROOT"
export LLVM_ROOT="\$OBJC3C_LLVM_ROOT"
export LLVM_DIR="\$LLVM_ROOT/lib/cmake/llvm"
export LD_LIBRARY_PATH='$setup_root/llvm-23.1.5/lib'
export PATH='$setup_root/venv315/bin:$setup_root/node-v26.13.0-linux-x64/bin:$setup_root/bin:'"\$LLVM_ROOT/bin:\$PATH"
export OBJC3C_CI_LLVM_VERSION=23.1.5
export OBJC3C_NATIVE_EXECUTION_CLANG_PATH="\$LLVM_ROOT/bin/clang++"
export OBJC3C_NATIVE_EXECUTION_LLC_PATH="\$LLVM_ROOT/bin/llc"
export OBJC3C_NATIVE_EXECUTION_LLVM_READOBJ_PATH="\$LLVM_ROOT/bin/llvm-readobj"
export OBJC3C_NATIVE_EXECUTION_LLVM_CONFIG_PATH="\$LLVM_ROOT/bin/llvm-config"
export XDG_CACHE_HOME='$setup_root/cache'
export XDG_CONFIG_HOME='$setup_root/config'
export XDG_DATA_HOME='$setup_root/data'
export UV_CACHE_DIR='$setup_root/uv-cache'
export UV_PYTHON_INSTALL_DIR='$setup_root/python'
export PIP_CACHE_DIR='$setup_root/pip-cache'
export npm_config_cache='$setup_root/npm-cache'
export NODE_COMPILE_CACHE='$setup_root/cache/node-compile'
export CCACHE_DIR='$setup_root/cache/ccache'
export CCACHE_BASEDIR='$repo_root'
export CCACHE_MAXSIZE='$cache_limit'
export CCACHE_COMPILERCHECK=content
export VIRTUAL_ENV='$setup_root/venv315'
export UV_PROJECT_ENVIRONMENT='$setup_root/venv315'
export CMAKE_CXX_COMPILER_LAUNCHER='$setup_root/bin/ccache'
export LDFLAGS=-fuse-ld=lld
export CMAKE_BUILD_PARALLEL_LEVEL=4
export OBJC3C_NATIVE_BUILD_PARALLELISM=4
EOF
}

main() {
  [[ $(uname -sm) == 'Linux x86_64' ]] || { echo 'Linux x64 is required.' >&2; return 1; }
  mkdir -p "$setup_root"/{bin,logs,staging}
  if [[ ${1:-} == --dependencies ]]; then
    scratch=$(mktemp -d "$setup_root/staging/install.XXXXXX")
    touch "$scratch/.objc3c-setup-owned"
    trap 'rm -rf -- "$scratch"' EXIT
    parallel_installs
    return
  fi
  exec 9>"$setup_root/setup.lock"
  flock 9
  # Only installer-owned staging directories are disposable, never repo outputs.
  for directory in "$setup_root"/staging/install.*; do
    [[ -f "$directory/.objc3c-setup-owned" ]] && rm -rf -- "$directory"
  done
  run_with_budget bash "${BASH_SOURCE[0]}" --dependencies
  write_activation
  source "$setup_root/activate.sh"
  cd "$repo_root"
  run_with_budget npm ci
  ccache --max-size="$cache_limit" --cleanup
  # Cached wheels are expendable; keep installed tools and useful build objects.
  uv cache prune --ci
  python -m pip cache purge
  if [[ -d "$setup_root/npm-cache" ]] && (( $(du -sm "$setup_root/npm-cache" | cut -f1) > 512 )); then
    npm cache clean --force
  fi
  check_disk_budget
  echo "Setup complete. Source $setup_root/activate.sh in each new shell."
}

if [[ ${BASH_SOURCE[0]} == "$0" ]]; then
  main "$@"
fi
