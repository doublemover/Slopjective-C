# C++ carrier value construction and call ABI

Source 2.136 / System 2.62 extends native Linux x86-64 caller-owned carrier
arguments and fresh indirect results to complete C++ record payloads with trivial
destructors. User-defined copy/move constructors retain their selected Sema
recipes, observable side effects and native exceptions. Plain, empty and union
records use the same value boundary. Pure C++ over-aligned values use correctly
aligned native storage. Runtime reflection preserves exact alignment through a
checked native storage description, including 64- and 128-byte payloads.

Runtime layout encoding accepts qualified trivial-destructor C++ records using
the existing opaque native storage description for ordinary alignment and
`__objc3_cxx_storage_v2` for larger alignments. The pinned GNU runtime decodes
that version with checked decimal arithmetic, power-of-two alignment, exact size
and body agreement, and rejection of malformed or overflowing descriptions.
Objective-C messages and native
Block calls preserve construction semantics even when no payload destructor is
needed. Typed checked calls bind results only after success; optional propagation,
discarded values, indirect calls and exhausted argument registers share the same
transport. Heap Block captures retain copy construction during stack creation and
heap copying, including constructor failure. Their helper identity now binds
the canonical C++ payload type as well as opaque runtime layout: equal-sized
records with different native copy/destruction operations cannot share a helper.

`objc3.cxx-carrier-call.v2` replaces v1. This is a hard semantic cutover: all C++
carrier value-call signatures and bodies require the new capability, including
otherwise plain records whose former byte-copy ABI differs from caller-owned
storage. `objc3.cxx-carrier-runtime.v2` independently requires the updated
native layout reader. Old readers reject v2 rather than interpreting it as a v1 or ordinary
trivial aggregate call. Reference-only carrier views do not invent a value-call
capability. Existing runtime/mixed capabilities continue to describe their
respective invocation boundaries.

All 49 dedicated compiler commands, 27 updated frontend groups, four publication
profiles and 380 production decoder replays pass. Fourteen runtime patch controls
and native layout controls pass with both release and ASan/UBSan runtime builds,
including 51 malformed-description rejection checks. The native fixture matrix
is being consolidated, including the equal-layout capture-helper regression.
This development span does not reseal the compiler or produce an SDK archive. Extension ABI 24, native frame 1, services 4 and schema
2.2 retain their layouts; final consolidation and one matched SDK follow the
implementation batch.
