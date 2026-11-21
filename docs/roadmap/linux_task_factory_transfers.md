# Existing task ownership returned by a factory

Source 2.157 / System 2.83 permits an owned factory to return `task<T>&&`.
The bridge moves the unique native frame out of that task exactly once, while
retaining the factory storage through inner-frame destruction. Task result type
normalization preserves `Task::from_native` and task handle deduction.

Child spawn, detached spawn, explicit group spawn and explicit/inferred
`with_task_group` use the same rule. Mutable task rvalue references qualify as
asynchronous work in the compiler. Lvalue and const/volatile task references do
not transfer ownership; raw native frame references remain excluded because
copying the frame aggregate does not clear the original owner. The explicit
`with_task_group<T>` overload now constrains its callback before instantiation,
so rejected ownership forms also fail ordinary `requires` probes.

Permanent source/PCH fixtures cover all spawn entry paths, factory failure,
pre-entry cancellation, detached retirement, invocation/resumption counts and
weak capture release. Four source-free provider/two-consumer configurations
exercise transferred group results and cancellation, then execute the direct
spawn fixture with the changed production compiler translation units. A compiler
AST control checks admission across C++20/23 ARC/MRC. Complete rebuilt SDK
qualification remains pending; runtime and native frame ABI layouts are unchanged.
