# Linux runtime carrier calls

Extension ABI revision 8 extends the [owned carrier call contract](linux_managed_call_abi.md)
through Objective-C messages and Block invocation on Linux x86-64, GNUstep 2.2,
with ARC and the ordinary nonvariadic C calling convention. It preserves
caller-owned argument snapshots and fresh caller-owned result storage. Native
Objective-C and C++ exceptions unwind those snapshots; the
[typed-error contract](linux_managed_typed_errors.md) defines synchronous ObjC3
success/failure storage and propagation under revision 8.

## Runtime encoding

The compiler publishes a versioned nominal encoding for each supported storage
and ownership shape. A strong object Optional encodes as:

```text
{__objc3_v8_optional_S40=B(__objc3_v8_payload=@)}
```

The nominal name contains the carrier kind and each arm's ownership code (`S`
strong, `W` weak, `U` unsafe, `P` plain), followed by the hexadecimal bytes of
its runtime leaf encoding. Both Result arms participate, including a void arm.
The body describes the native boolean tag and payload union, so the shipped
runtime can compute the actual storage size and alignment. Aliases, class and
protocol constraints and complete callable identity remain in source types and
locked semantic interfaces; this encoding describes storage and ownership.

The runtime subset admits strong, weak and unsafe object leaves, strong Block
leaves, and plain arms whose encodings the shipped runtime can parse: ordinary
integers through 64 bits, bool, float, double, long double, void, selectors,
character pointers and void pointers. Int128, half/extended floats, arbitrary
pointed-to records and nontrivial plain records remain outside this subset.
The admitted leaf codes are `v @ # : * c C s S i I l L q Q B f d D`, `@?` and
`^v`; other qualifier or aggregate encodings retain diagnostics.
Nested Optional/Result arms use an `N` marker and a SHA-256 child identity;
their bodies recursively describe native storage. The
[recursive contract](linux_recursive_carriers.md) defines ownership and encoding
bounds. The broader ordinary-function subset retains its existing bounds.

Method and Block signatures retain Clang's source argument-frame convention.
They describe the logical value size; the hidden result pointer is not a source
argument. The versioned nominal types are nontrivial owned values. Their native
calling ABI passes argument pointers and an indirect result, just as source
record encodings alone do not describe C++ special-member semantics. Runtime
reflection is qualified; generic aggregate FFI coercion, forwarding and
Foundation invocation adapters require ownership-aware implementation.

Revision 8 is a hard cutover. Rebuild providers, consumers, PCH and semantic
interfaces with the same sealed SDK; source identity and module locks enforce
that coherence.

## Dispatch and lifetime

Supported dispatch includes class and instance methods, protocol-qualified
receivers and superclass sends. The classifier counts `self`,
`_cmd` when present, the result pointer and managed argument pointers before
spilling remaining pointers to the stack.

A nil receiver initializes an all-zero result: Optional becomes none; Result
becomes Err with a zero payload. This also applies to discarded results, since
their full-expression cleanup still reads the selected tag and payload. Caller
argument snapshots clean up even when no method body runs. The [direct-dispatch contract](linux_direct_dispatch.md) qualifies native method
symbols, lazy class initialization, nil results and imported method identity.

Heap Blocks with ordinary ARC object captures can accept and return managed
carriers. The ordinary Block copy/dispose helpers retain those captured objects;
the invoke function borrows caller-owned value parameters. The
[owned capture contract](linux_owned_block_captures.md) now adds active-arm value
copy/dispose helpers. Carrier ivars/properties, reference captures,
pointer/reference carrier signatures,
variadics, alternate conventions, MRC, volatile managed storage, arrays and enclosing records,
mandatory tail calls retain explicit diagnostic boundaries. The
[typed-error contract](linux_managed_typed_errors.md) now composes managed
signatures with synchronous throws under revision 8.

## Installed qualification

`share/objc3c/examples/native/qualify-runtime-carrier-calls.py` requires a sealed
SDK and copies its source into a separate directory with spaces and quotes.
It verifies the SDK seal before and after nine executions:

- Objective-C C17 and Objective-C++ C++20 provider/consumer objects at O0, O2
  and O2 with ThinLTO.
- Locked semantic module imports at O0 and O2, including managed protocol
  messages and a returned heap Block, with stable interface fingerprints.
- A C++ consumer compiled from a PCH containing the runtime callable types.

The witnesses cover ownership replacement, weak zeroing, unsafe pointers, strong
Block payloads, both Result arms, register exhaustion, nil/discarded results,
all supported dispatch forms, native Objective-C/C++ unwinding and runtime
reflection. Reflection verifies encoding identity, argument count and type,
storage size/alignment and Block signatures. Every executable passes DWARF
verification. SDK publication requires all 53 commands; Debian and Ubuntu archive
acceptance repeat the suite as an ordinary user without a checkout mounted.
The public driver gives discarded COMDAT entries in DWARF 5 address pools an
all-ones tombstone. This preserves valid debug ranges when separate providers
emit identical Block copy/dispose helpers and the linker keeps one definition.

The original nine ordinary-function executions remain a separate publication
gate. Live clangd qualification includes message and Block result navigation,
hover and completion in both languages. Public Optional/Result/throws predicates
remain reserved until the bounded public language contract is complete.
