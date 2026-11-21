# Stored owner values and captured field requirements

System source 2.27 extends borrowed-owner provenance to tracked pointer fields and
array elements. A field load can establish a stable owner binding, carry the
actual identity from its initializer or record copy, and acquire a loan without
first introducing a scalar alias. Replacing, consuming, exposing a mutable output
or overwriting the representation retires the affected binding's loans. Different
fields of an input record have independent symbolic identities.

Variables and stored owners share one append-only identity namespace. Nested
analyses inherit that namespace with their storage paths before adding locals.
Field values flow through the ordinary CFG owner facts, including local aliases,
overwrite kills, aggregate copies and reference bindings. Conditional field
owners preserve each selected source. Getter acquisition snapshots occur when
stable field operands are evaluated, so later argument replacement or consumption
cannot conceal an expired loan.

Cached closure summaries retain the field/element paths used to acquire or
consume owners. Capture construction projects these paths through native record
copies, aggregate initializers and conditional initializers, then freezes their
current dependencies. Nested and copied closures retain the same requirements.
Using one field does not require an unrelated field to remain live; overwriting
a local alias before borrowing replaces its requirement. Later replacement of a
by-value source record does not retarget its captured owner.

Summary projection shares the existing native shape checks used for callable
members. Reference components resolve through the independent reference-binding
domain. Path depth, origin count and tracked positions remain bounded; an open or
unresolved projection requires an explicit stable lifetime contract. This span
covers tracked plain owner fields and concrete elements. General indirect/global
mutation, untracked array positions, independent managed-field ownership, lazy
frames and exceptional ownership remain explicit completion work.

The new maintained regression has eighteen C17/C++20/C++23 ARC/MRC source/PCH
commands. It covers direct and copied fields, nested records, arrays, input-record
field precision, conditional selection, mutable outputs, representation writes,
argument evaluation snapshots, and direct/aliased/copied/nested/initialized
captures. Source-free frontend publication/replay and rejecting consumers defend
the same owner-field contract. All thirty borrowed compiler regressions and three affected native frontend
groups pass. Extension ABI 24, physical runtime ABI 4 and accepted SDK 2.62 remain
unchanged.
