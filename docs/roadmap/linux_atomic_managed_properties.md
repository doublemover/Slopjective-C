# Atomic managed properties on Linux

Source 2.143 / System 2.69 implements atomic native Optional/Result properties
and qualified data-only C/C++ records containing carriers or ordinary ARC fields.
Getter snapshots copy ownership while holding the property lock. Setters move the
old value into separate storage and install the incoming value under that lock,
then destroy the replaced value after unlocking. Owner destruction may therefore
reenter the accessor without deadlock. Strong, weak and Block fields retain their
fieldwise copy/move semantics, including records first declared with a forward
declaration; the helper always inspects the complete record definition.

Runtime callback exceptions release their locks. Dual-object copies order the
actual lock addresses and acquire a shared lock slot only once. Lock release uses
an atomic release store. The additive managed-property swap entry leaves existing
public extension ABI 24/frame 1/services 4/schema 2.2 layouts unchanged. Records
with custom C++ special members, bases, volatile fields or unqualified storage do
not acquire the fieldwise managed-property admission.

`objc3.atomic-managed-properties.v1` binds published atomic declarations and
accessor bodies, including ordinary ARC record properties. Three source-free
profiles cover relocation, exact replay, atomic-to-nonatomic identity changes,
missing/invented capabilities and older-reader rejection. Production decoding
replays all 28 current capability families in 560 exact-byte controls.

All 38 dedicated compiler commands, 14 pinned-runtime patch controls, five native
framework configurations / seventeen commands, and four fully instrumented
runtime/client configurations / twelve commands pass. Native controls cover
concurrent writers/readers, deterministic snapshot lifetime after replacement,
weak and Block fields, reentrant owner destruction, colliding lock slots and
throwing callbacks. Instrumented runtime coverage includes ThreadSanitizer and
ASan/UBSan in both C17 and C++23. The
[receipt](../support/linux_atomic_managed_properties_progress.json) records the
prepared source and runtime identities. Full current consolidation and one
matched SDK follow the remaining implementation batch.
