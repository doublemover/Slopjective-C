# Native host CI and installed helper contracts

The checks at Source 2.187 exposed three stale CI assumptions after the native
Linux AArch64 cutover. The release policy searched for the retired architecture-
independent archive name; installed-helper tests copied an incomplete dependency
list; runtime/generator fixtures omitted required native architecture context.

Release validation now checks the same architecture-bound archive names used by
the producer and fresh-host installer, with the selected host forwarded to both
the native build and implementation-analysis owner. Mutation controls reject
foreign or unbound archives and host substitutions. All 128 workflow-policy
checks pass, including the repaired mandatory archive-publication controls.

SDK assembly and isolated helper checks share one installation routine and
inventory, including the auxiliary source validators. Every shipped helper is
imported without a source checkout, both with and without Python's optional
compression module. The semantic-oracle input inventory also binds its native
host helper. Runtime fixtures use the actual native host's ELF machine identity
and complete bootstrap tool inventory; generator checks execute the production
CMake host-selection include. Separate controls reject foreign ELF images and
missing/foreign runtime profile identities. These are configuration and import
controls, not replacement compiler/runtime qualification.

The focused affected tooling passes 83 tests with nine optional native-product
skips. This repair does not promote SDK 2.62 / ABI 21 or replace the remaining
current-source SDK/native-host acceptance gates.

The complete local native-tooling CI command also passes: 1,689 tests,
155 optional skips and 33 subtests. Remote CI must run again on the committed
repair; local unit/configuration checks are not an SDK acceptance receipt.

## Subsequent CI audit

The Source 2.238 automatic documentation, native-tooling and workflow-policy jobs
all pass. The earlier three failing jobs at `d51e62900349` predate the repairs
above; their missing-helper, native loader fixture, generator, release-policy
and formatting failures are covered by the maintained controls. A renewed local
check of the affected helpers, loader/runtime fixtures and native producer/reader
boundary passes 208 tests, with nine optional native-product skips.

The separate Source 2.153 extended campaign failed in its terminal upstream
receipt merge after its producers and consumers passed. The
[terminal-state repair](linux_upstream_evidence_terminal_state.md) is committed;
that historical run does not qualify the current compiler.

The retained coverage diagnostic now returns the aggregate command's actual exit
status after printing its annotation. Previously its final `echo` made a failed
aggregate appear successful. Four executable shell controls preserve success,
ordinary failures and timeout exit codes; workflow-policy mutations reject a
missing or overwritten exit status. No archive is promoted by a diagnostic run.

The shared analysis foundation now transports its configured
`bin/llvm-tblgen` with the staged compiler and Ninja dependency inputs. The
fork-matched frontend invokes that executable to generate the scanner's option
table. The producer rejects a missing, non-executable or wrong-architecture
tool, and the consumer validates the restored member before frontend
configuration. The earlier AArch64 partitions all stopped at this boundary.

The configured compilation database can name working directories containing
only unselected objects. The shared analysis archive now includes those
directory entries without recursing into their object files and checks their
restoration before running analysis. This preserves relative source-path
resolution and the exact dependency inventory on both Linux architectures.

The Source 2.280 x86-64 analysis restored the compiler and ran frontend and
runtime variants before reaching the maintained-Clang compilation database.
The database still listed generated PCH stub sources for unrelated LLVM
targets. The analyser now checks membership before requiring the selected
source to exist, including when a module map uses a relative input. A real
CMake/Ninja archive round trip verifies exact selected commands after omitting
an unrelated PCH source. The retained hosted graph has all 180 maintained
Clang object inputs in the producer archive.

Interface admission reparses providers using the consumer's effective target
CPU, ABI and feature options. The AArch64 SDK driver config enables FMV via
compiler-rt. Clang Tooling reparses now preserve that setting through the same
projection used for interface emission. The regression reproduces the profile
mismatch with libgcc and verifies admission with the projected target. Test
providers that construct their own Tooling ASTs use the SDK's native target
settings, and target-attribute controls use architecture-specific spellings.

The combined emitter suite now checks that an admitted `explicit(true)`
constructor keeps its spelling, metadata and conversion behavior through
replay. The previous expectation still treated the constructor as an
unsupported declaration even after explicit specifiers gained native support.

The x86-64 SDK producer retained five mutation suites that changed canonical
interface bodies without regenerating their content-bound include guards. The
repaired fixtures use the production guard wrapper and reach semantic identity
validation after source admission. Dependent array bounds now identify non-type
template parameters by depth and index, including canonical types that reuse a
parameter node from another declaration. Source-free storage witnesses exercise
the repaired contract under C++20 and C++23.

The deduced group witness uses configured include directories independent of its
test executable's `__FILE__` and working directory. Four Block binding CTest
entries share one executable and give each ARC/MRC and C++20/23 combination its
own timeout. The source coverage inventory requires all four. Provenance tests
report the failing source fragment and distinguish unknown capture lifetime from
a traced loan. The runtime workload and schedule oracles expect cancellation
after draining an explicitly cancelled group and still require all terminal
children to be consumed.

The generated worker schedule witness now joins the group and actor before
recording a drained epoch. An executor barrier alone can finish before a
later actor mailbox turn destroys its context. Native verification matched
all 64 maintained history seeds on the manual backend and 20 worker executions
per seed, for 1,344 matching traces.

The ordinary, sanitizer and analysis compilers shared a stale rejection for a
checked method returning a plain C record. Its replacement checks the register
ABI and hidden error branch on x86-64 and AArch64 in ARC and MRC. A flexible-array
record still receives the unsupported-ABI diagnostic.

Release qualification starts the fresh Debian and Ubuntu installs as soon as
the archive producer succeeds. They run alongside the remaining native and
analysis owners. The terminal release job requires the native gate, both fresh
installs and implementation analysis to pass on the selected host.

On a fresh runner the runtime instrumentation owner acquires the pinned GNUstep
and robin-map sources using the host Git configuration. The instrumented build
revalidates their commits and checked patches with the scrubbed environment and
uses the local source cache. The same lock covers acquisition and compilation.

Failed foundation commands now print a bounded recent log excerpt in the job
output. The complete logs and their hashes remain in the diagnostic artifacts.

The SDK frontend CTest plan records the executable's command argument index
from each CMake registration. Direct tests start with their executable. The
meta, diagnostic and text-file CLI wrappers pass it as an argument. Serial
qualification checks the actual CTest command, executes all aliases of the
same owner and retains the product tools for installation.

The public C embedding implementation compiles with the native source include
root and the standard C++ library. Its internal async export state uses
`std::span` for entry selections, so dependency scanning requires no LLVM
headers through the embedding API.
