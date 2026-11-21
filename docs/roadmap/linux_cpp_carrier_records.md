# Native C++ records containing carrier fields

Source C++ records may own direct `Optional` and `Result` fields with pure C++
payloads. A record owns actual C++ special members and uses the target's native
C++ class calling convention. The carrier's synthetic representation remains a
layout description; it does not acquire fabricated C++ constructors.

Explicit field construction and in-class initializers construct the selected
payload directly in the member. Aggregate initialization uses the same recipe.
Generated copy and move constructors resolve the native operation for every
possible C++ payload, then execute only the selected branches. Const payloads
retain their native copy behavior. If a field constructor throws, completed
earlier fields are destroyed in reverse field order. Generated destructors run
the class destructor body before destroying selected payloads in reverse field
order. Native source classes preserve these rules in bases, enclosing classes,
new/delete, and placement construction after explicit class destruction.

Class special-member deletion, access checking, exception specifications, and
copy-parameter const qualification account for the real payload operations.
Carrier fields make copy and move operations nontrivial and prevent register
passing and `trivial_abi` overrides. Implicit class assignment is deleted until
carrier assignment has an independently qualified ownership contract. A carrier
field requires an explicit initializer; a missing initializer does not invent an
empty state for a general `Result`. These runtime recipes do not claim constant
evaluation support.

The supported storage graph contains ordinary source C++ records, bases,
ordinary scalar/reference members, and direct recursively nested carriers whose
C++ leaves have qualified native lifetime operations. Unions, volatile or packed
owned storage, arrays hiding carrier fields, and managed Objective-C or Block
siblings remain bounded. An outer carrier still cannot treat a source C++ class
that itself hides carriers as an ordinary payload leaf. Objective-C runtime
method encoding and Block by-value callable encoding remain separately gated.
Direct nontrivial carrier allocation and placement storage reuse remain gated;
real source C++ classes use their actual native constructors and destructors.

The installed recursive-carrier suite adds native class argument and return
boundaries across separate translation units, class and aggregate copy/move,
field rollback, destructor body ordering, inheritance, enclosing classes,
new/delete, placement reconstruction, PCH replay, and explicit bounded-form
rejections. This suite is evidence for those cases and does not claim full
profile conformance or extend native defer CFG ownership admission.
