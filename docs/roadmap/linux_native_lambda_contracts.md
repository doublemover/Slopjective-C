# Native lambda callable contracts

Source 2.166 / System 2.92 preserves native attributes on published lambda call
operators. The maintained Clang statement printer now reuses declaration
attribute normalization, placing standard declaration attributes before the
parameter clause and GNU callable attributes after the exception specification.
Parameter attributes/defaults, template constraints, explicit object parameters,
static call operators, consteval and ObjC3 effects retain their native spelling.
Implicitly inferred constexpr state remains implicit; spelling it explicitly can
make otherwise valid C++20 lambdas ill-formed.

Interface identity binds each lambda's effective function type/calling convention,
constant-evaluation/static state and callable/parameter contracts. Native and
foreign parameter contracts receive the same admission policy as ordinary
functions. Attribute expression dependencies are checked against the package's
public/resolved closure. Unsupported callable attributes are rejected.

The required `objc3.cxx-lambda-expressions.v2` capability is a hard reader cutover.
Older readers cannot accept newly published lambda contracts. Native standard
header imports also require `objc3.cxx-standard-library-headers.v2`, including
lazy defaulted-definition delivery; this applies even to packages without lambdas. Regressions cover
canonical replay, moved source, changed instrumentation policy, missing reader
support, missing sidecar capability, guarded predicate reads, defaults,
constraints, static/nodiscard and consteval lambdas. AST import controls destroy
the original AST before printing and reparsing the imported contracts.

Independently registered synchronization CTests cover the default, assertion,
exceptions/RTTI-disabled and optional libc++ annotation configurations.
A real synchronization provider exercises checked mutex/scoped-guard adapters,
condition-variable predicates, shared locks, jthread joins, stop callbacks and
call_once across independent source-free consumers. It also defends native
header reuse of lazily defined special members: exact declaration reuse must
carry required defaulted definitions through consumer Sema and CodeGen.

Focused source checks use the existing SDK plus rebuilt maintained LLVM objects.
They are not a replacement SDK acceptance result. The optional libc++ annotation
configuration requires the pinned real vendor installation, and final archive,
Linux host, sanitizer and full profile consolidation remain separate gates.

The focused checkpoint passes all 82 maintained AST infrastructure tests, four
C++20/23 MRC/ARC publication/replay profiles, three native source-free lambda
ABI/execution configurations, adjacent Swift/CPU/thread-policy native controls,
and 29 facade/standard-library/CTest tooling tests. Plain C++20 and optimized
ARC C++23 synchronization executions pass. Wider vendor/profile execution and
matching SDK acceptance remain pending; libc++ is not installed locally.
