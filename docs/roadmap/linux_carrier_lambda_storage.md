# Native C++ carrier lambda captures

Source 2.140 / System 2.66 extends native selected-payload construction recipes to
C++ lambda captures. Pure C++ and mixed ARC/C++ carrier values and fixed arrays
retain real copy/move, access/deleted and exception behavior. Generated closure
special members reuse those recipes, including ordinary, generic and dependent
lambda instantiation. Captures do not become primitive byte copies.

A failed later capture or array element destroys the completed prefix. Controls
cover explicit copy/move init-captures, implicit value captures, closure copies and
moves, mutable/generic calls, heap closures, closure captures in heap Blocks,
strong/weak arms, and throwing constructor/default-argument cleanup at each native
array payload position. Borrowed captures and mutation analysis retain their
separate lifetime requirements; MRC mixed ownership remains unqualified.

Published bodies require `objc3.native-carrier-lambda.v1`, including dependent
value captures which may instantiate a carrier. Missing, invented and older-reader
capabilities reject. C++20/23 source-free replay and native AST import controls
retain the constructor recipes. The [verification receipt](../support/linux_carrier_lambda_storage_progress.json)
records completed checks: 49 dedicated compiler commands, all 57 AST controls,
two source-free profiles, 500 production decoder replays and three native
ASan/UBSan configurations / ten commands pass. The 31 updated frontend groups
are being consolidated. Full current compiler/analysis consolidation and one
matched SDK follow the implementation batch; ABI 24/frame 1/services 4/schema 2.2
retain their layouts.
