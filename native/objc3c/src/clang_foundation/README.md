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

`frontend_api.h` exposes versions 2 and 3 through plain C data and owned result
handles. Clang private C++ types remain inside the implementation. Results preserve
diagnostic IDs and spelling/expansion locations. A host failure is distinct from
language diagnostics and Slop typed errors. Version 3 adds locked semantic
interface imports and a compiler/dependency identity record while preserving the
version 2 options and result ownership contract.

Semantic interface emission and import currently support modeled C and
Objective-C public declarations. Emission reparses generated source with the
original language, target, runtime and ARC settings and compares declaration
identities. Reparse and import helpers use isolated physical filesystems, so
their working-directory settings do not change the host process directory.
Imports validate `.objc3i` source and its JSON sidecar against the
locked module name, semantic fingerprint, package identity and consumer target,
runtime, extension ABI, language revision and profile. Canonical modules resolve
through configured search roots in dependency-first order. Clang ASTImporter
inserts validated declarations into the consumer AST; dependency provenance
retains the originating module, and only public re-export edges extend lookup
visibility. Conflicting identities, invalid artifacts, cycles and unsupported
declarations fail explicitly. C++/Objective-C++ semantic interface interchange
remains unsupported. Source, sidecar and optional object output share a rollback
boundary; failed compilation preserves prior products.

The CLI's `--module-search-path` and `--import-module name fingerprint package`
options select locked interfaces eagerly. Adding `--source-imports` makes those
entries a registry for ObjC3 `@import Module.Path;`: an unused registry entry
contributes neither visible declarations nor a semantic dependency. Import
directives resolve and validate selected modules as parsing reaches them;
qualified module names use the same visibility rules. Unregistered imports
continue through Clang's native module loader. Version 3 embedding options expose
the same source-directed mode, and the older version 3 prefix retains eager
imports. Successful imported compilations expose compiler, driver, resource,
patch and module identity through the dependency record; the CLI can write it
with `--emit-dependency-record`.

The tracked
`scripts/objc3c_toolchain_runtime/run_source_import_fixtures_windows.ps1`
runner emits a provider interface and object, archives the original provider
source, then separately compiles, links and runs eager and source-directed
consumers. It checks the dependency record against the provider lock and verifies
that rejected wrong-lock builds preserve existing object and dependency outputs.
At checkpoint 46, both consumer modes compiled, linked and executed successfully,
including qualified imported types at file and block scope; wrong-lock outputs
were preserved.
Package ecosystem install/reuse scripts validate metadata agreement and local
artifact installation/reuse; they do not install Clang. The matching compiler
SDK must be built or staged separately through the pinned LLVM toolchain workflow.

Checkpoint 46 passed all 15 facade CTest tests, including scalar-boundary text
checks and the file CLI's 21 input/error cases. The tests cover compilation and C
embedding, native pointer/ARC and C++ cleanup/EH IR, semantic interfaces and
imports, artifact rollback, carriers, task services and generic collections.
The broader compiler and runtime matrix is recorded in
[`toolchains/llvm/README.md`](../../../../toolchains/llvm/README.md).
Full language cutover and complete extension lowering remain in progress.
Public extension availability follows the separate capability authority; these
checks do not promote public feature gates.

Private `__objc3_optional(T)` and `__objc3_result(T, E)` forms now enter native
canonical type identity, declaration parsing, template substitution, PCH,
AST import, indexing and target layout. Callable async and typed/untyped throws
facts belong to the native function type; native C++ exception specifications
remain separate. The consumer observes effects on functions, callable pointers
and typedefs from that AST. Modeled C/Objective-C interfaces preserve canonical
async and typed/untyped throws declarations, including nested callback types,
through source/sidecar emission and exact declaration-identity roundtrips.
When used, sidecars declare `objc3.native-carrier-types.v1` and
`objc3.canonical-function-effects.v1`; imports reject unknown required semantics.
Executable typed-error handling and async execution remain unsupported.
C++/Objective-C++ semantic interface interchange remains unsupported.

