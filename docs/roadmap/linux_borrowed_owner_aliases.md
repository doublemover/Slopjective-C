# Owner acquisition through tracked aliases

System source 2.29 admits contracted owner loads through closed pointer aliases,
joined record pointers, scalar indirection, reference-selected records and exact
byte views. The selected lvalue objects must have complete tracked native type
and storage proofs. Each acquired origin retains its actual storage binding;
retargeting the selector does not retarget an existing loan or hide replacement
of the original owner field. Consumption and field replacement preserve their
existing lifetime effects.

Selector operands participate in acquisition snapshots before later arguments.
The ordinary CFG alias and byte-position domains supply the object set, cached
once per acquisition evaluation. Metadata propagates into nested analyses, so a
local pointer alias inside a captured record body preserves the cached field
owner requirement. Dynamic reference members resolve to their actual objects.

Unknown pointees, open array indices and failed admission limits diagnose a
non-stable owner. They cannot silently discard owner effects and produce an
untracked borrowed result. The proof admits at most 64 selected objects and
bounds candidate-origin expansion at 4096. An over-budget case retains an
explicit diagnostic; introducing a stable scalar owner binding avoids repeated
selector expansion. These are bounded local proofs, not general interprocedural
or arbitrary array-index analysis.

Two maintained regressions add twenty-four C17/C++20/C++23 ARC/MRC source/PCH
commands. They cover consumption/replacement, selector retargeting, joined/scalar/
byte aliases, future record-owner requirements, reference members, argument
snapshots, open selectors and a deliberately exceeded candidate budget with a
stable-binding control. Source-free frontend publication and replay preserve the
aliased getter's owner-return relationship and reject escaping consumers.
All nine focused compiler regressions and three affected native frontend groups
pass, including twenty-four new source/PCH commands.

Extension ABI 24, physical runtime ABI 4 and accepted SDK 2.62 are unchanged.
Global/interprocedural effects, open positions and broader lifetime/ownership
contracts remain open. SDK consolidation follows the implementation sprint.
