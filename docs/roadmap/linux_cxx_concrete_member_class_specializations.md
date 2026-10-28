# Concrete member class-template specializations on Linux

Source **2.66** adds full and constrained partial specializations of class
templates owned by concrete records. The concrete parent retains the complete
primary and specialization contracts; specialization bodies introduce no
independent interface rows or concrete assembly labels.

Publication checks template heads, arguments, constraints, bodies, visibility and
source ownership. Replay preserves authored ordering and record qualification.
Every specialization keeps its primary member template's module ABI owner.
Consumers import full and partial definitions before comparing the concrete
parent, then use native specialization selection and lifetime rules.

The capability `objc3.cxx-concrete-member-class-specializations.v1` binds this
projection to concrete nested-template owners; missing or inconsistent metadata
is rejected with `O3IFC075`. Namespace specialization capabilities retain their
own ownership rules.

The [focused source receipt](../support/linux_cxx_concrete_member_class_specializations_progress.json)
records the emitter suite, twelve native importer tests, alias ABI code generation,
72 tooling tests and 16 source-free configurations / 96 commands passing. Source-free
witnesses cover provider allocation, consumer copying, virtual lifetime counts,
member templates, constrained pointer and non-type patterns, primary fallback
and new consumer instantiations across C++20/23, ARC/MRC, O0/O2 and both import
modes. Combined compiler/frontend/sanitizer validation follows implementation.
[SDK 2.62](../support/linux_source_batch_2_62_acceptance.json) remains accepted
and predates this source milestone.

Dependent-parent specializations, member function/variable specializations,
remaining effectful boundaries, full profile conformance and native Linux AArch64
qualification remain open.

The [combined 2.63–2.73 source receipt](../support/linux_member_effects_batch_progress.json)
supersedes deferred source checks above: 411 compiler regressions, 220 native unit
tests, all 33 optimized frontend suites and 335 maintained tooling tests pass.
Normal, ASan/UBSan and TSan each pass 168 source/PCH configurations / 1,158 commands.
Source-free C++/key-path packages pass 80 configurations / 384 commands.
Coordinated SDK delivery remains pending; SDK 2.62 stays accepted.
