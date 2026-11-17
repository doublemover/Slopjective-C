# Native ABI tags in Linux C++ packages

Source 2.177 / System 2.103 preserves native vendor ABI tags alongside the
reserved module ownership tag. The seven native naming paths normalize one
complete tag set instead of requiring the owner to be the only tag. Native
class, function and variable templates, partial/explicit specializations and
ordinary classes keep their source-defined tags. Different or conflicting
reserved owners remain errors.

Canonical declaration printing retains complete effective tags across
redeclarations and quotes one native GNU spelling. Structural identity binds
only native tag payloads here; module ownership retains its existing independent
identity. Tag order is normalized. `objc3.native-declaration-attributes.v5` is a
hard reader cutover, covering these ABI contracts and prior diagnostic/physical
calling-convention contracts. ABI layout and runtime services are unchanged.

The dedicated source-free control authors native tagged records, template and
specialization graphs, nested function/variable templates and tagged unions.
Separate consumers exercise native provider calls, vtables, virtual destruction
and RTTI. Metadata changes and foreign/multiple reserved owners have rejection
controls. Matching SDK/complete qualification and unrelated roadmap work remain
open.

Focused checks pass: all four native ABI-tag configurations, all eight adjacent
diagnostic/union configurations, the complete module-naming test, twelve selected
existing publication groups, 91 LLVM AST infrastructure controls, production
codec and 22 related tooling tests. The naming test uses the production reader
catalogue; capability omission/older-reader/forged-name rejection remains
independently checked. Maintained patch replay and undefined-name checks pass.
These are implementation controls, not current-source SDK acceptance.
