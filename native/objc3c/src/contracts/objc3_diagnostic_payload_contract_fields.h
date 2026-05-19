#pragma once

#include <string_view>

inline constexpr std::string_view kObjc3DiagnosticPayloadSeverityField =
    "severity";
inline constexpr std::string_view kObjc3DiagnosticPayloadLineField = "line";
inline constexpr std::string_view kObjc3DiagnosticPayloadColumnField =
    "column";
inline constexpr std::string_view kObjc3DiagnosticPayloadCodeField = "code";
inline constexpr std::string_view kObjc3DiagnosticPayloadMessageField =
    "message";