Carrier storage uses an AST-owned hidden record and its actual target layout.
It is a value representation, separate from platform Objective-C object layout.
Private Some/None/Ok/Err construction, explicit tag queries and plain active-arm
copies now lower through native CodeGen. Presence is independent of zero, false
and null payloads; source snapshots preserve self-assignment and overlapping
storage. Stored payload initialization uses its actual hidden record field,
with ordinary C++ source-temporary cleanups owned by the full expression.
Plain carrier values also cross nonvariadic C function boundaries on the tested
Win64 target, using Clang's native ABI classification of the hidden record.
Direct initialized carrier locals with admitted C++ payloads use their native
constructors and selected-arm destructors, including normal and C++ exception
cleanup. For admitted C++ record payloads, `__objc3_has_value` and `__objc3_is_ok`
borrow the tag from an existing carrier glvalue without copying, moving,
constructing or destroying its payload. Queries on those carrier temporaries
remain rejected. Ordinary fixed reference parameters transport the carrier's
address through Clang's native reference ABI; this does not admit by-value carrier
transport or new payload lifetime operations. Direct initialized ARC carrier
locals admit strong, weak and unsafe object fields with selected-arm
initialization and normal/EH cleanup; None and
inactive Result arms do not initialize managed storage. Autoreleasing, Block and
nested managed payload storage remain rejected. ARC carrier copying, assignment,
temporaries and by-value calls remain gated. Other carrier call boundaries,
runtime type encodings, whole enclosing aggregate copies, atomic access and
representation bitcasts still diagnose during CodeGen. Carrier pointer overloads
can have distinct native symbols without qualifying other by-value carrier ABIs.

Private C++ `__objc3_copy` and `__objc3_move` expressions additionally initialize
direct carrier locals from a nonvolatile source glvalue. Sema resolves the actual
payload constructors for each arm; copy requires an lvalue, while move supplies
an xvalue and permits native copy fallback for a const source. CodeGen evaluates
the source once, constructs only the active payload and publishes the destination
tag after successful construction. Moving preserves the source tag and leaves
its active payload in the state produced by the selected native constructor.
Destination cleanup uses the same selected-arm normal/EH lifetime path. These
expressions do not qualify
assignment, arbitrary carrier temporaries, nested carriers or ARC payload
transfer.

Private `__objc3_match` statement and expression forms have native AST nodes,
template revalidation, PCH persistence and AST import. Sema checks pattern types,
case binding scope, guards and coverage. CodeGen evaluates the scrutinee once and
tries cases in source order. Its admitted domains are plain Optional/Result
carriers and native integral, enum and pointer values, with immutable borrowed
`let` bindings to scalar scrutinees or active carrier payloads. Expression forms
require exhaustive coverage and a supported scalar result. Guards and arm bodies
use native temporary/EH cleanup scopes. Managed payload matches, mutable or move
bindings, type tests, string patterns and aggregate expression results remain
unsupported. General payload extraction outside this match slice and async
execution remain in progress; public feature gates report unavailable.

`objc3_text_file_cli` is a native text-service application fixture. It reads an
explicitly selected UTF-8 file, parses `label:signed-decimal` records with
scalar-index search and slicing, checks a signed 64-bit sum and renders `records=N sum=S`
through an owned reusable text builder. Embedded NUL remains text data, and
scalar indexing does not imply normalization or grapheme segmentation. The
external-directory Python harness covers Unicode, CRLF and unterminated final
lines, malformed input, integer range errors, aggregate overflow, the 1 MiB input
bound and file errors. Opaque service handles remain outside the Objective-C object and
value-operations ABIs.

Private typed-error try, raise and ordered do/catch forms now have native AST
nodes, template revalidation, PCH persistence and AST import. Propagation checks
the actual callable's effect; surrounding handlers do not authorize errors
inside separate lambdas or Blocks. Native C++ and Objective-C exceptions retain
their separate controls. The unfinished typed-error ABI and static-analysis
model diagnose explicitly before emission or analysis; this frontend slice
does not qualify executable typed-error handling.
