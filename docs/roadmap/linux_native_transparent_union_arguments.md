# Native transparent-union arguments

Source 2.222 / System 2.148 publishes GNU `transparent_union` contracts on native
C unions. Previously the publisher rejected this modeled-by-Clang argument
conversion/ABI policy as an unsupported declaration attribute. Native attribute
admission and canonical printing now retain it under the existing version-six
native declaration attribute contract. The attribute changes the union's semantic
identity; its member layout and dependency identities remain native Clang data.

Matching C17 and C23 providers and source-free consumers compile, link and execute
the pointer argument ABI and return 42. A non-member `double` argument is rejected.
Removing the attribute changes the declaration identity. Exact language-revision
admission remains enforced; this does not broaden cross-language or cross-revision
interface compatibility. The preceding publisher fails the new native test.

The native test is registered as its own bounded frontend owner for CI. Fresh
maintained-patch application and 90 tooling checks pass, with four
archive-dependent skips. Native ARM and matching SDK/archive acceptance remain
separate work.
