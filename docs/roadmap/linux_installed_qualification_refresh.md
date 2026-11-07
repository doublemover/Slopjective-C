# Installed qualification refresh

The hosted native run on `277d14b64` passed the repaired source graph, then
rejected a supported C++ member-template specialization because the installed
qualification suite still classified it as unsupported. A generic-lambda factory
result had the same stale classification.

Both cases now live in the positive publication fixture. Each emits a provider
object/interface, removes the original source from the consumer directory,
imports through its exact semantic lock, and links/runs a fresh consumer. The
remaining twelve publication rejections require their specific `O3IFC101` or
`O3IFC112` class. Four dialect/ownership/visibility rejections retain their scope.
The complete C++ matrix passes **14 executions / 118 commands / 16 rejections**
with the current development compiler. No SDK archive was built.

Inspection also found **46 optimization-removable checks** in ten shipped
qualification scripts. Explicit `ValueError` gates replace them. Corrupt-evidence
controls execute every replacement under Python optimization levels 0, 1 and 2;
the source audit now covers both toolchain owners and shipped qualifiers. The
combined gate/assurance selection passes **213 tests** at this checkpoint.

The development diagnostic-prefix helper can run the full installed C++ matrix
directly through `--cxx-interfaces`. This reuses the current compiler and runtime
bytes without a package/archive cycle. Accepted SDK 2.62 and broader hosted CI
acceptance remain separate from these checks.
