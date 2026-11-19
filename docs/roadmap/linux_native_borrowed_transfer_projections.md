# Native borrowed transfer projections

Source 2.297 / System 2.223 follows the active arm in Clang's semantic union
initializer. A designated record arm retains its local pointer and C++
reference bindings. Unnamed bit-fields consume no initializer slot. Writes
through these aliases update the original cell. The compiler diagnoses escape
of a borrowed value from that cell and accepts an independent replacement.
The union controls pass 36 Strict System source and PCH commands for x86-64
and AArch64, including C17 and C++20/23 in ARC and MRC.

Source 2.221 / System 2.147 preserves known borrowed pointer identities at
concurrency boundaries when a native C++ getter returns a borrowed field through
an unqualified pointer result. The strict-concurrency path previously classified
that result as an ordinary scalar pointer; Strict System already rejected the
uncontracted return earlier. The boundary check now retains the known source in
the profiles where ordinary definition checking admits the getter.

Closed field/helper/template returns carry separate identity, borrowed and
pointer facts. Casts, conditional alternatives, pointer offsets and integer casts
of pointer bits retain the appropriate source. Boolean conversion, pure status
calculations and ordinary scalar/character copies remain observations. Reading an
unrelated scalar field does not inherit a sibling field's borrowed type.

The shared bounded source/index analysis follows scalar aliases, local writes,
immutable pointer cells and factory actual arguments without substituting plain
formal types for the original source. C++ lambda init captures and native Block
captures retain the same restriction. Awaited value checks and late frame source
checks include borrowed provenance, alongside resource provenance.

Borrowed rejection is separate from cleanup/resource unsafe admission. Marking a
raw pointer alias `objc_unsafe_sendable` cannot erase a known loan. Existing
structural borrowed type restrictions remain in effect. Ordinary strong object
snapshots and independent observations retain their native ownership behavior.
This does not replace Strict System's CFG owner/lifetime checker or claim complete
interprocedural, global, opaque producer or pointer lifetime analysis.

The new source/PCH controls cover aliases, pointer-cell writes, unchecked aliases,
getters/helpers/templates, alternative returns, offsets, pointer bits and C++
lambda captures across C17/C++20/C++23, ARC/MRC and four profiles. Strict System's
uncontracted-return diagnostic remains checked separately from the admitted getter
lanes. Native integration tests exercise source-free getter imports, independent
field observations, borrowed captures and awaited unchecked-parameter crossings.
All 576 source/PCH commands pass across the new and seven adjacent projection
suites. The complete native isolation test passes, including all four source-free
C++20/C++23 × ARC/MRC getter configurations and awaited unchecked-parameter
crossings. Fresh maintained-patch application and 90 tooling checks pass, with
four archive-dependent skips. Matching SDK/archive acceptance remains separate.
