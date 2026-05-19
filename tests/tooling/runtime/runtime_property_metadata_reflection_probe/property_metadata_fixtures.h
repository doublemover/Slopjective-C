#pragma once

namespace objc3c::runtime::probe::runtime_property_metadata_reflection {

struct PropertyMetadataQuery {
  const char *class_name;
  const char *property_name;
};

inline constexpr const char *kWidgetClassName = "Widget";
inline constexpr const char *kMissingWidgetClassName = "MissingWidget";

inline constexpr PropertyMetadataQuery kTokenPropertyQuery{
    kWidgetClassName, "token"};
inline constexpr PropertyMetadataQuery kValuePropertyQuery{
    kWidgetClassName, "value"};
inline constexpr PropertyMetadataQuery kCountPropertyQuery{
    kWidgetClassName, "count"};
inline constexpr PropertyMetadataQuery kMissingPropertyQuery{
    kWidgetClassName, "missing"};
inline constexpr PropertyMetadataQuery kMissingClassPropertyQuery{
    kMissingWidgetClassName, "count"};

}  // namespace objc3c::runtime::probe::runtime_property_metadata_reflection
