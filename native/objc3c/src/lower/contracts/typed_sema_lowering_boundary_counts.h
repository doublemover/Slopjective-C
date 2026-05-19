#pragma once

#include "lower/contracts/typed_sema_lowering_boundary.h"

void Objc3PopulateTypedSemaToLoweringBoundaryCounts(
    const Objc3Program &program,
    Objc3TypedSemaToLoweringBoundary &boundary);
