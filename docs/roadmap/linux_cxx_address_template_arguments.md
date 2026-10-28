# Address template arguments on Linux

Source **2.76** extends exact template argument identity to non-null function,
object, reference and member addresses, including structural template values.
Identity binds declarations and module-owned names, canonical parameter types,
structural subobject/base/array paths, byte offsets, one-past state and converted
member-pointer paths. It never binds a host process address.

The capability `objc3.cxx-address-template-arguments.v1` follows published argument
syntax and semantic types through complete owners and definition sets. Missing
or inconsistent capability metadata is rejected with `O3IFC085`. Dependency
validation checks address leaves inside structural values as well as direct
arguments; excluded exports and internal objects remain rejected.

The [focused source receipt](../support/linux_cxx_address_template_arguments_progress.json)
records the emitter suite, 83 tooling tests and 16 source-free configurations /
96 commands passing, plus 411 compiler regressions, 23 focused native checks and
all 37 native import-family tests. Native import preserves converted member paths,
array fillers, null member pointers and empty union/lvalue paths. Witnesses
cover source-free object/function/member pointers, reference arguments, exact full
function/class/variable specialization selection, provider/consumer shared template
storage, structural callable/member values and array/one-past subobject identities.
Native import preserves converted member-pointer paths and target AST ownership,
array fillers, null members and empty union/lvalue states.
Combined validation and SDK delivery remain batched;
[SDK 2.62](../support/linux_source_batch_2_62_acceptance.json) remains accepted.
Remaining generic closure/type projections, custom coroutine promises, ownership,
concurrency, isolation, key-path, language/library/profile work and native Linux
AArch64 remain open.
