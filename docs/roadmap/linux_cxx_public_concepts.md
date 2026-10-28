# Published native C++ concepts

The 2.42 source span publishes named C++20/23 concepts through semantic
interfaces. Type, non-type and pack parameters, requires-expressions, composed
concepts and constrained local generic lambdas retain their native C++ meaning.
Consumers can use imported concepts in new constraints and overload sets.

Each concept carries a reserved module/package/declaration owner annotation.
Its semantic identity binds that owner, its parameter head and its typed
constraint expression. Concepts introduce no binary symbol. Owner annotations
survive printed source and PCH replay; native structural equivalence distinguishes
owners so unrelated modules cannot silently merge atomic constraints. Source
relocation preserves identity, while changed constraints or module owners change
it. Forged or duplicate owner annotations are rejected.

Concept references participate in dependency and export checks, including
requires-expression overload candidates and lexical requirement parameters.
Printed references retain qualified names, and non-type template parameters
remain lexical. Interfaces require `objc3.cxx-concepts.v1` and the C++ data
contract; encoding and module validation reject missing or inconsistent
capabilities.

Public function, class and variable templates, template-template parameter
heads and the broader language generics roadmap remain separate implementation
work. This source span does not claim full profile conformance or rebuild the
accepted SDK 2.33 archive.

The [source receipt](../support/linux_cxx_public_concepts_progress.json) records
409 compiler regressions, 195 native unit tests, all 33 frontend suites,
335 tooling tests and 24 source-free package configurations / 96 commands across
normal, eager-import and ASan/UBSan execution.
