# Linux GNUstep 2.2 direct dispatch

Extension ABI 6 cuts the Linux x86-64 SDK over to GNUstep 2.2 with libobjc2 2.3.
The public driver, semantic compiler, installed source builders, editor and
runtime clients share that profile. Obsolete runtime selections fail before
object publication, including options supplied through response files.
Rebuild providers, consumers, interfaces and PCH with the same sealed SDK.

## Native call and ownership contract

`objc_direct` methods call a native symbol and omit the runtime `_cmd` argument.
They must not appear in Objective-C runtime method tables: treating their
implementation as an ordinary IMP would use the wrong ABI. The GNUstep metadata
emitter now filters direct methods, including classes with only direct entries.
Ordinary methods retain their normal reflection and message behavior.

Direct instance methods return zero values for nil receivers. Managed results
become Optional none or Result Err with a zero payload, and the caller's
argument snapshots still clean up. Hidden typed-error output is initialized
before the direct-method nil prologue. Native Objective-C/C++ exceptions unwind
snapshots and completed results under the existing owned-value contract.

Direct class methods perform GNUstep lazy initialization. The installed witness
starts four threads together, makes simultaneous first calls and verifies that
`+initialize` runs exactly once. An explicit `_cmd` reference inside a direct
method uses the compiler-generated selector.

Semantic method identity includes direct dispatch and requires
`objc3.direct-dispatch.v1`. Declaration printing preserves the ABI attribute;
source round trips retain the same fingerprint. Consumers reject an unknown or
erased capability. Ordinary and direct declarations have distinct identities.

## Qualification

SDK publication runs `qualify-direct-carrier-calls.py` outside the installation
in paths containing spaces and quotes. Its 80 commands require nine successful
executions, twelve forced-failure traps and nine obsolete-runtime rejections.
The matrix covers Objective-C and Objective-C++ at O0/O2/ThinLTO, separate
providers/consumers, semantic imports, PCH, strong/weak/Block values, Result arm
ownership, nil and discarded results, failed assignment, native unwinding,
runtime metadata exclusion and concurrent lazy initialization. Every executable
passes DWARF verification. Ignored attributes are errors.

The three existing installed call suites remain mandatory: 27 executions and
18 forced failures. Live clangd probes inspect direct-call result types in both
languages. Fresh Debian 13 and Ubuntu 26.04 archive qualification repeats these
checks as an ordinary user after relocation.

The [acceptance receipt](../support/linux_direct_dispatch_acceptance.json) binds
the source, archive, SDK seal, interface, editor, debugger and distribution
evidence. Public Optional/Result/throws predicates remain reserved until the
bounded synchronous language contract is complete.
