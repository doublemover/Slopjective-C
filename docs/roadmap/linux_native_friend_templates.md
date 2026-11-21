# Native friend templates in Linux C++ packages

Source 2.180 / System 2.106 extends source-free class packages to hidden function
templates, constrained and variadic friends, delayed namespace definitions and
dependent friend types. Primary, partial and nested class-template definitions
retain their friend declarations and constraints. Native argument-dependent
lookup, overload selection and private access survive import. A hidden friend
does not become a namespace-qualified lookup name merely through publication.

Canonical printing puts the template head and its requires-clause before the
`friend` keyword. Dependency traversal binds template parameters, constraints,
signature types and bodies. Shared ordinary friend declarations require every
owning record to be exported with the same SPI policy. Class-template friends
remain attached to their template publication owner and retain its reserved ABI
tag. Naming binds an already instantiated function's template primary as well
as the function, covering instantiation before the top-level naming callback.
Independent native vendor tags retain their existing treatment. Incompatible
reserved owners and foreign redeclarations reject.

Typed body identity now traverses canonical nested function-type graphs, so
injected-class-name sugar and the spelled specialization reconstructed by
replay bind the same nominal owner and converted arguments. The
`objc3.cxx-friends.v2` reader cutover covers new friend contracts. Older readers,
missing capability requirements and namespace-forged ownership reject.

Four optimized source-free native configurations cover C++20/C++23 and ordinary
C++/Objective-C++ ARC/MRC. They remove the provider source and execute two
consumers, including native calls back into the provider. Controls check friend
bodies and constraints, dependent friendship, excluded/mismatched-SPI owners,
nonviable overloads, private-member access and hidden namespace lookup. An LLVM
AST control destroys the provider AST before printing and replaying imported
constrained friends in both language versions. All four native configurations,
18 selected publication regression groups, all 92 LLVM AST controls, complete
module-naming/SPI/codec tests and 26 tooling checks pass (one optional skip).
Maintained patch replay, generated authority and undefined-name checks pass.
These are focused development
checks; complete current-source SDK qualification remains pending. SDK 2.62 /
ABI 21 remains the last accepted archive.
