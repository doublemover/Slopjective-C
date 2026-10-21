# Qualified LLVM foundation

The compiler uses one patched LLVM/Clang SDK. The authoritative upstream tag,
commit, exact development tool versions, build configuration and canonical
extension patch series are recorded in [manifest.json](manifest.json). The
current foundation is LLVM 23.1.3. No system Clang substitution is admitted.

## Build the Linux SDK

```sh
bash scripts/setup_cloud_environment.sh
source /workspace/.cloud-setup/Slopjective-C/activate.sh
python -m scripts.objc3c_toolchain check-host-tools --runtime
python -m scripts.objc3c_toolchain build-sdk --fetch
source out/toolchains/linux-sdk/activate.sh
objc3c --version
```

`build-sdk` coordinates source sealing, LLVM compilation and tests, the pinned
libobjc2/robin-map runtime, frontend compilation and tests, installed consumers,
native and editor qualification, archive sealing and installation. It keeps
incremental Ninja build directories, uses persistent ccache, limits compilation
to host CPU/memory capacity and serializes large LLVM link steps.

The bootstrap compiler is selected by `OBJC3C_BOOTSTRAP_LLVM_ROOT`. Product
activation sets `OBJC3C_SDK_ROOT`, `OBJC3C_CLANG_FOUNDATION_ROOT` and consumer
LLVM/Clang paths separately and isolates SDK library lookup. Installing a
product SDK does not change the compiler used to build that product.

A previously qualified sealed SDK can supply the LLVM foundation:

```sh
python -m scripts.objc3c_toolchain build-sdk --foundation-sdk /path/to/sealed/objc3-sdk
```

Every file and symbolic link is verified before reuse, and its source/build
identity must match the locked fork. The current frontend and runtime are still
built and qualified. Subsequent runs automatically consider the active installed
SDK. Reuse keys include source, compiler/tool identities, configuration,
foundation content, runtime/service inputs and qualification fixtures. Complete
release receipts and per-command logs/timings live in
`out/toolchains/linux-sdk-builds/<input identity>/`. Failed candidates are
preserved for inspection and are never activated.

Release installations use `out/toolchains/linux-sdk/sdks/<full seal>` and an
atomic `current` symbolic link. The mutable foundation stage and build candidate
are development outputs; only a verified archive enters the installation store.
The archive includes Clang, LLD, clangd, clang-format, clang-scan-deps, LLDB without a Python/Lua
ABI dependency, matching libraries/resource headers, libobjc2, the C embedding
API, CMake targets, source examples, licenses and standalone verification and
installation tools.

## Change the fork

The six canonical patches define the extension source. A development checkout
lives under `out/toolchains/objc3-development/src`; source changes must be
exported and sealed before any build is admitted:

```sh
python -m scripts.objc3c_toolchain.generate_features
cp toolchains/llvm/generated/ObjC3Features.def \
  out/toolchains/objc3-development/src/clang/include/clang/Basic/ObjC3Features.def
python -m scripts.objc3c_toolchain.export_development_patches
python -m scripts.objc3c_toolchain seal-development
python -m scripts.objc3c_toolchain verify-development
python -m scripts.objc3c_toolchain build --host linux-x86_64 --stage
```

The generated `ObjC3Features.def` in the LLVM source must match the repository's
feature authority before exporting patches. Extension feature states come from
[the capability schema](../../schemas/objc3-language-capabilities.json), never
from a compiler version or a fixture's existence. Compiler source seals bind the
upstream commit, canonical patch hashes and configuration; a changed source tree
or stale frontend content stamp is rejected.

`build-plan <configure|build|test|stage>` prints identified commands without
executing them. `prepare --fetch` materializes the pinned fork; an upstream-only
baseline requires the explicit `--pristine-baseline` flag and has a distinct
identity. Test output and source/header inventories are checked after execution.

## Qualify the installed product

```sh
source out/toolchains/linux-sdk/activate.sh
python -m scripts.objc3c_native_validation
python "$OBJC3C_SDK_ROOT/share/objc3c/verify-sdk.py" "$OBJC3C_SDK_ROOT"
```

The Linux native plan includes O0/O2 execution, ABI and managed lifetime checks,
expected traps, eager/source-directed semantic interface consumers and editor
qualification. Validation snapshots compiler, runtime and system header/library
contents before and after each batch. Reused evidence must retain matching
source, toolchain, node and artifact identities.

The managed carrier transfer milestone uses Clang's native ARC member
initialization for explicit Objective-C++ local copy/move construction. Its IR
and PCH checks cover strong objects, weak storage, unsafe pointers, Blocks,
Result arms and template instantiation. O0/O2 runtime witnesses add source
evaluation, nil/none, weak zeroing, typed-error optionalization and exception
cleanup. Implicit direct ARC carrier copying and assignment are also qualified
in Objective-C and Objective-C++: the selected RHS is preserved before LHS
evaluation and old-arm destruction, including self-aliasing, all Result arm
transitions, weak zeroing, Blocks, C++ sequencing and exception cleanup.
Volatile carriers, by-value returns and nested managed payloads retain their
diagnostic boundaries. Patch export preserves canonical section ordering and
metadata across untracked and intent-to-add source files, so index bookkeeping
does not change unrelated patch identities.
The [assignment acceptance receipt](../../docs/support/linux_arc_assignment_acceptance.json)
binds the compiler and native execution results to the exact SDK archive,
installed editor/debugger checks and ordinary-user Debian/Ubuntu acceptance.

[`scripts/qualify_linux_archive.sh`](../../scripts/qualify_linux_archive.sh)
qualifies the actual archive on fresh Debian/Ubuntu hosts: install and relocation,
public compilation/linking, a source-built ARC/defer application, CMake embedding
and native runtime services, LLDB and standalone seal verification. The Linux
release workflow runs this with no checkout mounted in consumer containers.
