# Public Linux synchronous errors

The qualified public synchronous error contract uses explicit native function
effects and checked call expressions. It shares the Linux x86-64 GNUstep 2.2
compatibility profile, ownership operations and ABI of
[public Optional/Result values](linux_public_carriers.md).

```objc
int checked(int value) throws(int) {
  if (value < 0) throwError(value);
  return value;
}

int forward(int value) throws(int) {
  return tryValue(checked(value));
}

int recover(int value) {
  int answer = 0;
  handle { answer = tryValue(forward(value)); }
  catch(int error) { answer = -error; }
  return answer;
}
```

`throws(E)` declares one error payload type. Native type identity, ownership and
layout participate in function, callback, Block, C++ member and Objective-C method
compatibility. A redeclaration or import must preserve that effect and payload.
Nonthrowing functions, member pointers and Blocks can widen to a throwing
callable view; the qualified ABI appends a caller-initialized error slot which
the original nonthrowing callee leaves untouched. Checked calls preserve the
ordinary result and ownership operations. Dropping or changing a throws payload
is rejected. The bare `throws` form uses the existing native object error contract. Public
async/await and actors remain reserved.

`tryValue(call())` executes a throwing call and propagates its declared error to
the current compatible throwing function or active handler. Every evaluated throwing call requires an explicit checked form, including
throwing receivers and nested arguments. Unevaluated type and size queries do
not execute the call. `tryOptional(call())` turns success into a
present Optional and failure into absence; a returned Optional gains an outer layer, preserving the difference between
a successful absent value and a failed call. A Result return retains its Result
arm inside the Optional. `tryForce(call())` unwraps success and traps on failure.
The call and selected payload conversions are evaluated once.

`throwError(payload);` converts and transfers the payload into the current
error continuation. `handle { ... }` introduces a local error handler. Ordered
`catch(E name) { ... }` clauses recover a matching declared payload; `catch { ... }`
handles the remaining error. These constructs use the compiler's typed error
transport. Native C++ `try`/`throw`/`catch` and Objective-C `@try`/`@throw` retain
their ordinary exception behavior and participate in native cleanup and unwinding.

The public error forms are compiler syntax with no macro definitions. `throws`
is contextual after a function declarator or method signature. `handle` is
contextual before its compound statement, and `catch` is interpreted by that
handler. `tryValue`, `tryOptional`, `tryForce` and `throwError` are function-like
compiler forms in ObjC3 mode. Ordinary names, members, selectors, qualified C++
lookup and non-ObjC3 source retain their native grammar elsewhere.

Failure owns one initialized error payload and no successful return value. Each
selected catch owns the appropriate snapshot. A completed return value remains
callee-owned until full-expression cleanups finish; if a cleanup throws natively,
its storage is destroyed before unwinding. Failed assignment preserves the prior
destination. Unused arguments, nil receivers, direct dispatch and forwarding
thunks use the same qualified call ownership convention. Forced failure releases
completed managed values before trapping.

ARC strong, weak and unsafe object/Block leaves and recursive carriers use their
native active-arm operations. Weak errors preserve registration and zeroing.
Managed MRC transfer, unsupported payload storage, incompatible effects and
cross-mode interface reinterpretation are rejected. There is one declared error
slot; error unions, effect inference, asynchronous transport and cancellation
require separate contracts.

`__has_feature(objc3_throws)`, `__has_feature(objc3_typed_throws)`,
`__OBJC3_FEATURE_THROWS__` and `__OBJC3_FEATURE_TYPED_THROWS__` describe this bounded
contract on its qualified target, runtime and profile. They remain false without
ObjC3 opt-in and outside that qualification. Canonical printing and semantic
interfaces use the public effect spelling and lock the compiler and extension
ABI; dependent objects, PCH files, interfaces and packages must be regenerated
at the cutover.

Installed qualification exercises the public forms through ordinary calls,
callbacks, C++ members, Objective-C methods, direct dispatch and Blocks at O0,
O2 and ThinLTO, with PCH and semantic imports. It includes success, propagation,
catch recovery, optionalization, failed assignment, nil receivers, managed return
cleanup, native unwinding, weak zeroing and deliberate traps. Sanitizer, editor,
DWARF/LLDB and relocated ordinary-user Debian/Ubuntu checks bind the shipped
archive.
