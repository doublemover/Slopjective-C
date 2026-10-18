#!/bin/sh
# Run in a fresh Debian/Ubuntu host or container with only release files mounted.
set -eu
archive=$1
expected_sha=$2
install_root=$3
printf '%s  %s\n' "$expected_sha" "$archive" | sha256sum -c -
# Provision the small installer prerequisites before inspecting the archive.
apt-get update
DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
  python3 python3-venv zstd g++ libc6-dev libzstd-dev zlib1g-dev libssl-dev passwd util-linux
umask 077
scratch=$(mktemp -d)
trap 'rm -rf -- "$scratch"' EXIT
mkdir "$scratch/installer"
tar --zstd -xf "$archive" -C "$scratch/installer" --strip-components=3 \
  objc3-sdk/share/objc3c/install-linux.sh objc3-sdk/share/objc3c/install-sdk.py
sh "$scratch/installer/install-linux.sh" --archive "$archive" --sha256 "$expected_sha" --root "$install_root"
# Move the entire installation and regenerate activation from the installed
# installer. This also exercises content-addressed prefix reuse after relocation.
mv "$install_root" "$install_root-relocated"
sh "$scratch/installer/install-linux.sh" --archive "$archive" --sha256 "$expected_sha" --root "$install_root-relocated"
. "$install_root-relocated/activate.sh"
python3 "$OBJC3C_SDK_ROOT/share/objc3c/verify-sdk.py" "$OBJC3C_SDK_ROOT"
mkdir "$scratch/client"
cp -R "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/." "$scratch/client/"
cd "$scratch/client"
objc3c main.objc3 -O2 -g -MMD -o application
env -u LD_LIBRARY_PATH ./application
objc3c extra.c multiple.c -o multiple
env -u LD_LIBRARY_PATH ./multiple
env -u LD_LIBRARY_PATH "$OBJC3C_SDK_ROOT/bin/objc3-managed-sample"
for optimization in O0 O2; do
  python3 "$OBJC3C_SDK_ROOT/share/objc3c/examples/managed/build.py" \
    --output-dir "$scratch/managed-$optimization" --optimization "$optimization"
done
objc3c main.objc3 -O0 -g -o application-debug
"$OBJC3C_SDK_ROOT/bin/llvm-dwarfdump" --verify ./application-debug
"$OBJC3C_SDK_ROOT/bin/lldb" --batch -o 'settings set target.disable-aslr false' \
  -o 'breakpoint set --name main' -o run -o 'frame variable cleanup_order' \
  -o 'expression -- cleanup_order + 42' -o continue ./application-debug > "$scratch/debugger.log" 2>&1
cat "$scratch/debugger.log"
python3 - "$scratch/debugger.log" <<'PY'
from pathlib import Path
import sys
text = Path(sys.argv[1]).read_text()
assert all(value in text for value in ('cleanup_order = 0', ' = 42', 'exited with status = 0'))
PY
# CMake is needed only to qualify the embedding consumer, not normal compiler use.
python3 -m venv "$scratch/cmake"
cmake_version=$(python3 -c 'import json,os; print(json.load(open(os.environ["OBJC3C_SDK_ROOT"]+"/objc3-sdk.json"))["host_tools"]["cmake"])')
ninja_version=$(python3 -c 'import json,os; print(json.load(open(os.environ["OBJC3C_SDK_ROOT"]+"/objc3-sdk.json"))["host_tools"]["ninja"])')
"$scratch/cmake/bin/pip" install "cmake==$cmake_version" "ninja==$ninja_version"
export PATH="$scratch/cmake/bin:$PATH"
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="$OBJC3C_SDK_ROOT" \
  -DCMAKE_CXX_COMPILER="$OBJC3C_SDK_ROOT/bin/clang++" -DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld
cmake --build build --parallel 2
./build/embedding "$OBJC3C_SDK_ROOT"
python3 "$OBJC3C_SDK_ROOT/share/objc3c/verify-sdk.py" "$OBJC3C_SDK_ROOT"
useradd --create-home --shell /bin/sh objc3-sdk-user
runuser -u objc3-sdk-user -- sh -s -- "$install_root-relocated" <<'SH'
set -eu
. "$1/activate.sh"
sdk_user_work=$(mktemp -d)
trap 'rm -rf -- "$sdk_user_work"' EXIT
cd "$sdk_user_work"
cp -R "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/." .
objc3c main.objc3 -O0 -g -o application
env -u LD_LIBRARY_PATH ./application
python3 "$OBJC3C_SDK_ROOT/share/objc3c/examples/managed/build.py" --output-dir ./managed
"$OBJC3C_SDK_ROOT/bin/lldb" --batch -o 'settings set target.disable-aslr false' \
  -o 'breakpoint set --name main' -o run -o 'expression -- cleanup_order + 42' -o continue ./application
python3 "$OBJC3C_SDK_ROOT/share/objc3c/verify-sdk.py" "$OBJC3C_SDK_ROOT"
printf 'Unprivileged Linux SDK consumer: PASS\n'
SH
printf 'Fresh Linux archive qualification: PASS\n'
