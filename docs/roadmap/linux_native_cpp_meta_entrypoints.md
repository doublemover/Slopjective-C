# Native C++ macro entrypoint identity

Source 2.214 / System 2.140 preserves canonical `objc_macro` markers on C++
function templates, static/instance methods, member templates, nested records
and class-template members, including redeclarations and out-of-line definitions. Markers remain declaration metadata; annotating a
function does not execute it as an unrestricted compiler plugin.

A function template records the attributes on its templated function. Exported
C++ records bind each marked member's stable declaration identity, including
nested records, rather than retaining one undifferentiated record flag. Moving
or removing a marker changes the record's canonical identity. Reader admission
accepts the corresponding function/member/record/template owners and continues
to reject unrelated owners and absent required capabilities.

The preceding source fails the new template identity assertion. The repaired
native compilation test passes C++20/C++23 publication and source-free consumer
instantiation for all listed forms, marker-placement changes and a forged
variable owner. Its existing six C17/C++20/C++23 × ARC/MRC execution witnesses,
package/lock, dependency, C embedding and class/property controls also pass.
Extension ABI 24 and `objc3.expanded-metaprogramming.v1` are retained; full-profile
and matching SDK/archive acceptance remain separate work.
