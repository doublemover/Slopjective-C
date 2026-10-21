# Native match value results

The Objective-C 3.0 compiler now lowers value-producing `match` expressions on
Linux x86-64 with GNUstep 2.2. Previously these expressions parsed and type-checked,
then failed during code generation. A selected aggregate arm initializes the caller's result storage directly. Scalar
and complex arms join their native representations.

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
Managed snapshots preserve native field ownership. Reference payload bindings
initialize independent strong storage, using native atomic weak loads for weak
fields, and retain their values through the selected arm;
C++ carrier snapshots resolve their payload copy constructors in Sema and drain
the active payload on normal and exceptional exits. `var` bindings initialize
separate case-local storage with native copy/ownership rules. Failed guards
destroy that copy before trying the next arm; mutation leaves the scrutinee
unchanged. `let` bindings remain immutable. Whole-carrier bindings follow the
same rules. Branch guards and arm temporaries retain their own cleanup scopes. Discarded
results still destroy their active values.

The result paths support native records, admitted Optional/Result storage,
objects, Blocks, complex values, vectors and C++ data/function member pointers.
Object and Block results retain/copy the selected value before arm or scrutinee
cleanup, protect it against exceptional cleanup, then release it at the end of
the surrounding full-expression. Object/Block scrutinees use owned snapshots,
including loads from zeroing weak storage and nullable reference patterns.

These paths work with separate compilation and semantic interfaces. The
scrutinee domains remain explicit: integral/enum/pointer/reference values and
admitted carriers, including direct C++ class payloads with resolved copy and
destruction recipes. General nontrivial C++ carrier call ABI, optional object
type tests, string patterns and consuming `move` patterns require further work.
This milestone does not claim a complete Core profile or a public async/actor
capability.

The integration gate compiles ordinary and PCH consumers in Objective-C
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

Fresh archive qualification supports a `match-results` scope that runs the
installed native library and match gates as an ordinary user after installation,
relocation, embedding and debugger checks.

SDK packaging installs the fixtures and runner and executes this gate. The
activated native-library SDK remains identified by its own
[delivery receipt](../support/linux_native_stdlib_acceptance.json) until a new
compiler archive is built and activated.
