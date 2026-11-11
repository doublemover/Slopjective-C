# Native constructor and destructor subobject cleanup

System source 2.44 admits native constructor/destructor bodies to the owned
cleanup CFG. Constructors register only completed direct base/member subobjects;
an initializer failure destroys that prefix in reverse order after callee-owned
cleanup. A body failure runs local/deferred cleanup before the complete prefix.
Successful constructor return transfers a complete object without destroying it.

Destructors run body cleanup, then reverse member/base cleanup on fallthrough,
return and native exception paths. An exception from normal subobject destruction
continues through the remaining prefix; a second exception during unwind
terminates. Destructor return leaves body handlers before implicit member/base
cleanup, so those handlers cannot catch a later subobject destructor exception.
Base destructor elements expose their actual native declaration to CFG consumers.
Primitive ARC subobjects have nonthrowing private release operations. Registration
is bounded to 64 owned parts and 256 initializers.

Native Linux `noexcept` constructor/destructor entries terminate without unwinding
their frame when no body handler catches an exception. Exact native traces cover
initializer failure, body failure, normal member destruction, failed new-expression
initialization and delete-destructor failure. Callee-owned cleanup may complete
before its exception reaches the boundary; caller lexical/subobject cleanup and
failure deallocation are skipped. A handler inside the body still gets its normal
local unwind. These are the emitted native semantics, verified by child-process
termination witnesses rather than assumptions about exception propagation.

Source analysis can run before a callable's body is published on its declaration;
the builder retains the active body explicitly. Constructor prefix state is
restored before delayed goto/backpatch cleanup. Native finally transfer replacement
continues to use its existing cleanup and exception context.

Published constructor/destructor bodies require exact
`objc3.native-subobject-cleanup.v1` identity and reader admission. Publication
feature predicates retain their owning declaration through template/member
traversal, so class and nested published bodies keep matching source-free identity.
Private builder state and helpers preserve extension ABI 24 and runtime ABIs.

Direct nonvirtual bases, direct members, bounded array groups and primitive ARC
subobjects are admitted. Delegation, constructor/destructor function-try blocks,
virtual bases, indirect anonymous-member initialization, resource/Block subobject
recipes and full-expression initializer temporaries retain separate proof gates.
Wider ownership and final matched SDK delivery remain in the roadmap.

All 61 affected compiler cases, four new source-free profiles, eight adjacent
frontend groups, codec/isolation controls, 29 AST controls and twelve ASan/UBSan
native configurations / 42 commands pass.
