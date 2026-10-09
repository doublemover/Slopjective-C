# Clang foundation migration frontend

This explicit build path uses the repository's verified Clang 22.1.6 SDK for
parsing, Sema, AST and native CodeGen. The `objc3c-clang` CLI and versioned C
embedding API call the same in-process compile session. Selecting this build
path never falls back to the legacy parser.

From the admitted Windows x64 developer environment, configure with:

```powershell
cmake -S native/objc3c -B out/toolchains/objc3-facade/build -G Ninja `
  -DOBJC3C_ENABLE_CLANG_FOUNDATION=ON `
  -DOBJC3C_CLANG_FOUNDATION_ROOT=C:/Users/sneak/Development/Slopjective-C/out/toolchains/objc3-development/build `
  -DCMAKE_CXX_COMPILER=cl.exe -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL `
  -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build out/toolchains/objc3-facade/build --parallel 8
ctest --test-dir out/toolchains/objc3-facade/build --no-tests=error --output-on-failure
```

Replace the absolute SDK path with the checkout's verified build-tree or staged
SDK. Its LLVM/Clang configurations, generated identity header, driver and
resource headers must belong to the same SDK. Configuration verifies the source
seal and driver patch digest; compile sessions also verify the linked library
identity.

The CLI recognizes ordinary `.c`, `.cc`/`.cpp`/`.cxx`, `.m` and `.mm` input.
`.objc3` explicitly selects Objective-C extension mode. C18 and C++20 are
defaults; explicit Clang dialect arguments after `--` override them. ARC is an
explicit option. Native target/runtime arguments and relative response files
use the invocation's working directory.

```powershell
out/toolchains/objc3-facade/build/src/clang_foundation/objc3c-clang.exe source.mm `
  --arc --emit-object out/source.obj -- --target=x86_64-pc-windows-msvc
```

`frontend_api.h` exposes version 2 through plain C data and owned result handles.
Clang private C++ types remain inside the implementation. Results preserve
diagnostic IDs and spelling/expansion locations. A host failure is distinct from
language diagnostics and Slop typed errors.

Semantic interface emission currently supports modeled C and Objective-C public
declarations. It reparses generated source with the original language, target,
runtime and ARC settings and compares declaration identities. Unsupported C++,
attributes and dependency imports fail explicitly. Source, sidecar and optional
object output share a rollback boundary; failed compilation preserves prior
products.

The eight native tests cover compilation and C embedding, native pointer/ARC
and C++ cleanup/EH IR, interfaces, artifact rollback, carriers, task services and
generic collections. These checks establish their tested behavior; full language
cutover, complete semantic imports and extension lowering remain in progress.
Public extension availability follows the separate capability authority.
