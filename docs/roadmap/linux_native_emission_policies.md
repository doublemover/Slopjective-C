# Native emission policies in published interfaces

Source 2.149 / System 2.75 preserves Linux native emission policies through
source-free declarations, inline bodies, templates, static locals and import.
Supported attributes are `section`, `used`, `retain`, `weak`, `no_sanitize`,
`disable_sanitizer_instrumentation`, `no_profile_instrument_function`,
`no_instrument_function`, `no_stack_protector` and `nodebug`. Section names and
sanitizer selections contribute typed attribute values to declaration identity;
sanitizer-name sets are independent of argument order. Local policy values are
bound in a stable set instead of source insertion order.

Canonical printing retains written and inherited policies. Idempotent emission
flags print once when an inline definition inherits flags from its preceding
asm-label declaration. Valued and repeatable attributes keep their own contracts.
Generated external variable declarations now ask Clang to print `extern` with
their attributes, avoiding a prefix-order change on reparse. These repairs make
published source bytes stable, including repeated `used`/`retain` spellings.

`objc3.native-emission-policies.v1` binds the new publication semantics. Both
production validators require exact marker/capability agreement; missing,
invented and unsupported-reader claims reject. Alias/ifunc targets, arbitrary
unmodeled attributes and broader vendor configuration remain separate admission
requirements. No Windows or legacy compatibility is introduced.

Twenty-four compiler source/PCH commands cover C++20/23 under all four profiles.
The complete compiler sweep passes 547 executable cases (550 discovered, three
unsupported); the final idempotent-printer repair and additional helper gates
then pass 42 focused commands. All 64 AST controls pass, including native
attribute import and duplicate inherited-flag printing. Six C17/C++20/C++23
ARC/MRC publication profiles check exact source bytes, semantic relocation,
section/sanitizer/flag mutations, equivalent sanitizer sets, repeated flags,
static locals and templates. Sixteen adjacent frontend groups and 680 exact
production decoder replays across 34 families pass.

The maintained native witness removes its provider before compiling two
consumers. Three plain C++/Objective-C++ ARC/MRC ASan/UBSan configurations perform
27 compile/IR/link/execute operations. Provider and consumer IR preserve section
retention, sanitizer disabling and profile/stack-protector policies while an
ordinary control retains instrumentation. A separately defined strong symbol
overrides the published weak provider implementation in both consumers.

The [receipt](../support/linux_native_emission_policies_progress.json) records
focused and consolidation scope. Extension ABI 24/frame 1/services 4/schema 2.2
layouts keep their versions. Remaining implementation precedes matched analysis
and one SDK delivery.
