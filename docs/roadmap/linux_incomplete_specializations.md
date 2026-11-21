# Opaque template specializations and later completion

Frontend source 2.120 extends incomplete-declaration publication to namespace and
member class-template partial specializations, explicit class specializations,
and external/static-member variable-template partial declarations. Shared and TLS
storage retain their native distinction. Ordinary class members and dependent
member graphs use the same rules.

The primary owns its specialization contracts; forward declarations preserve
selection and remain incomplete instead of falling back to the primary layout.
Later definitions replace the incomplete contracts. Tests check selected partial,
explicit and member tags, data/TLS values, printed replay, relocation and complete
graphs losing their declaration-only markers. Non-public specialization arguments
remain dependency errors.

Module-owned standard-library customizations may also remain opaque. Their
ownership still derives from an admitted user type and the canonical native
primary. Both explicit and partial `std::hash` declarations survive source-free
publication, remain incomplete in a consumer and accept later definitions with
usable operators. A forward declaration preceding its definition is validated
through that definition rather than rejected because the earlier node is opaque.

Admission makes a hard cutover to
`objc3.cxx-incomplete-template-declarations.v2`. The marker follows the owning
graph and independently published native standard specialization rows. The shared
catalog, codec, contract validator, fuzz dictionary and installed package checks
agree; v1 is not retained. The production decoder test now includes forward class
and variable partials as well as an explicit class specialization.

All four new source-free profiles and nine adjacent frontend groups pass focused
consolidation. Instrumented C++20/O0 and C++23/O2 witnesses execute partial,
explicit and member selection, external/TLS completion and standard hash invocation.
The registered production decoder integration repeats forty exact replays and
checks changed-byte and unknown-capability rejection. Codec/isolation units and all eleven selected frontend groups pass.

The SDK fixture includes opaque partial/explicit type checks, pointer identity,
selected shared/TLS symbols and opaque native standard-library pointers. Installed,
source-removed package execution remains part of final matched SDK consolidation.
Extension ABI 24, native frame/service ABIs and schema 2.2 are unchanged. Broader
dependent lookup, unmodeled declaration/predicate contracts, vendor configuration,
package/profile obligations and final SDK delivery remain tracked separately.
