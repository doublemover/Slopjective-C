#pragma once

#include "pipeline/objc3_frontend_types.h"

bool IsClassSourceRecordLess(const Objc3RuntimeMetadataClassSourceRecord &lhs,
                             const Objc3RuntimeMetadataClassSourceRecord &rhs);
bool IsProtocolSourceRecordLess(
    const Objc3RuntimeMetadataProtocolSourceRecord &lhs,
    const Objc3RuntimeMetadataProtocolSourceRecord &rhs);
bool IsCategorySourceRecordLess(
    const Objc3RuntimeMetadataCategorySourceRecord &lhs,
    const Objc3RuntimeMetadataCategorySourceRecord &rhs);
bool IsPropertySourceRecordLess(
    const Objc3RuntimeMetadataPropertySourceRecord &lhs,
    const Objc3RuntimeMetadataPropertySourceRecord &rhs);
bool IsMethodSourceRecordLess(const Objc3RuntimeMetadataMethodSourceRecord &lhs,
                              const Objc3RuntimeMetadataMethodSourceRecord &rhs);
bool IsIvarSourceRecordLess(const Objc3RuntimeMetadataIvarSourceRecord &lhs,
                            const Objc3RuntimeMetadataIvarSourceRecord &rhs);

bool IsExecutableMetadataInterfaceNodeLess(
    const Objc3ExecutableMetadataInterfaceGraphNode &lhs,
    const Objc3ExecutableMetadataInterfaceGraphNode &rhs);
bool IsExecutableMetadataImplementationNodeLess(
    const Objc3ExecutableMetadataImplementationGraphNode &lhs,
    const Objc3ExecutableMetadataImplementationGraphNode &rhs);
bool IsExecutableMetadataClassNodeLess(
    const Objc3ExecutableMetadataClassGraphNode &lhs,
    const Objc3ExecutableMetadataClassGraphNode &rhs);
bool IsExecutableMetadataMetaclassNodeLess(
    const Objc3ExecutableMetadataMetaclassGraphNode &lhs,
    const Objc3ExecutableMetadataMetaclassGraphNode &rhs);
bool IsExecutableMetadataProtocolNodeLess(
    const Objc3ExecutableMetadataProtocolGraphNode &lhs,
    const Objc3ExecutableMetadataProtocolGraphNode &rhs);
bool IsExecutableMetadataCategoryNodeLess(
    const Objc3ExecutableMetadataCategoryGraphNode &lhs,
    const Objc3ExecutableMetadataCategoryGraphNode &rhs);
bool IsExecutableMetadataPropertyNodeLess(
    const Objc3ExecutableMetadataPropertyGraphNode &lhs,
    const Objc3ExecutableMetadataPropertyGraphNode &rhs);
bool IsExecutableMetadataMethodNodeLess(
    const Objc3ExecutableMetadataMethodGraphNode &lhs,
    const Objc3ExecutableMetadataMethodGraphNode &rhs);
bool IsExecutableMetadataIvarNodeLess(
    const Objc3ExecutableMetadataIvarGraphNode &lhs,
    const Objc3ExecutableMetadataIvarGraphNode &rhs);
bool IsExecutableMetadataGraphEdgeLess(
    const Objc3ExecutableMetadataGraphEdge &lhs,
    const Objc3ExecutableMetadataGraphEdge &rhs);
