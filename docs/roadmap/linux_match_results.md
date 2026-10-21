# Native aggregate match results

The Objective-C 3.0 compiler now lowers aggregate-valued `match` expressions on
Linux x86-64 with GNUstep 2.2. Previously these expressions parsed and type-checked,
then failed during code generation. A selected arm now initializes the caller's
result storage directly.

```objective-c
Optional(id) copy = match(input) {
  case .Some(let object) => Some(id, object);
  case .None => None(id);
};
```

The implementation preserves native ARC ownership and C++ value semantics.
Record-valued lvalue arms resolve their copy/move constructor and access checks
through ordinary C++ initialization. Prvalue arms preserve guaranteed copy
elision, including types with deleted copy and move constructors. Explicit
rvalues select moves. A completed result is destroyed if an arm or scrutinee
temporary destructor throws; a failed constructor leaves no initialized result
to destroy.

A case binding names a snapshot of the single scrutinee evaluation. Mutating the
original scalar or carrier through another name does not change that binding.
Managed snapshots retain their active payload and release it after the match;
branch guards and arm temporaries retain their own cleanup scopes. Discarded
results still destroy their active values.

The result paths support native records and admitted Optional/Result storage,
with separate compilation and semantic interfaces. The existing scrutinee and
pattern limits remain explicit: integral/enum/C-pointer domains and supported
carrier cases; broader object type tests, string patterns, mutable/move patterns
and additional scalar/complex result domains require further implementation.
This milestone does not claim a complete Core profile or a public async/actor
capability.

The installed integration gate compiles ordinary and PCH consumers in Objective-C
with a C17 base and Objective-C++ in C++20/23 mode, under ARC/MRC and O0/O2.
Separate ARC interface consumers execute after provider source removal. O0 C++
debugger checks inspect the copied record and its live-object count in both
ordinary and PCH cases. The [acceptance receipt](../support/linux_match_results_acceptance.json)
binds the admitted development compiler and fixtures; it distinguishes that
compiler from the last activated SDK.

```sh
python3 -B SDK/share/objc3c/source-build/qualify_match_results.py \
  --sdk-root SDK --output-dir out/native-validation/match-results
```

SDK packaging installs the fixtures and runner and executes this gate. The
activated native-library SDK remains identified by its own
[delivery receipt](../support/linux_native_stdlib_acceptance.json) until a new
compiler archive is built and activated.
