# Linux managed records

Extension ABI 10 extends native ARC lifetime operations through enclosing records
and their constant-size arrays on Linux x86-64 with GNUstep 2.2. C records copy
and destroy each owned field; C++ records use generated or user-defined special
members. Carrier fields are nontrivial native subobjects. Default C++
initialization selects None or the zero-initialized Result arm. Mutable moves
preserve tags and clear transferred strong or weak payloads. Const moves copy.

Record assignment snapshots its source before destroying the previous value.
The helper identity includes layout, field offsets, array dimensions, carrier
arms, native ownership qualifiers, and object versus Block representation.
Ordinary ARC strong, weak, and Block fields retain their native operations when
mixed with carriers. Packed records, unions, volatile storage, and unsupported
payload layouts do not acquire this lifetime contract.

Completed fields and array elements become owned before initializer temporaries
are destroyed. Failed initialization destroys the completed prefix in reverse
order. This applies to native exceptions, typed-error propagation, C++ constructor
failure, and a throwing temporary destructor after a completed initialization.
Return storage remains callee-owned through initializer temporaries, lexical
cleanups, and parameter destruction. Named return optimization transfers that
obligation after the local cleanup relinquishes its result. Exceptional exits
destroy completed results exactly once. Generated C++ copies and assignments
account for potentially throwing Block
copies in their exception specifications. Associated lookup visits carrier
payload types.

Ordinary synchronous nonvariadic functions pass qualified records indirectly
and return through fresh caller-provided storage. The callee owns its incoming
record snapshot, including cleanup during native unwinding. C and C++ use the
same convention for matching C-compatible record declarations. Direct carrier
arguments retain their separately documented caller-owned convention. This is
a hard ABI cutover; rebuild every dependent object and semantic interface.
The [heap storage milestone](linux_heap_storage.md) extends qualified data
records into Objective-C methods, Block signatures and captures. Typed throws
record signatures remain a separate admission boundary.

The native runtime preserves an Objective-C exception's object when C++ cleanup
frames resume unwinding into a C handler. A permanent two-language CTest covers
128 direct throws and rethrows, balanced catches, C++ cleanup, and object release.

The installed `qualify-record-carriers.py` suite compiles all four C/C++
provider/consumer combinations at O0, O2, and ThinLTO. It also executes PCH and
semantic-interface consumers, verifies DWARF, and checks source diagnostics for
unsupported storage and call forms. Its lifetime witnesses cover nested fields,
record arrays, weak zeroing, hybrid ARC fields, self-assignment, generated moves,
constructor failure, templates, base classes, C++ closure copies, and native and
typed exception exits.

```sh
python3 -B "$OBJC3C_SDK_ROOT/share/objc3c/examples/native/qualify-record-carriers.py" \
  --sdk-root "$OBJC3C_SDK_ROOT" --output-dir ./record-carrier-evidence
```

Public Optional/Result and synchronous throws predicates remain reserved until
the bounded public syntax and contract are qualified. Qualified ivar/property storage and shared `__block` captures are now covered
by the heap storage milestone.

The [acceptance receipt](../support/linux_managed_record_acceptance.json) binds
implementation commit `e2626d31c` to the exact ABI 10 archive, installed execution,
ASan/LSan and LLDB evidence, and ordinary-user Debian 13 and Ubuntu 26.04 results.
