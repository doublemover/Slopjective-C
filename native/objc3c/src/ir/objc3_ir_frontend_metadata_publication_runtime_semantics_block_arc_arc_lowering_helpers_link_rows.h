#pragma once

#include <iosfwd>

void EmitObjc3IRArcAutomaticInsertionRuntimeLinkFields(
    std::ostringstream &out);
void EmitObjc3IRArcCleanupWeakLifetimeRuntimeLinkFields(
    std::ostringstream &out);
void EmitObjc3IRArcBlockAutoreleaseReturnRuntimeLinkFields(
    std::ostringstream &out);
