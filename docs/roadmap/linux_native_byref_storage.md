# Native escaping byref storage

Source 2.138 / System 2.64 implements native Linux x86-64 escaping byref cells
for over-aligned C/C++ values and qualified pure C++ / mixed ARC carriers.
Stack cells publish their alignment without changing the cell layout. The pinned
GNU runtime preserves that alignment when allocating the forwarding cell.

Sema retains selected copy/move construction recipes, including access/deleted
checks and throwing constructor cleanup. AST import preserves the escaping flag,
Blocks attribute and recipe before dependent code queries them. Heap publication
commits only a completed value. A failed relocation leaves the stack forwarding
pointer usable and frees the uncommitted allocation. Concurrent initial copies
share one published cell, dispose the losing candidate and retain their own
references. Reference-count retries use constant stack space.

Selected native C++ destruction and managed source cleanup retain their qualified
source ordering. Throwing copy/dispose callbacks still free their allocations;
throwing Block disposal also clears weak references. Native weak Block admission
uses the Block header rather than an object-only refcount prefix. Encoding parser
callbacks now have exact function pointer signatures under runtime UBSan.

Published bodies require `objc3.native-byref-storage.v1`; missing, invented and
older-reader capabilities reject. Six C17/C++20/C++23 ARC/MRC publication profiles
cover dependent carriers, alignment mutation, relocation and source-free replay.
All 73 dedicated compiler commands, 56 AST controls, 29 frontend groups, 460
production decoder replays, fourteen runtime patch controls, six native ASan/UBSan
configurations / 21 commands, two instrumented-runtime configurations / seven
commands and the instrumented ThreadSanitizer race client pass.
The durable [verification receipt](../support/linux_native_byref_storage_progress.json)
records the completed checks. Full compiler/analysis consolidation and one matched
SDK follow the remaining implementation batch. Extension ABI 24, frame 1,
services 4 and schema 2.2 retain their layouts.
