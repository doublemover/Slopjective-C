# Objective-C 3.0 <a id="toc"></a>

Objective-C 3.0 is a native compiler built on a patched LLVM/Clang SDK. The compiler
uses the GNUstep 2.2 ABI and pinned libobjc2. Release qualification covers Linux
x86-64 and AArch64 on Debian 13 and Ubuntu 26.04.

[Download the Linux SDK preview](https://github.com/doublemover/Slopjective-C/releases/tag/v0.1.0-preview.1)
and follow [Getting started](../../docs/tutorials/getting_started.md) to compile your
first program. Try the [log summary example](../../docs/tutorials/log_summary.md)
for a complete command-line application.

[Linux release support](../../docs/releases/preview-support.md)

## Current implementation {#intro}

`objc3c` uses Clang's parser, AST, ownership model, native CodeGen and linker.
Objective-C objects, methods, protocols, categories, properties, ARC and Blocks
use the native runtime. `.objc3` selects Objective-C 3 with ARC and Blocks.
Semantic interfaces and explicit local package graphs support separate builds.

[The language capability schema](../../schemas/objc3-language-capabilities.json)
is the feature authority; the [generated capability reference](../../docs/support/clang_foundation_capabilities.json)
records the same states. Public `defer`, `Optional`, `Result` and typed `throws`
have qualified support. Public async/await and actors remain reserved.
Experimental task runtime adapters and migration intrinsics do not widen that
public language boundary. Each published archive lists the distributions and
minimum runtime versions in its release manifest.

## Start here {#toc-quick-routes}

- [Root README](../../README.md) for building or installing the SDK.
- [Linux release downloads and installation](../../docs/releases/linux.md).
- [Linux release status](../../docs/releases/status.md).
- [Build, run and verify](../../docs/tutorials/build_run_verify.md) for expected output and checks.
- [Installed walkthrough](../../docs/tutorials/guided_walkthrough.md) for managed modules and package graphs.
- [Showcase catalog](../../showcase/README.md) for runnable examples.
- [Tutorial index](../../docs/tutorials/README.md) for the full learning path.
- [Migration guide](../../docs/tutorials/objc2_to_objc3_migration.md) for existing Objective-C code.
- [Objective-C, Swift and C++](../../docs/tutorials/objc2_swift_cpp_comparison.md) for language boundaries.
- [Compiler API guide](../../native/objc3c/src/clang_foundation/README.md) for embedding the frontend and importing interfaces.

## Distribution and development {#decisions}

Use `objc3c` as the installed compiler and `objc3c-build` for explicit package
graphs. Repository maintainers build the SDK with
`python -m scripts.objc3c_toolchain build-sdk --fetch`, then activate
`out/toolchains/linux-sdk/activate.sh`. The [getting-started tutorial](../../docs/tutorials/getting_started.md)
contains the complete compile/run path and release-install alternative.
The [LLVM workflow](../../toolchains/llvm/README.md) describes compiler development.

## Specification map {#toc-front-matter}

The draft specification describes the intended language. Its design coverage is
broader than the implemented feature set; implementation claims follow the
capability schema above.

- [Draft introduction](../../spec/INTRODUCTION.md)
- [Grammar and precedence](../../spec/FORMAL_GRAMMAR_AND_PRECEDENCE.md)
- [Attributes and syntax](../../spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md)
- [Abstract machine](../../spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md)
- [Lowering and runtime contracts](../../spec/LOWERING_AND_RUNTIME_CONTRACTS.md)
- [Module metadata and ABI](../../spec/MODULE_METADATA_AND_ABI_TABLES.md)
- [Conformance checklist](../../spec/CONFORMANCE_PROFILE_CHECKLIST.md)
