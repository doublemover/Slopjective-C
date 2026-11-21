---
title: Objective-C 3.0
layout: default
---

# Objective-C 3.0 <a id="toc"></a>

Objective-C 3.0 is a native compiler built on a patched LLVM/Clang SDK. The compiler
uses the GNUstep 2.2 ABI and pinned libobjc2. Release qualification covers Linux
x86-64 and AArch64 on Debian 13 and Ubuntu 26.04.

[Linux release downloads and installation](https://github.com/doublemover/Slopjective-C/blob/main/docs/releases/linux.md)

[Linux release status](https://github.com/doublemover/Slopjective-C/blob/main/docs/releases/status.md)

## Current implementation <a id="intro"></a>

`objc3c` uses Clang's parser, AST, ownership model, native CodeGen and linker.
Objective-C objects, methods, protocols, categories, properties, ARC and Blocks
use the native runtime. `.objc3` selects Objective-C 3 with ARC and Blocks.
Semantic interfaces and explicit local package graphs support separate builds.

[The language capability schema](https://github.com/doublemover/Slopjective-C/blob/main/schemas/objc3-language-capabilities.json)
is the feature authority; the [generated capability reference](https://github.com/doublemover/Slopjective-C/blob/main/docs/support/clang_foundation_capabilities.json)
records the same states. Public `defer`, `Optional`, `Result` and typed `throws`
have qualified support. Public async/await and actors remain reserved.
Experimental task runtime adapters and migration intrinsics do not widen that
public language boundary. Each published archive lists the distributions and
minimum runtime versions in its release manifest.

## Start here <a id="toc-quick-routes"></a>

- [Root README](https://github.com/doublemover/Slopjective-C/blob/main/README.md) for building or installing the SDK.
- [Getting started](https://github.com/doublemover/Slopjective-C/blob/main/docs/tutorials/getting_started.md) for compiling and running an application.
- [Build, run and verify](https://github.com/doublemover/Slopjective-C/blob/main/docs/tutorials/build_run_verify.md) for expected output and checks.
- [Installed walkthrough](https://github.com/doublemover/Slopjective-C/blob/main/docs/tutorials/guided_walkthrough.md) for managed modules and package graphs.
- [Showcase catalog](https://github.com/doublemover/Slopjective-C/blob/main/showcase/README.md) for runnable examples.
- [Tutorial index](https://github.com/doublemover/Slopjective-C/blob/main/docs/tutorials/README.md) for the full learning path.
- [Migration guide](https://github.com/doublemover/Slopjective-C/blob/main/docs/tutorials/objc2_to_objc3_migration.md) for existing Objective-C code.
- [Objective-C, Swift and C++](https://github.com/doublemover/Slopjective-C/blob/main/docs/tutorials/objc2_swift_cpp_comparison.md) for language boundaries.
- [Compiler API guide](https://github.com/doublemover/Slopjective-C/blob/main/native/objc3c/src/clang_foundation/README.md) for embedding the frontend and importing interfaces.

## Distribution and development <a id="decisions"></a>

Use `objc3c` as the installed compiler and `objc3c-build` for explicit package
graphs. Repository maintainers build the SDK with
`python -m scripts.objc3c_toolchain build-sdk --fetch`, then activate
`out/toolchains/linux-sdk/activate.sh`. The [getting-started tutorial](https://github.com/doublemover/Slopjective-C/blob/main/docs/tutorials/getting_started.md)
contains the complete compile/run path and release-install alternative.
The [LLVM workflow](https://github.com/doublemover/Slopjective-C/blob/main/toolchains/llvm/README.md) describes compiler development.

## Specification map <a id="toc-front-matter"></a>

The draft specification describes the intended language. Its design coverage is
broader than the implemented feature set; implementation claims follow the
capability schema above.

- [Draft introduction](https://github.com/doublemover/Slopjective-C/blob/main/spec/INTRODUCTION.md)
- [Grammar and precedence](https://github.com/doublemover/Slopjective-C/blob/main/spec/FORMAL_GRAMMAR_AND_PRECEDENCE.md)
- [Attributes and syntax](https://github.com/doublemover/Slopjective-C/blob/main/spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md)
- [Abstract machine](https://github.com/doublemover/Slopjective-C/blob/main/spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md)
- [Lowering and runtime contracts](https://github.com/doublemover/Slopjective-C/blob/main/spec/LOWERING_AND_RUNTIME_CONTRACTS.md)
- [Module metadata and ABI](https://github.com/doublemover/Slopjective-C/blob/main/spec/MODULE_METADATA_AND_ABI_TABLES.md)
- [Conformance checklist](https://github.com/doublemover/Slopjective-C/blob/main/spec/CONFORMANCE_PROFILE_CHECKLIST.md)
