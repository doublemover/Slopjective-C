static void PublishUnsupportedFeatureClaimFamilyCounts(
    Objc3UnsupportedFeatureClaimEnforcementStats &stats) {
  if (stats.throws_source_rejection_site_count > 0u) {
    ++stats.live_unsupported_feature_family_count;
  }
  if (stats.blocks_source_rejection_site_count > 0u) {
    ++stats.live_unsupported_feature_family_count;
  }
  if (stats.arc_source_rejection_site_count > 0u) {
    ++stats.live_unsupported_feature_family_count;
  }
  const std::size_t categorized_unsupported_site_count =
      stats.throws_source_rejection_site_count +
      stats.blocks_source_rejection_site_count +
      stats.arc_source_rejection_site_count;
  if (stats.live_unsupported_feature_site_count >
      categorized_unsupported_site_count) {
    ++stats.live_unsupported_feature_family_count;
  }
}
