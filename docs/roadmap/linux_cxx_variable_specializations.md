# Explicit inline variable-template specializations

Source 2.58 publishes explicit namespace inline variable-template specializations
with independently owned storage and exact argument identities. Mutable values,
constexpr selection and thread-local storage preserve native provider/consumer
sharing, including floating-point signed-zero distinctions. Specializations stay
inside primary-template lookup and never become ordinary variable declarations.

The required `objc3.cxx-variable-specializations.v1` capability depends on the
primary variable-template capability. Contracts bind specialization arguments,
types, initializers and native owners. Private dependencies and inconsistent or
unsupported capability metadata reject. Native declaration replay prints the
explicit specialization head and arguments and preserves owned labels.

Maintained witnesses cover source/reprint/relocation, changed initializers,
capability rejection, native import/replay, constexpr selection, shared mutable
state and independent worker TLS after provider source removal. Non-inline
namespace variable specializations, partial specializations, class/member
specializations and detached definitions remain separate work. Combined
validation and coordinated SDK delivery remain batched; SDK 2.52 remains the
accepted archive. The [focused receipt](../support/linux_cxx_variable_specializations_progress.json)
records five native import tests in C++20/23, the emitter suite, 19 tooling
tests and eight source-free configurations / 40 commands passing. Full profile and native Linux AArch64 qualification remain open.
