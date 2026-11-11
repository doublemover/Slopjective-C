# Array disposal and failed construction

System source 2.41 extends native exceptional cleanup to nontrivial C++ array
disposal and ordinary scalar construction failure. A failed scalar constructor
deallocates only the storage already allocated, before enclosing cleanup; an
allocation failure never runs that deallocator. The incomplete whole object is
not destroyed. The constructor call includes its own native member/base unwind
before returning an exception to its caller.

Array disposal separates null, empty and nonempty arrays. The existing array
destructor group includes reverse destruction and completion of the remaining
element prefix after a first exception. It then deallocates before propagating
that exception. A second escaping element exception terminates before array
deallocation. Ordinary new/delete syntax activates this model without requiring
a separate explicit lifetime statement. Empty arrays still deallocate, while skipping even a noreturn
element destructor. The operand evaluates once throughout these paths.

Constant one-dimensional array construction is admitted for at most 64 elements
with nonthrowing, repeatable initialization and proved target-size bounds. Zero
elements skip initialization. Implicit array fillers participate in both the
throwing and repeatability checks; an omitted filler must not hide a throwing
default constructor. Scalar failure registration admits ordinary allocation and
primitive constructor arguments. Placement/nested allocation, managed argument
temporaries, dynamic/multidimensional arrays and wider partial element
construction retain explicit proof gates.

Strict System rejects scalar or array deletion of borrowed storage: a loan does
not transfer deallocation ownership. Private CFG cleanup registration and the
array nonempty predicate add no public AST layout, extension ABI or native
service/frame change. Physical extension ABI 24 remains unchanged.

Publication admits compiler-implicit replaceable global allocation declarations;
explicit replacements retain ordinary public dependency checks. Published
new/delete bodies require exact
`objc3.native-allocation-cleanup.v1` and `objc3.native-disposal.v2` admission.
The disposal capability makes a hard cutover from v1. All codec, artifact, module
and import readers require matching canonical identities.

Twenty-two AST controls pass, including complete allocation/constructor failure
and null/empty/normal/caught/termination path checks. Twenty-four affected new
compiler commands cover C++20/23 ARC/MRC source/PCH, bounded arrays, throwing
fillers, expired consumers and borrowed deletion. Native smoke witnesses verify
member cleanup before matched free, no incomplete whole-object destructor,
reverse array destruction, exact allocation/free counts and controlled secondary
exception termination. All 79 affected compiler cases, four allocation and four
disposal source-free profiles, six adjacent frontend groups, codec/isolation
controls, all 22 AST controls and twelve ASan/UBSan native configurations /
42 commands pass after the final compiler rebuild.

Wider constructor/destructor bodies, placement and dynamic array construction,
managed argument/element prefixes, base/union ownership, open callable and global
precision, complete profiles and final matched SDK delivery remain on the roadmap.
