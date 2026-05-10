  void ApplyBlockLiteralStorageOwnershipAnnotations(Expr &block) {
    const BlockLiteralSourceUseKind source_use_kind =
        CurrentBlockLiteralSourceUseKind();
    ApplyBlockLiteralStorageOwnershipSourceModel(block, source_use_kind);
    BuildBlockLiteralStorageOwnershipProfiles(block, source_use_kind);
    ApplyBlockLiteralStorageOwnershipSlotModel(block);
    PublishBlockLiteralStorageOwnershipSurface(block);
    FinalizeBlockLiteralCopyDisposeOwnership(block);
  }
