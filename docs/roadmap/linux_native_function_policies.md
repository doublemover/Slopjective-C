# Native function definition and optimizer policies

Source 2.186 / System 2.112 extends source-free native publication with eleven
function policies: selected/all builtin suppression, `minsize`, convergence and
its opt-out, trusted-call domains and domain leaves, split-stack suppression,
speculative-load hardening and its opt-out, patchable entries and register
clearing. Templates, native local methods, stored generic lambdas, field
initializers and parameter defaults retain the same checked attributes as
ordinary published functions.

Identity binds the complete selected builtin set, distinguishes all builtins,
preserves trusted-domain strings, and records patch count, offset and section
plus the exact register-clearing mode. Published local and lambda annotations
also contribute typed body identity. Imported parameter defaults remain in the
public declaration inventory instead of being filtered as private variable
initializers. Canonical printing preserves inherited policies, normalizes GNU
and C++ spellings, and deduplicates identical facts. C++23 lambda annotations
retain valid placement; strings retain C escaping.

`no_builtin` is definition-only. It remains on published definition bodies and
is omitted from the generated asm-label forward declaration. A private
out-of-line body's builtin policy remains in its producer's code; it is not
invented as a consumer declaration contract. The native all-builtins sentinel
prints as the accepted no-argument attribute. A default patchable-entry section
is omitted rather than printed as an invalid explicit empty section.

C library package consumption also preserves native builtin lookup rules.
Visible imported library declarations are consulted before Clang manufactures
an implicit C library builtin. Unimported/private declarations remain unavailable,
local shadows keep ordinary scope priority, and reserved compiler/target
intrinsics retain native lookup. Canonical declarations spell core diagnostic
integer sugar (`__size_t` and its signed/difference counterparts) using the
target's actual integer types; ordinary source typedefs remain named.

`objc3.native-function-policies.v1` binds new interface admission. Missing
capabilities, older readers and forged unrelated declaration owners reject.
Native controls use a separate resolved external ABI module and independent
source-free package consumers. Eighteen configurations execute C17/C++20/23
programs across pure, MRC and ARC modes at O0/O2; an additional x86-64 IR
configuration enables split stacks and load hardening globally with matching
producer/consumer profiles. Definition IR preserves each policy; ISO C noinline
consumers retain provider calls rather than inventing duplicate bodies. Native
controls also cover direct/on-demand/qualified library lookup, ordinary local
shadowing, private and pre-import rejection, exact source replay, rehashed
policy tampering and matching/mismatched trusted-call domains.

All 100 AST controls pass, including import after provider destruction, escaped
policy arguments, default patch sections, all-builtin suppression and native
core integer printing for x86-64, AArch64 and i386 C/Objective-C. Focused compiler
object relinking exercises the changed source; it does not qualify a new SDK.

This source span does not promote an SDK or claim every native/vendor contract
or full language/profile conformance. A matching current-source SDK/ABI 24
campaign remains necessary; SDK 2.62 / ABI 21 remains accepted until its
replacement passes the complete release gates.
