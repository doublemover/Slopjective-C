# Locked native metaprogramming

Source 2.212 / System 2.138 implements a bounded Part 10 expansion host. Explicit,
content-locked packages generate real Objective-C class declarations and method
bodies before final checking and code generation. This replaces metadata-only
request recording for the supported package-program contract. It does not claim
the optional `OPT-META` profile or provide default implementations of its four
portable derives.

## Using a package

Compile with repeatable `--meta-package directory-or-objc3-meta.json` and one
`--meta-lock file` option:

```sh
objc3c module.m --objc3 --meta-package ./meta --meta-lock ./meta.lock \
  --emit-object module.o -- -std=c17
```

The package directory contains `objc3-meta.json`. There is no ambient discovery,
network acquisition, executable plugin loading or host-program execution.
`objc3c --meta-package-fingerprint ./meta/objc3-meta.json` prints its canonical
fingerprint; copy that value into the manifest and the corresponding lock row.
The fingerprint excludes its own field and JSON object-key order/whitespace,
but includes exact template strings and program array order. SHA-256 hashes
`objc3c.meta.package.v1\n` followed by canonical semantic JSON.

A minimal manifest, with the fingerprint filled in, is:

```json
{
  "contract_id": "objc3c.meta.package.v1",
  "id": "example.answers",
  "version": "1.0.0",
  "fingerprint": "FILL_WITH_THE_PRINTED_SHA256",
  "requires": {"meta_revision": 1, "extension_abi": 24},
  "capabilities": [],
  "entrypoints": [{
    "name": "Answer",
    "kind": "macro",
    "requires": [],
    "declarations": [{"text": "- (int)answer;"}],
    "implementations": [{"text": "- (int)answer { return 42; }"}]
  }]
}
```

The lock is an exact set, with no missing or unused rows:

```json
{
  "contract_id": "objc3c.meta.lock.v1",
  "packages": [{
    "id": "example.answers", "version": "1.0.0",
    "fingerprint": "FILL_WITH_THE_SAME_SHA256"
  }]
}
```

Apply `__attribute__((objc_expand("example.answers::Answer")))` before a concrete
class interface and provide its ordinary `@implementation`. `objc_derive("Name")`
selects a `derive` entry, while `objc_property_behavior("Name")` selects a
`property-behavior` entry on an individual property. Unqualified names must be
unambiguous in the explicit package set. The canonical zero-argument
`objc_macro` function marker survives interface and PCH transport; it does not
permit execution of an arbitrary annotated host function.

## Program and query contract

Programs contain text, property/ivar loops, conditional branches and explicit
requirement errors:

```json
{"for_each":"properties", "where":{"kind":"integer", "writable":true},
 "separator":" + ", "body":[{"text":"self.${member.getter}"}]}
```

`{"if":"ownership.arc","then":[...],"else":[...]}` selects a branch;
`{"error":"missing serializer for ${class.name}"}` rejects the request.
Supported queries are `class.name`, `class.superclass`, `entry.name`,
`package.id`, `package.version`, `package.fingerprint`, and, inside a member loop,
`member.name`, `member.type`, `member.getter`, `member.setter`. Append `:string`
for a correctly escaped C string literal; `$$` emits a literal dollar sign.

Predicates cover class member/superclass presence, C++/ARC/strict mode, member
writability, borrowing, resource, weak, atomic and class-property status, and
integer/floating/object/Block/pointer/record kinds. Queries examine the complete
authored class interface and explicit ivars; synthesized or generated members do
not become additional inputs to a recursive expansion. Requests compose in
source order. Unknown queries, predicates, operations and schema keys reject.

Generated tokens bypass caller preprocessor macros. Templates use canonical
language declarations and semantic names; CPP conveniences such as `nil`,
`NULL` or user macro aliases are not implicitly expanded. Preprocessing,
container escapes, recursive expansion requests and inline assembly are rejected.
Normal Clang checking still governs generated declarations, access, types,
ownership, bodies and conflicting signatures. A generated method that collides
with another explicit class declaration is rejected.

Property behaviors currently support properties on concrete class interfaces with explicit
`dispatch: "direct" | "dynamic"` and `executor: "none"`. The actual accessor
methods must match that dispatch contract and have implementations when the
class implementation is present. Executor-dependent/actor behavior lowering,
protocol/category applications and more general storage transformations remain
outside this contract; they diagnose rather than silently weakening their
requirements. Normal declared properties can use ordinary autosynthesis.

## Bounds, publication and separate compilation

Inputs are bounded regular files: at most 64 packages, one MiB per document,
32 JSON nesting levels, 32,768 JSON nodes and 4,096 program nodes per package.
Expansion has eight program nesting levels, 65,536 work units, at most 4,096
members per loop and one MiB of combined output per class. Duplicate decoded JSON
keys, including escaped spellings, reject. SemVer, declared capabilities,
package fingerprints and the complete lock are checked before collection.

The structural pass suppresses function-body and dependency-file emission. The
final pass injects owned in-memory tokens, checks bodies and verifies class-query
inputs against the collected model. All package/lock bytes are rechecked before
normal transactional product publication. Primary/interface/sidecar/record and
dependency-file outputs cannot replace their metaprogramming inputs. Make-style
dependency files name the physical manifests and lock, not generated buffers.
The semantic dependency record additionally retains their exact content hashes.

The source-package builder accepts an optional `metaprogramming` object on a
package/application manifest, with `packages` (relative manifest paths) and
`lock` (one relative file). All inputs must remain in that source root. Package
source hashes include these files, so preserved timestamps cannot retain stale
expansion products.

Textual interfaces contain expanded declarations, attached protocols and
package/version/fingerprint/entry provenance. They require
`objc3.expanded-metaprogramming.v1`, bound bidirectionally to declaration
identities. Readers without that capability reject. Consumers of an expanded
interface do not need or rerun its original package. Extension ABI 24 and interface
schema 2.2 are retained; the new reader capability identifies this transport.
Matching SDK/archive and full-profile acceptance remain separate work.

## Focused verification

The maintained package-reader, interpreter, native compilation and CLI tests pass.
The compilation test executes six generated-code witnesses across C17/C++20/C++23
and ARC/MRC, checks LLVM bodies and normal message dispatch, removes manifests
before source-free imports, exercises direct/dynamic property behaviors, and
rejects missing accessors, collisions, invalid locks/API tails and unsupported or
forged reader capabilities. Dependency files retain package inputs without
leaking generated paths; failed collection preserves an existing dependency file.

Seventy-two source/PCH commands pass across those language/ownership modes and all
four profiles. Fresh patch application reproduces every changed maintained file
and every manifest digest. The focused graph/coverage/surface/export tooling
selection passes 90 checks; four SDK execution checks are skipped pending a
matching archive. These are focused current-source receipts, not SDK acceptance.

The dependency controls also repaired interface argument replay: Clang's GNU
option table now retains complete semantic operands and removes action, output
and dependency options. In particular, an orphaned `-MF` can no longer consume
the following resource-directory option, and clang-cl joined spellings cannot
misclassify an absolute Linux compiler path.
