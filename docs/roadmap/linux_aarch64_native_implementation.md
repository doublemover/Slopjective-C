# Native Linux AArch64 implementation

Source 2.175 / System 2.101 implements the second native Linux host through the
compiler, runtime, package builder and delivery workflows. Extension ABI 24 is
unchanged for existing x86-64 interfaces. AArch64 artifacts carry their own target,
ELF machine, native build environment and archive seal. Native ARM execution and
fresh archive acceptance remain required; this implementation checkpoint does
not promote an ARM SDK or replace the accepted SDK 2.62 archive.

## Compiler and interface behavior

The maintained LLVM stack builds both X86 and AArch64 backends. Public defer,
Optional, Result and typed-throws predicates include the canonical Linux AArch64
GNUstep 2.2 compatibility profile. ARC/C++ carrier calls, runtime encodings, native
async/group admission, aligned heap Blocks and aligned byref forwarding include
AArch64. Runtime header admission checks the same concrete LP64 structures and
callable fields on both hosts.

Owned carrier parameters use pointers to initialized caller-owned storage, never
`byval` copies; owned results use AAPCS64 indirect results through x8. Ordinary
plain carriers retain native aggregate coercion. Keeping the owned representation
indirect protects weak-cell addresses, inactive union bytes and exact cleanup
when more than eight integer argument slots are needed.

AArch64 `target_version` variants retain distinct declaration identities and one
module-owned assembly family. Import keeps their exact bodies and multiversion
state. Canonical reparsing preserves effective FMV context; native declaration
and lambda printing retain vector/SVE calling conventions even when those exist
only in the function type. `native-emission-policies.v3` requires new readers and
rejects missing, forged, malformed or namespace-owned CPU variant claims.

## Host tools, runtime and SDK assembly

`toolchains/linux-hosts.json` pins architecture-specific LLVM 23.1.5, Node
26.13.0, CMake 4.4.5, ccache 4.14.2 and the ccache signature-verifier dependencies.
Provisioning checks native ELF architecture before executing retained or newly
installed tools. Compiler/version queries select the bootstrap's own libraries.
Binary/Python/package caches are architecture-separated; immutable LLVM,
libobjc2 and robin-map source objects remain shared. Downloads and extraction
staging remain temporary, and concurrency respects cgroup CPU/memory limits.

Both Linux profiles select the same pinned, patched libobjc2 2.3 / GNUstep 2.2
runtime. Build, runtime, facade, sanitizer, coverage, semantic/fuzz and debugger
consumers select native target paths and loader identities. Native C++ fixtures
use the executing host while deliberate header-free x86 contract controls retain
their explicit cross target.

The SDK stages content-bound native compiler-rt builtins, sanitizers, profiling,
libFuzzer and startup support from the exact pinned bootstrap, including shared
runtimes and symbol lists. Every archive member must be a native ELF64
little-endian relocatable object; thin, foreign, truncated and non-object archives
are rejected. Incomplete sanitizer inventories and foreign shared/startup objects
are rejected before assembly. These are SDK products rather than reusable
compiler-foundation inputs, so stale runtime libraries cannot survive reuse. ARM SDK
configuration selects compiler-rt by default so native FMV resolver support is
available. Header bundles bind their target and reject cross-host installation.
The self-contained archive installer rejects a foreign target or ELF member
before publishing its activation pointer.

Swift 6.4.2 has independently pinned Debian 13 archives for both architectures.
The ARM archive's exact hash, detached release signature and retained-file
inventory were verified; the transient download/extraction was removed.

## CI and evidence

Native assurance, implementation analysis, dependency-cache qualification,
LLVM/debugger assurance and release qualification accept `linux-aarch64` and use
`ubuntu-26.04-arm`. Producer transports, partition artifacts, SDK archives and
concurrency groups include host identity. The same complete/disjoint terminal
gates apply, followed by native Debian 13 and Ubuntu 26.04 installation consumers.
Wrong requested/actual host combinations fail before compiler provisioning.

The added AArch64 ABI/publication control runs on either native host: it compiles
C17/C++20/23 ARC/MRC ARM IR, checks plain and owned ABI shapes, weak argument spill
snapshots, aligned Block/byref metadata and source-free target-version/vector PCS
replay. Its ARM execution counterparts use the existing full native suite on the
ARM runner. Source-coverage ownership now remaps earlier patch ranges through
later insertions/deletions, preserving final-stack coordinates and denominator
membership.

Focused implementation checks passed: 91 LLVM AST infrastructure controls, six
AArch64 C/C++ ABI configurations plus source-free dispatch/PCS and malformed
metadata controls, four native storage lifetime configurations, the production
codec test, and 12 native emission-policy configurations. The other three
emission-policy configurations reached sanitizer linking but the retained local
development SDK has no ASan libraries; they require the newly assembled SDK and
are not counted as passing. The new runtime-delivery controls cover that missing
product and bind version, target, archive contents and materialization.

The main related tooling run passed 486 tests (17 unavailable optional-tool
controls skipped); workflow actionlint, Python undefined-name checks, shell syntax,
generated feature authority and byte-exact maintained patch replay also passed.
Pinned Debian 13 and Ubuntu 26.04 qualification image indexes include both amd64
and arm64. None of these focused checks substitutes for native ARM execution or
current-source SDK acceptance.
