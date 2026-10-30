# Nonvirtual C++ class lifecycle packages

The concurrency 2.8.0 delivery extends semantic packages beyond aggregate
records to bounded nonvirtual classes. The
[acceptance receipt](../support/linux_cxx_class_lifecycle_acceptance.json) binds
the source checkpoint, current SDK, archive and qualification reports.
Extension ABI 21, semantic interface schema 2.1, service ABI 4 and
frame ABI 1 are unchanged; compiler-dependent artifacts require regeneration.

The [2.8.1 correction](linux_cxx_deleted_members.md) additionally binds deleted
ordinary aggregate methods to this capability and semantic fingerprint. Its
receipt records the current SDK; this page retains the original class milestone.

Published classes may declare private/protected/public instance fields and
out-of-line members, constructors, destructors, copy/move operations and conversion functions.
Mutable fields and deleted declarations retain their native meaning. Callers
can construct, assign, move and destroy objects, use heap storage, and pass
nontrivial class values through the existing typed native-frame bridge.
Move-only classes can be frame value parameters; task results retain the shared
copyable-value/nonthrowing-destruction contract.

A required `objc3.cxx-class-lifecycles.v1` capability binds this contract. Class
identity includes layout, field access/mutability and native calling traits;
member identity includes access, deletion and explicit-conversion/constructor
state. Canonical source preserves class access sections and declarations.
Removing the capability is rejected before import, and unsupported declarations
suppress both interface products.

Module-owned constructors/destructors have a stable declaration stem and a
separate native name for each ABI entry: `_ctor_<CXXCtorType>` and
`_dtor_<CXXDtorType>`. Complete/base entry points never collapse onto one asm
label. Ordinary asm names retain upstream behavior. Deleted functions have no
required implementation symbol. Overloads retain individual declaration
identities; collision checks compare all earlier overloads, including later
members of a constructor/function overload set.

This stage excludes bases, virtual dispatch, concrete inline or template methods,
defaulted/constexpr member declarations, conditional explicit specifiers, member
initializers, nested declarations and default arguments. Source/header C++
support for those forms is independent of semantic-package publication. Existing
aggregate and ownership-projection bounds remain intact; public async/actor and
full profile predicates remain reserved.

Maintained fixtures cover canonical round trips, required-capability rejection,
owned structor symbols, overload conflicts, private layout and actual source-free
execution. The package provider exports copyable and move-only classes; consumers
exercise lifecycle operations, private access through public methods, heap
construction and typed async values after provider source removal.
All eight C++20/23 ARC/MRC O0/O2 cases pass in each normal, ASan/UBSan and
TSan mode (24 cases / 96 commands). The rebuilt foundation passes 370 compiler
regressions and 166 native contracts; all 32 facade suites, nine GNUstep clients,
37 installed consumers and 33 SDK gates pass. The installed native async package
gate executes 12 modes / 36 consumers, including eight C++ lifecycle modes and
four C-frame modes after source removal and package relocation. Fresh Debian 13
and Ubuntu 26.04 pass the exact archive as ordinary users. Both owned containers
are removed; superseded SDK outputs are reclaimed.
