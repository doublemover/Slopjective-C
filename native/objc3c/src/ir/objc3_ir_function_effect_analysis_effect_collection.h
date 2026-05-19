#pragma once

struct Objc3IRFunctionEffectAnalysis;
struct Objc3IRFunctionEffectAnalysisOptions;

void CollectObjc3IRFunctionEffects(
    const Objc3IRFunctionEffectAnalysisOptions &options,
    Objc3IRFunctionEffectAnalysis &analysis);
