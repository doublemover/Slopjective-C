#!/usr/bin/env bash
# Reusable Linux x64 cloud setup. Uses the existing checkout; no worktree needed.
set -euo pipefail

repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
setup_root="${OBJC3C_CLOUD_SETUP_ROOT:-/workspace/.cloud-setup/Slopjective-C}"
cache_limit="${OBJC3C_CLOUD_CCACHE_MAXSIZE:-4G}"
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

download_verified() {
  local url=$1 archive=$2 digest=$3
  curl -fsSL --retry 3 --max-filesize 2300000000 "$url" -o "$archive"
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
  local dest="$setup_root/node-v26.12.0-linux-x64" archive="$scratch/node.tar.xz"
  if [[ ! -x "$dest/bin/node" ]]; then
    download_verified https://nodejs.org/dist/v26.12.0/node-v26.12.0-linux-x64.tar.xz \
      "$archive" fd47865ecd6391ebb431fc815e01b9135bf83ded5cf373247e9331e196b7aded
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


install_llvm() {
  local dest="$setup_root/llvm-23.1.3" archive="$scratch/llvm.tar.xz"
  if [[ -f "$dest/.objc3c-install-incomplete" ]]; then rm -rf -- "$dest"; fi
  if [[ ! -x "$dest/bin/llvm-config" ]]; then
    # SHA-256 from LLVM's upstream release provenance, not a rolling APT snapshot.
    download_verified https://github.com/llvm/llvm-project/releases/download/llvmorg-23.1.3/LLVM-23.1.3-Linux-X64.tar.xz \
      "$archive" ca9fe385a5d9e2d88132dc0e23a00e9f87f38fa75f67207b5e457d208e71d013
    mkdir -p "$dest"
    touch "$dest/.objc3c-install-incomplete"
    tar -xJf "$archive" -C "$dest" --strip-components=1
    rm -f "$dest/.objc3c-install-incomplete"
    rm -f "$archive"
  fi
  [[ $("$dest/bin/llvm-config" --version) == 23.1.3 ]]
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
    download_verified https://github.com/ccache/ccache/releases/download/v4.14.2/ccache-4.14.2-linux-x86_64-glibc.tar.xz \
      "$archive" bd1313f5316f594443dcb4113a4191c666ac6450e0cf016ff8cb8b080f46c160
    curl -fsSL --max-filesize 4096 https://github.com/ccache/ccache/releases/download/v4.14.2/ccache-4.14.2-linux-x86_64-glibc.tar.xz.minisig -o "$archive.minisig"
    "$scratch/minisign/usr/bin/minisign" -P RWQX7yXbBedVfI4PNx6FLdFXu9GHUFsr28s4BVGxm4BeybtnX3P06saF -Vm "$archive"
    tar -xJf "$archive" -C "$setup_root"
    rm -rf "$archive" "$archive.minisig" "$scratch/minisign" "$scratch/minisign.deb"
  fi
  ln -sfn ../ccache-4.14.2-linux-x86_64-glibc/ccache "$setup_root/bin/ccache"
  "$dest/ccache" --version
}

parallel_installs() {
  local job status=0 index
  local -a jobs=(install_python install_node install_llvm install_ccache) pids=()
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
export OBJC3C_BOOTSTRAP_LLVM_ROOT='$setup_root/llvm-23.1.3'
export OBJC3C_LLVM_ROOT="\$OBJC3C_BOOTSTRAP_LLVM_ROOT"
export LLVM_ROOT="\$OBJC3C_LLVM_ROOT"
export LLVM_DIR="\$LLVM_ROOT/lib/cmake/llvm"
export LD_LIBRARY_PATH='$setup_root/llvm-23.1.3/lib'
export PATH='$setup_root/venv315/bin:$setup_root/node-v26.12.0-linux-x64/bin:$setup_root/bin:'"\$LLVM_ROOT/bin:\$PATH"
export OBJC3C_CI_LLVM_VERSION=23.1.3
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
