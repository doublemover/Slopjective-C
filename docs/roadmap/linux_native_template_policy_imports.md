# Native template declaration policy import

Source 2.202 / System 2.128 closes native AST-equivalence gaps that can
otherwise reuse a conflicting declaration despite matching function types or
record layout. Function-template comparison queues the templated declaration,
retaining guide explicitness, native method category and existing borrowed or
isolation signature checks. It continues to use the shared structural-equivalence
work queue instead of recursively starting a second comparison.

Complete authored record comparison retains both bare and conditional explicit
policies and the template wrapper/head around a policy-bearing method. Class
and method `final` state participates in equivalence, including whole-record
imports where no individual method was requested separately. Implicit template
specialization caches do not substitute for authored policy definitions.

Four new regression groups fail against the previous implementation and pass
with the repair. They exercise namespace guide templates, member guide and
constructor templates, bare explicit constructors, class/method final changes,
static/instance member-template categories, renamed equivalent heads and
conflicting whole-record imports in both directions. All 122 AST controls pass.
No runtime ABI, semantic-interface schema or reader capability changes.

The repaired objects also pass all six optimized source-free deduction-guide
configurations, all 17 import groups and 18 publication groups. Fresh patch
application reproduces production/control sources byte-for-byte, manifest
hashes match and all 16 focused tooling checks pass. These checks do not
qualify a new SDK archive.
