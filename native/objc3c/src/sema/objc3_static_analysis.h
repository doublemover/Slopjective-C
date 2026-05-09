#pragma once

#include <memory>
#include <vector>

#include "sema/objc3_static_scalar_analysis.h"

bool StatementAlwaysReturns(const Stmt *stmt, const StaticScalarBindings *bindings = nullptr);
bool BlockAlwaysReturns(const std::vector<std::unique_ptr<Stmt>> &statements,
                        const StaticScalarBindings *bindings = nullptr);
