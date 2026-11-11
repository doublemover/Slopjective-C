# C-style variadic template publication

Source 2.53 publishes namespace function templates and methods of supported
primary class templates with C-style ellipses. Both ordinary and templated
in-class methods retain native variadic calling conventions. C++ template packs
and C varargs remain distinct in the canonical callable type.

`objc3.cxx-variadic-templates.v1` is required whenever an exported function or
class-template owner contains an admitted variadic callable. Complete definition
identity includes the ellipsis, defaults, constraints and checked bodies. Missing
or inconsistent capability contracts reject, including older consumers that do
not advertise this capability. Existing dependency/export/SPI checks apply.
Effectful templates, detached definitions and explicit/partial specializations
remain separate work.

Maintained witnesses cover builtin `va_start`, `va_copy`, `va_arg` and `va_end`,
zero arguments, integer and floating default promotions, direct/member/indirect
calls and new consumer specializations after provider source removal. Emission
checks cover C++20/23, relocation, reprint, changed bodies, dependency rejection
and capability mismatch. The [focused source receipt](../support/linux_cxx_variadic_templates_progress.json)
records the optimized build, C++20/23 emitter suite and all eight source-free
runtime configurations / 32 commands passing. Broader validation remains batched;
SDK 2.52 remains the published archive and predates this source span. Full profile
and native AArch64 qualification remain open.

The [combined 2.53–2.62 source receipt](../support/linux_template_publication_batch_progress.json)
supersedes the deferred source checks above: 411 compiler regressions, 208 native
unit tests, 33 optimized frontend suites and the maintained 335 tooling tests pass.
Normal, ASan/UBSan and TSan each pass 168 source/PCH configurations / 1,158 commands.
Source-free C++/key-path packages pass 80 configurations / 384 commands.
The [SDK 2.62 acceptance receipt](../support/linux_source_batch_2_62_acceptance.json)
completes coordinated delivery: all 33 installed gates and exact-archive native
feature qualification pass on fresh Debian 13 and Ubuntu 26.04 as ordinary users.
