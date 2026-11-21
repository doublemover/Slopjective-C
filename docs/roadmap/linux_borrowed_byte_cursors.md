# Stored byte cursors and typed representation access

System source 2.24 carries independent byte-position facts through pointer
bindings, aggregate/representation copies, CFG joins, captures and evaluation
snapshots. Character casts retain byte offsets without acquiring an ordinary
typed-target proof. Constant compound steps, prefix/postfix updates, conditional
bases, GNU statement results and returned builtin destinations preserve the
correct evaluated address. Dynamic/overflowing offsets, unknown provenance and
representation mutation discard complete byte proofs.

Builtin fills/copies and typed lvalue reads/writes share exact native-layout
range recovery. Root bounds, full access size, tracked field/element offsets and
actual type must agree. Typed selection can identify a pointer field inside an
equal-sized containing record; a representation copy still selects the complete
outer object. Member and subscript access use their native field offset/element
stride. Partial, mismatched, out-of-bounds and unmodeled union paths remain
conservative. A byte write cannot erase an entire stored pointer's loan.

Byte-address values expose the root's current loan/callable facts instead of
freezing its former contents. A complete overwrite removes old obligations;
future sibling payloads remain visible through a byte cursor. Exact pointer and
aggregate copies carry byte provenance. Unknown calls, weak writes, zero fills,
move captures, nullable joins and native proof invalidations update the same
independent domain. Closed byte sets are bounded at 64 unique positions; larger
joins lose their complete proof rather than dropping alternatives.

The implementation passes all twenty-four borrowed compiler regressions before
splitting the expensive size-boundary controls into their own maintained file.
The resulting selection passes all twenty-five regressions: eighteen new normal
source/PCH commands across C17/C++20/C++23 ARC/MRC plus six separate 64/65-target
boundary commands. These cover copies, joins, expression results, stale cursor
representations, typed stores/loads/members/subscripts, owner expiry, callable
consumption/clearing, expired payload overwrite and current sibling exposure.
The native frontend replay/identity/relocation control passes all six modes,
including published stored-cursor copies, clears and typed loan-producing writes,
with valid and escaping source-free consumers.

The extension ABI remains 24. This span does not rebuild or qualify the accepted
SDK 2.62 archive. Consolidated package/runtime/sanitizer matrices and SDK delivery
remain pending. Union active-member reasoning, global/interprocedural aliases,
mutable/reference/generic/lazy callable behavior and exceptional ownership remain
separate completion-ledger contracts.
