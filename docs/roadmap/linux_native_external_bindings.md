# Explicit native external bindings

Source 2.191 / System 2.117 publishes native assembly labels and Objective-C
runtime names from unowned ABI packages. Logical declarations can bind C
functions, variables, callback storage, C++ static/instance entries and native
Objective-C classes/protocols whose physical names differ from their source names.
Module-owned adapters can import those packages while retaining their own reserved
names.

`objc3.native-external-bindings.v1` is a required reader cutover. Each external
binding wraps its ordinary typed identity in one length-delimited physical-name
contract. The canonical source retains the actual native attribute; both source
replay and AST import must reproduce that exact name and type. The production
catalog, emitter, contract validator and decoder agree on the capability and its
legal owners. The codec fuzzer dictionary includes both the capability and its
framing marker.

One native-name admission function validates reserved ownership and explicit
external bindings. External names cannot claim the reserved `__objc3_m_` domain.
Malformed lengths, missing types, nested wrappers, control bytes and unrelated
owners reject. Marker-looking text inside ordinary type arguments remains data.
Module-owned source still rejects an explicit name that conflicts with its
computed owner; callers use an imported ABI package for such foreign bindings.
Renaming the internal validator has no compatibility alias.

All nine optimized C17/C++20/C++23 × pure/MRC/ARC configurations pass. The provider
is removed before an owned adapter is published, and the adapter source is removed
before two consumers execute. Controls verify exact canonical replay, native
function/data/callback address identity, dotted ELF labels, C++ static/instance
calls, Objective-C runtime protocol lookup and exact object destruction. They also
reject older/missing capabilities, malformed or forged owner/name metadata,
conflicting module-owned declarations and a changed physical name after a valid
source rehash. The latter fails specifically at semantic identity comparison.

The complete module-naming suite, 18 related publication groups, all 17 import
groups, production codec and 70 tooling/authority checks pass. Extension ABI 24,
interface schema 2.2 and runtime layouts remain unchanged. This preserves declared
native ABI contracts; arbitrary foreign implementation behavior and broader
ownership-aware adapters retain their separate requirements. Matching SDK and
native-host acceptance remain pending.
