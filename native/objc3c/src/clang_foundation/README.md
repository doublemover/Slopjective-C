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

Dependency-record publication uses that same rollback boundary. An inaccessible
record destination or a request with no imported dependencies preserves existing
object, interface and sidecar outputs. A later product-publication failure
restores the previous record; failed rollback retains its backup for recovery.
Record destinations must be regular files with accessible physical parents and
must differ from compilation inputs, imported interfaces and other product paths.
The C embedding API continues to return dependency records in memory without
choosing a filesystem destination.

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

The `validate-native-foundation` workflow selects actual native checks from a
20-node inventory: 14 carrier suite/driver nodes, two typed-error runtime nodes,
a compiled-package provider and two consumers, and one editor qualification
node. The package consumers install
only the emitted object/interface/sidecar in fresh directories outside the
checkout, then compile, link and execute against that installed provider.

```powershell
npm run objc3c -- validate-native-foundation --list
npm run objc3c -- validate-native-foundation --node native-package:consumer-source --run-id package-source
npm run objc3c -- validate-native-foundation --family editor --run-id editor-private
npm run objc3c -- validate-native-foundation --family typed-errors --run-id typed-errors-private
```

`--family`, `--node` and `--driver` accept exact, repeatable selectors; selecting
a consumer includes its provider dependency. `--list` shows the inventory and
selected dependency closure without requiring built tools. `--reuse-report`
checks a prior complete report, toolchain/input identities and artifact hashes
before reusing passing nodes. Reports and durable products are written under
`out/native-validation/<run-id>` by default. See the pinned LLVM README for the
full selector inventory and reuse example. This workflow does not build/install
the compiler or promote public capabilities. At checkpoint 47, all 18 nodes
passed with 68 runtime executions and unchanged source/toolchain snapshots.
Both compiled-package consumers ran outside the checkout without provider
source, using the sealed native linker and exact emitted module identities.

At checkpoint 49, all 20 nodes passed with 80 runtime executions, including 12
typed-error witnesses across both drivers at O0/O2. The matching SDK passed 126
extension tests with three platform skips and 44 AST tests; its rebuilt facade
passed all 15 CTest tests. Source and toolchain identities matched before and
after the native batch. Public extension gates remain unavailable.

Checkpoint 50 qualified owned error transport through callee cleanup: the SDK
passed 127 extension tests (three platform skips), 44 AST tests and all 15 facade
CTest tests. All 20 native nodes and 80 executions passed with matching source
and toolchain identities. The typed-error witnesses additionally check nested
pools, temporary cleanup, native exception interruption and nullable discard.

Matching editor qualification passed on 2026-10-13 at 03:36:46 UTC. Identical C17
`.m` and C++20 `.mm` fixtures passed exact formatting, idempotence, native syntax
and `clangd --check` with zero errors. In each mode, both the ordinary parameter
and the match's borrowed `const int` payload passed live hover, definition at the
exact declaration location and completion checks; live diagnostics contained no
errors. This establishes
the tested private scalar carrier/match editor scope without promoting public
capabilities.

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
Private synchronous ARC and MRC free functions returning scalar/void values can lower
Error-object throws through a trailing autoreleasing error-output pointer.
Private Error-object do/catch also runs in nonthrowing functions in both modes.
Async execution remains unsupported.
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
their separate controls. The private Error-object ABI admits synchronous ARC/MRC
free-function calls, ordinary function pointers/references, static C++ members
and Blocks returning scalar/void values, with a nullable trailing
`id<Error> __autoreleasing *` slot. Success clears the slot to nil; failure
stores a non-null error and returns a zero/void placeholder. The compiler retains
the pending error before expression cleanup and owns it through function cleanup,
including callee autorelease pools. A native exception releases that pending
ownership without publishing the typed error. The return block transfers the
error to the caller's pool, or releases it immediately for a null output pointer.
Propagating try forwards that error, forced try traps on failure, and
try-optional builds the native scalar/void Optional representation. Exact call
contexts prevent an outer try from authorizing nested bare throwing calls.
Static typed nil is rejected, dynamic nil traps, and scalar zero remains valid
in the semantic model. Scalar/value error transport,
aggregate or managed success returns, instance methods, variadic calls
and async execution still diagnose before emission. The runtime
witness covers nested pools, a pool-owning C++ temporary, expression and function
cleanup interrupted by native exceptions, and nullable output discard. Ordinary
MRC callers of the physical C output ABI are covered by the runtime witness.
MRC throw evaluates the operand normally and retains a separate transport
reference. Source-owned references remain the program's responsibility, including
an owning factory result. Mixed ARC/MRC translation units exercise calls in both
directions, propagation through callee-owned pools, manual releases from C++
destructors and defer, and nullable-output discard without a caller pool.

Effect-qualified callbacks retain the trailing error-output ABI through local,
global, array and record storage, higher-order calls and C++ templates. The
runtime witness covers argument/callee evaluation once, ARC consumed parameters,
C++ by-value parameter cleanup and native exceptions interrupting that cleanup.
Pending-error unwind cleanup is registered before parameter cleanup so that it
remains active if a parameter destructor throws. On the qualified Win64 C ABI,
implicit nonthrowing-to-throwing conversion preserves the thin function pointer;
the plain callee ignores the extra trailing output and the caller observes its
initialized nil slot. This includes nonthrowing static C++ functions. Installed
C/Objective-C interface consumers execute imported callbacks and higher-order
error calls without provider source; C++ interface interchange remains closed.

Synchronous throwing Blocks retain Clang's native context prefix, capture
layout, byref forwarding and copy/dispose helpers. Their invoke function receives
the trailing error output after source arguments; the Block signature descriptor
includes that physical slot. The hidden output receives ordinary parameter
storage without replacing the Block context. ARC/MRC witnesses check escaping
captures, C++ capture destruction, captured objects, consumed object arguments,
local handlers and C++ parameter cleanup. An Objective-C 2 client directly calls
the physical Block signature, including nullable output discard. Nonthrowing
Block widening uses the qualified Win64 convention. PCH and installed interfaces
preserve the effect-qualified Block type. Dynamic-nil and forced-error Block
witnesses require the exact illegal-instruction exit in both ownership modes.

Ordered Error-object do/catch owns the pending error through protected-body
cleanup and selects the first matching class, protocol or bare catch. Unmatched
errors move to an enclosing handler or the callable's error output; errors raised
inside a catch body reach the enclosing context rather than sibling clauses.
ARC catch bindings use native lifetime inference and support explicit weak
storage. A strong binding can transfer ownership to an outer local. MRC bindings
borrow from the handler; retaining an escaping binding is explicit. The runtime
witness covers subclass and protocol matching, nested propagation, explicit
rethrow, strong/weak binding lifetime, catch return/break/continue/goto, deferred
cleanup and native C++ exceptions interrupting either the body or a catch.
Jumps into protected bodies and catches are rejected. An error cannot escape a
defer body, including after template substitution; a handler wholly inside the
defer remains valid. ARC catch bindings retain their types and lowering through
PCH and template instantiation.

Static analysis of the extension remains unqualified. Public typed-error
capabilities stay disabled.
