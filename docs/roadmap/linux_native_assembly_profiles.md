# Native assembly dialect in package profiles

Source 2.183 / System 2.109 binds Clang's effective inline-assembly dialect to
the package language/profile identity. AT&T and Intel templates can spell the
same operands with different instruction syntax and ordering. A package may
therefore not silently inherit a different consumer `-masm` setting.

The profile records `native-inline-asm-dialect:v1` with the completed compiler
invocation's actual dialect. Emission, canonical reparsing, direct import and
on-demand source-directed import preserve that value explicitly. Reparse driver
arguments select the same native dialect on x86. The AST re-emission check uses
the reparsed ASTUnit's effective CodeGen options. Opposite-ownership projections
retain the setting in both their consumer contract and canonical producer proof.
Unknown emitter dialect values reject. There is no process-global policy state.

The new profile suffix is a hard source-free package cutover: older profiles do
not compare equal. It changes neither the LLVM extension ABI layout nor the
interface schema. Embedders using the internal AST emission/import helpers pass
the effective dialect explicitly; their default remains native AT&T. The normal
compiler entry points obtain it from Clang's completed invocation, including
configuration-file and repeated-option resolution.

Eight optimized x86 native configurations cover C++20/C++23, ordinary C++ and
Objective-C++ ARC/MRC, in both dialects. They remove the provider source, replay
exact canonical bytes, compile two consumers and execute real instructions plus
GNU dialect alternatives. Objective-C++ configurations also compile on-demand
`@import` consumers. Opposite-dialect consumers reject during profile admission,
before assembly reaches lowering. The same fixture retains the default native
ARM path without inventing an Intel assembly mode there.

Ownership-projection controls cover C17/C++20/C++23 in both ARC directions and
both dialects. All eight assembly configurations, the four adjacent native
control-flow configurations, all 19 selected publication groups, complete direct
import/source-bridge/ownership-projection/codec tests and 26 tooling checks pass
(one optional skip). Feature authority, undefined-name and whitespace checks pass.
These checks do not qualify a fresh SDK or native ARM archive.
