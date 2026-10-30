# Published generic local C++ callables

The 2.39 source span admits closed generic lambdas in concrete published bodies,
class/default/member initializers and constant data. Type arguments must have
stable named types; integral, null and pack template arguments have explicit
length-delimited identities. Local/unnamed argument types, structural/declaration
non-type arguments, constraints, dependent exception specifications and effectful
or coroutine generic bodies remain separate contracts. Public template and
closure types remain open.

Both dependent source and complete concrete call-operator instantiations enter
body identity and dependency checking. Instantiations sort by semantic argument
keys, including empty packs, rather than compiler container order. Resolved
instantiated calls retain their selected native functions and methods. A private
static overload cannot disappear from an interface and silently select a public
alternative with the same result type. Unavailable or unsupported instantiations
fail closed.

Interfaces require `objc3.cxx-generic-local-lambdas.v1` alongside their existing
lambda/definition capabilities. Encoding, decoding and module validation require
the identity and capabilities to agree; loading, resolution and ordinary import
admit the same capability. Native lambda printing retains parameter declaration
syntax, including pack ellipses, defaults and attributes. Immediate and static
operators retain their specifiers; static and explicit-object operators never
acquire a synthetic `mutable` keyword.

Authored witnesses cover captures, mutable state, multiple specializations,
empty/variadic packs, integral parameters, defaults, nested generic lambdas,
constant/immediate evaluation, function-pointer conversion, named record methods
and C++23 static operators. Source-free pure C++ object consumers and
Objective-C++ ARC/MRC runtime packages exercise the imported bodies. Source
validation and installed delivery are recorded separately; SDK 2.33 remains the
accepted archive.

The [source receipt](../support/linux_cxx_generic_local_lambdas_progress.json)
records 406 compiler regressions, 192 native unit tests, all 33 frontend suites,
332 tooling tests and 24 source-free package configurations / 96 commands across
normal, eager-import and ASan/UBSan execution. The compiler invoker-import
regression fails before the native repair and passes afterward.
