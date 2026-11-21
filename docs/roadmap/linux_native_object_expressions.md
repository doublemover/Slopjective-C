# Native Objective-C expression packages

Source 2.192 / System 2.118 transports Objective-C property, indexed/keyed
subscript, reflection, literal and ARC writeback graphs through native AST import
and source-free interfaces. Explicit and implicit properties preserve object,
super and class receivers, getter/setter declarations and messaging flags.
Verified method and property accessor rows enter the consumer selector pool at
the module import point, so native `id`/`Class` sends and chained subscript
results retain method signatures without exposing hidden modules or ungranted SPI.
Compiler-owned `Protocol` reflection types reuse the consumer's canonical class.
Selector/protocol/type-encoding expressions, Objective-C booleans, native bridges,
string/number/array/dictionary literals and dependent literal packs retain their
typed payloads and factory dependencies. Existing subscript and pseudo-object
import now share complete typed accessor and dependency identity.

Imported nodes own all referenced declarations, types, source locations and
children. Property merging rejects changed ownership, atomicity, mutability or
accessor selectors even when the value types match. Opaque values retain the unique evaluation flag, and ARC indirect
copy/restore preserves the distinction between initialized inout storage and
zero-initialized out storage. Canonical printing separates every bridge keyword
from its type, uses the lowercase super-message keyword and normalizes qualifier
placement when MRC omits an ownership annotation. Reprinting the generated source
must reproduce the same bytes and semantic fingerprint.

`objc3.native-object-expressions.v1` requires current readers. Typed identity
binds exact property/accessor/protocol/factory references, receiver kind, selector,
encoded type, bridge kind, literal constant eligibility and dictionary pack
shape. Signature, constraint, field/default-argument and published-body
expressions participate. Unpublished implementation bodies do not create an
expression transport promise. The production emitter, validator and codec agree
on capability and declaration-owner admission; the codec dictionary includes the
new semantic and marker.

The native witness covers C17/C++20/C++23 in MRC and ARC at optimization level 2.
Providers are removed before two independent consumers, both with direct inputs
and with source-directed imports. Pre-import selectors remain unavailable; an
MRC cached selector miss does not prevent resolution after the import. It compares imported
property and subscript operations against directly compiled source for exact
receiver/argument evaluation order, accessor calls, assignments and compound
updates, then verifies native reflection, bridge ownership, copy/zero writeback
and exact object destruction. Literal-only packages additionally publish,
reparse and import twice without requiring a bundled Foundation implementation;
those checks establish compiler transport, not Foundation runtime acceptance.
Older/missing readers, forged declaration owners and validly rehashed changes to
selectors or encoded types are rejected. Native AST controls destroy the source
AST before checking retained graph ownership and evaluation flags, and reparse
all original source declarations including super expressions and literal packs.

All six native configurations, 107 AST controls, all 17 import groups, complete
source-import bridge/SPI/production-codec suites, 18 related publication groups and
16 tooling checks pass. Maintained LLVM patches replay byte-for-byte and all
manifest digests match.

Extension ABI 24, interface schema 2.2 and public runtime layouts are unchanged.
Objective-C method implementation-body transport, broader property behaviors,
metaprogramming, profile conformance and matching SDK/native-host acceptance keep
their separate obligations. The focused compiler-object checks do not qualify a
replacement SDK archive.
