#pragma once

#include <iosfwd>

void BeginObjc3IRArcLoweringHelperMetadataNode(const char *metadata_node_id,
                                               const char *first_field,
                                               std::ostringstream &out);
void EmitObjc3IRArcLoweringHelperStringField(const char *field_value,
                                             std::ostringstream &out);
void EndObjc3IRArcLoweringHelperMetadataNode(std::ostringstream &out);
