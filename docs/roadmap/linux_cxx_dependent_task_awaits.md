# Native awaits in ordinary C++ task templates

Concurrency source 2.108 gives dependent native awaits a persistent AST syntax
bit. PCH serialization, AST import, printing and native/ODR expression identity
retain it. Instantiation rebuilds isolation entries, native-frame/task adapters
and checked-error bindings after substitution, preserving operator lookup and
ADL. Known scalar results with instantiation-dependent operands defer native
await processing; ordinary C++ `co_await` retains its own admission rules.
Native-frame templates keep their separate opaque continuation protocol.

The coroutine body builder unwraps function-template declarations before
processing concrete promises, including value-only template parameters. This
prevents a null function-prototype access during coroutine return construction.
The frontend detects native dependent awaits in published bodies and requires
`objc3.native-dependent-await.v1`. Task-only templates receive the capability;
otherwise equivalent ordinary `co_await` bodies have a distinct identity.

Extension ABI **24** is a hard cutover: regenerate dependent PCH, interfaces and
packages. The accepted SDK 2.62 archive is unchanged. All six owning patch packets
and their manifest carry the compiler change.

The matching development compiler rebuild completes. Six focused compiler
regressions pass sixty-six source/PCH commands across C++20/23 and ARC/MRC.
These include indirect frame/task/plain callbacks, scalar value dependence,
custom awaiters and ADL, ordinary-coawait separation, diagnostics and spelling.
Five native AST controls pass, including native/ordinary expression profile and
ODR separation and imported syntax preservation. Four package-source modes pass.

The focused native ARC/MRC storage witness passes all 944 cases and five
compile/link/execution commands. Direct task-template paths exercise executor
entries, noncopyable receivers, virtual dispatch, exact lifetime counts,
cancellation and quiescence, indirect callback forms and scalar substitution.
Custom awaiter/ADL controls use the compiler fixture's custom promise; the
production task promise continues to accept runtime-managed await operations.
Three affected frontend units pass syntax checks. Frontend/source-free package
execution, optimized/instrumented matrices and final SDK delivery remain pending.
