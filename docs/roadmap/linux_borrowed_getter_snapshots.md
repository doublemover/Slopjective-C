# Evaluated owners of borrowed getter calls

System 2.8.0 snapshots borrowed-return owner loans when the selected stable owner
slot is evaluated. A later argument cannot silently attach that loan to a
replacement owner. Consumption, binding replacement and scope expiry affect the
pending loan before the getter returns, including replacement followed by
restoration of the original pointer bits. An earlier independent lifetime pin
continues protecting the original owner.

The pending acquisition uses the existing CFG's expression evaluation order,
expiry operations and may joins. It stays separate from the owner's value so
passing an owner does not itself expose borrowed pointer data. Each executed
acquisition overwrites its previous snapshot; conditional owner operands select
the executed branch and comma operands use their final value. Function calls,
Objective-C message arguments and C++ call operators share the same handling.
This does not choose a language evaluation order where C++ leaves it unspecified
or qualify unsequenced C expressions.

Native witnesses cover C++20/23, ARC/MRC and PCH, replacement, consumption,
restoration, independent pins, unrelated argument effects, conditional/comma
owners, message/operator dispatch and repeated acquisitions. Frontend witnesses
repeat the key rejection and selected-owner acceptance cases. General indirect
owners, ownership counts and interprocedural effects remain open. Full Strict
System conformance is not claimed. Compiler identity changes require rebuilding
dependent PCH, interfaces and packages; runtime/extension ABI layouts are unchanged.
