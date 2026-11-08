# Native aligned Block storage and local alignment publication

Source 2.137 / System 2.63 implements native Linux x86-64 heap storage for Block
captures whose alignment exceeds malloc's guarantee. Clang emits a versioned
alignment descriptor and includes alignment in descriptor identity, including
trivial captures without copy/dispose helpers. The pinned GNU runtime allocates
aligned storage while preserving the existing copy/dispose, reference-count and
failed-construction cleanup protocol. Repeated heap copies retain the original
allocation and keep its alignment.

Ordinary C/C++ values, selected C++ carriers and ARC mixed carriers share the
allocation contract. Native constructors verify their actual heap addresses;
copy failures at stack construction and heap publication retain exact owner
counts. Empty carrier arms do not construct inactive payloads. C17 and C++20/23,
ARC/MRC, source/PCH, optimized/no-elision and C++-only exception configurations
have native controls.

Published bodies require `objc3.native-block-alignment.v1`. Local `alignas` and
GNU `aligned` attributes now retain dependency and value/type identity instead
of being rejected. That includes dependent type alignment and aligned locals
which do not ultimately require an aligned heap allocation. Missing, invented
and older-reader capabilities reject; source-free replay, relocation and
alignment mutations have controls in six C/C++ ownership profiles.

All 73 dedicated compiler commands, 420 production decoder replays and six native
ASan/UBSan configurations / 21 commands pass. Fourteen runtime patch controls
pass. Adjacent frontend groups and execution against the instrumented runtime
are being consolidated. Escaping byref cells retain their separate storage and
relocation implementation boundary.

Extension ABI 24, native frame 1, services 4 and schema 2.2 retain their layouts.
This remains an unsealed development compiler. One matched SDK follows the
implementation batch and consolidated verification.
