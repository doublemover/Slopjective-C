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
"$OBJC3C_SDK_ROOT/bin/llvm-dwarfdump" --verify ./build/embedding
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
python3 "$OBJC3C_SDK_ROOT/share/objc3c/examples/managed/qualify.py" --output-dir ./module-evidence
python3 "$OBJC3C_SDK_ROOT/share/objc3c/source-build/qualify.py" --output-dir ./package-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/source-build/qualify_binary_packages.py" --output-dir ./binary-package-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/source-build/qualify_ownership_projections.py" --output-dir ./ownership-projection-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/source-build/qualify_public_surfaces.py" --output-dir ./public-surface-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/source-build/qualify_api_availability.py" --output-dir ./api-availability-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/source-build/qualify_module_spi.py" --output-dir ./module-spi-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/source-build/qualify_module_names.py" --output-dir ./module-names-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/source-build/qualify_structured_runtime.py" --output-dir ./structured-runtime-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/source-build/qualify_task_coroutines.py" --output-dir ./task-coroutine-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/source-build/qualify_native_async.py" --output-dir ./native-async-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/source-build/qualify_native_async_packages.py" --output-dir ./native-async-package-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-carrier-calls.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./carrier-call-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-runtime-carrier-calls.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./runtime-carrier-call-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-managed-throws.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./managed-throws-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-direct-carrier-calls.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./direct-carrier-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-nested-carrier-calls.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./nested-carrier-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-captured-carriers.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./captured-carrier-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-array-carriers.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./array-carrier-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-record-carriers.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./record-carrier-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-heap-carriers.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./heap-carrier-evidence
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-cpp-interfaces.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./cxx-interface-evidence
for optimization in O0 O2; do
  objc3c carrier-transfer.mm -std=c++20 -fexceptions -fobjc-arc-exceptions \
    -"$optimization" -g -o carrier-transfer
  env -u LD_LIBRARY_PATH ./carrier-transfer
  for language in objective-c objective-c++; do
    if [ "$language" = objective-c++ ]; then standard=c++20; else standard=c17; fi
    objc3c -x "$language" carrier-assignment.m -std="$standard" \
      -fexceptions -fobjc-arc-exceptions -"$optimization" -g -o carrier-assignment
    env -u LD_LIBRARY_PATH ./carrier-assignment
    "$OBJC3C_SDK_ROOT/bin/llvm-dwarfdump" --verify ./carrier-assignment
    printf 'Installed ARC assignment (%s, %s): PASS\n' "$language" "$optimization"
  done
done
"$OBJC3C_SDK_ROOT/bin/llvm-dwarfdump" --verify ./carrier-transfer
"$OBJC3C_SDK_ROOT/bin/lldb" --batch -o 'settings set target.disable-aslr false' \
  -o 'breakpoint set --name main' -o run -o 'expression -- cleanup_order + 42' -o continue ./application
python3 "$OBJC3C_SDK_ROOT/share/objc3c/verify-sdk.py" "$OBJC3C_SDK_ROOT"
printf 'Unprivileged Linux SDK consumer: PASS\n'
SH
printf 'Fresh Linux archive qualification: PASS\n'
