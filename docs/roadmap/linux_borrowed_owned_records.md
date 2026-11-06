# Independent owner fields in copied captures

System source 2.28 recognizes ARC strong fields retained by a copied record or
array capture. Native C/Objective-C Blocks and C++ lambdas own these fields
independently of the source record. Borrowing through a copied strong field does
not require the original variable or record field to remain unchanged. Nested,
initialized, array and reference-variable value captures preserve this ownership.
A reference member still designates outside storage and supplies no independent
claim; weak, unsafe, byref and ordinary MRC fields retain their boundaries.

Capture ownership follows native record/base/array paths and canonical field
lifetime qualifiers. A by-value capture of a reference variable copies its value;
a reference encountered inside that copied value stops the ownership proof.
Borrowed future uses of a proven owned field need no external dependency. A field
with a consumed-owner requirement retains that distinct effect. An independently
copied strong field can also protect an existing interior loan when the known
owner identity agrees; an unrelated may-owner set cannot prove that equality.

Reference-field inputs now receive distinct symbolic pointee owner identities.
This preserves future requirements even when the first body check precedes
capture initialization. Concrete reference bindings and inherited owner facts
replace those symbolic inputs in the enclosing analysis. Named reference-bearing
captures keep their existing explicit invocation-contract boundary.

Eighteen maintained source/PCH commands cover C17/C++20/C++23 under ARC/MRC,
strong/raw/weak/reference fields, nested and array paths, existing loans,
initialized captures, copied reference-variable values and returned C++ closures.
The native explicit-capture witness now counts exact object destruction across
heap Block copies and independent C++ closure copies after the source owner and
record are cleared. Four ARC C17 O0/C++23 O2 source/PCH configurations pass all
fourteen compile/PCH/link/execution commands with ASan/UBSan instrumentation.
Source-free frontend replay publishes the owned-record/loan capture body.
All thirty-one borrowed compiler regressions and three affected native frontend
groups pass, including eighteen new source/PCH commands.

Extension ABI 24, physical runtime ABI 4 and accepted SDK 2.62 remain unchanged.
Mixed/manual ownership counts, ambiguous owner correlations, arbitrary mutable
or lazy captures, indirect/global mutation and exceptional ownership remain open.
The focused native run is development evidence; matched SDK qualification and
archive delivery follow consolidated verification.
