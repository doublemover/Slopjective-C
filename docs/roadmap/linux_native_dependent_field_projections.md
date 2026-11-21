# Dependent native field projections

Source 2.219 / System 2.145 preserves cleanup/resource provenance through selected
concrete method-template and class-template getter bodies that remain in Clang's
pending-instantiation queue when concurrency checking runs. The proof uses the
active Sema explicitly, verifies the AST context/current-function/implicit-used
specialization guards, and materializes only the selected visible inline
single-return field/member expression. It does not drain unrelated pending work
or infer a concrete field from an unresolved template pattern.

Getter inference shares 256 work units, a 16-level helper depth bound and an
active-method cycle set. Materialization consumes 16 units before calling the
native Sema instantiation entry point. The selected definition still undergoes
ordinary closed dispatch, ownership, availability and effect restrictions.

Native casts, unary-plus, comma and conditional return expressions retain field
identity, including alternative field references and reference writes into a
local aggregate cell. Boolean conversion and pure arithmetic/status/literal
observations do not copy resource identity. They do establish a receiver read
role, preventing a status getter from falsely escaping the same ordinary object
whose field is subsequently transferred. The pure observation walk excludes
mutation, addresses/dereferences, member-pointer operations, opaque calls and
volatile/atomic fields; recursive helper bodies retain the common bounds.

The analysis still collapses object fields into conservative local aggregate
cells. General pointer/pointee ownership, mutable globals, interprocedural state,
opaque/stateful/multi-statement bodies and open dispatch remain outside the
proved field graph. This does not claim complete raw-resource or full-profile
analysis.

All 432 source/PCH commands pass across the new dependent getter suite and five
adjacent projection suites, covering C17/C++20/C++23, ARC/MRC and four profiles.
An independent two-function regression previously accepted cleanup-managed copies
through pending method/class-template getters and now diagnoses both transfers.
The complete native isolation test passes, including awaited-executor managed
copies/reference writes and ordinary boolean observations. Fresh maintained-patch
application and 90 adjacent tooling checks pass, with four archive-dependent
skips. Matching SDK/archive acceptance remains separate.
