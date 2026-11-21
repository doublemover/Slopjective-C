# Native backend and reader consistency

Source 2.226 / System 2.152 initializes every backend described by the linked
SDK's generated LLVM target headers. Static frontend builds link the configured
code generators and available assembly parsers, with an early configuration error
if a declared generator is missing. Shared-LLVM builds retain their single-library
link contract. Embedded compilation previously initialized only the host target,
even when a newer SDK supplied another backend.

The native backend test emits LLVM IR and ELF objects for each configured Linux
X86/AArch64 generator, checks the target triple and ELF machine, and links and
executes the native result. An unavailable generator must reject code generation
without publishing an object. The current development SDK supplies only X86, so
local execution establishes that path; the replacement SDK's two-backend and
native ARM campaigns remain required. Cross-target object emission is not native
host execution evidence.

Thirteen native integration files now select the executing Linux host rather than
hard-code x86-64. This includes compiler requests, embedding options, dependency
record expectations, object/IR consumers, codec production fixtures and response
files. Pure architecture-specific metadata fixtures retain their explicit target.
The change allows ARM native campaigns to exercise their own installed headers,
backend and runtime instead of accidentally requesting X86 products.

Broader codec replay found a defect introduced with inline anonymous objects:
when a record variable had no definition, a missing anonymous owner compared
equal to that missing definition. Both identity and printing now require an
actual owner. Named record/union declarations retain ordinary native USRs;
permanent union-result helpers retain their body identity through publication.
Dedicated named-variable controls and all 39 production decoder families cover
this, including a new v4 inline anonymous-object family with shared types,
nested storage and structural arguments.

Positive decoder tests now use the production reader catalogue. Twenty-three
manually copied catalogues could become stale as emitted bodies acquired new
contracts, including native object expression metadata in key-path bodies.
Explicit old-reader, missing-capability, forged-owner and erased-marker controls
remain separate and enforced. Selected groups do not qualify the entire frontend
catalogue, profiles or a replacement archive.

Local validation passes thirteen complete native integration executables, all
39 production decoder families, the anonymous-object execution test and 25
selected emitter groups. The compiler/import/C API/code-generator controls and
109 tooling checks pass, with four archive-dependent skips. Full catalogue,
replacement archive and native ARM receipts remain outstanding.
