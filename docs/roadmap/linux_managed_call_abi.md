# Linux managed carrier call ABI

The native migration contract admits direct ARC Optional/Result values with
strong, weak or unsafe Objective-C object leaves and strong Block leaves on
Linux x86-64, GNUstep 2.0, using the ordinary nonvariadic C calling convention.
It applies to Objective-C and Objective-C++ free functions, function-pointer
callbacks, C++ members and constructors, virtual dispatch, member pointers and
template instantiations. Ordinary Blocks remain supported as payload values.
Public Optional/Result and throws feature queries remain reserved.

## Ownership and physical lowering

Every managed parameter travels as a pointer to an isolated, initialized value
owned by the caller. The caller constructs that value when evaluating the
argument, before later arguments can mutate its source. The callee borrows the
slot and may assign to its parameter; caller cleanup destroys the final selected
arm at the end of the full expression. That cleanup also runs if a later
argument or the callee throws a native C++ exception.

A managed result uses fresh caller-provided `sret` storage. The callee constructs
its active arm before returning. Returning a local copies its selected payload;
explicit moves retain their existing source-clearing rules. A forwarding call
can construct directly into the result slot. Discarded results and returned
values passed onward as arguments still have full-expression cleanup. A named
receiver owns its result through its normal local lifetime.

The target classifier accounts for the result pointer and each parameter
pointer when allocating registers. Exhausting argument registers spills the
pointer, rather than copying the record onto the stack. Weak registrations keep
their original storage address. Inactive arms and padding are never read for
ABI coercion. Managed arguments have neither `byval` nor `dead_on_return`, since
the caller must read and destroy their final contents after the call.

Tag queries borrow initialized local, parameter, reference or pointer storage.
A query on a managed call result constructs an owned temporary and cleans it
up at the end of the full expression.

Extension ABI revision 3 introduced this contract; revision 4 extended it through
the [runtime-encoded callable subset](linux_runtime_call_abi.md), and revision 5
adds [managed typed-error calls](linux_managed_typed_errors.md). Compiler identity and locked
semantic interfaces bind consumers to that revision and the complete patch
identity. Physical representation similarity does not establish interchange
with another SDK.

## Qualification and remaining boundaries

The installed `share/objc3c/examples/native/qualify-carrier-calls.py` copies its
inputs outside the SDK into paths containing spaces and quotes, then verifies
the SDK seal before and after qualification. Its nine executions cover:

- Objective-C and Objective-C++ at O0 and O2, with separate provider and consumer
  objects, callbacks, all managed leaf kinds, Result arm changes, argument
  replacement, register exhaustion, native exceptions and C++ dispatch.
- Both languages with ThinLTO at O2, exercising ownership under interprocedural
  optimization.
- A C++ consumer using a precompiled header.
- Locked installed semantic interface consumers at O0 and O2, including strong
  and weak parameters/results and a stable semantic fingerprint.

Every executable also passes DWARF verification. SDK publication requires this
suite; fresh Debian and Ubuntu archive acceptance repeats it as an ordinary user.
For LLDB field inspection of header-defined C++ carrier typedefs, compile with
`-g -fstandalone-debug` to retain complete type definitions across translation
units. A live installed-SDK witness inspects the callee's parameter and copied
local, then the caller's returned local, checks their shared payload identity
and continues the application to a successful exit.

Objective-C messages and Blocks now admit direct managed parameters and results
under the runtime encoding contract. Managed signatures with ObjC3 typed throws
now preserve success/failure ownership across those callable forms. Variadic
calls, alternate calling conventions, MRC,
volatile carriers, nested managed storage and reference-bound returned
temporaries remain diagnostic boundaries. Mandatory tail calls are incompatible
with these owned value temporaries. These boundaries are checked before object
publication; they are separate from supported native C++ exception unwinding.

Next, carry the same active-arm rules through recursive storage and qualify a
runtime profile supporting true direct dispatch before promoting public syntax.
