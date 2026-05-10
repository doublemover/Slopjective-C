  std::size_t AlignObjcIvarLayoutOffset(std::size_t offset,
                                        std::size_t alignment) const {
    const std::size_t effective_alignment =
        std::max<std::size_t>(alignment, 1u);
    const std::size_t remainder = offset % effective_alignment;
    return remainder == 0u ? offset
                           : offset + (effective_alignment - remainder);
  }

  bool IsValidObjcIvarLayoutAlignment(std::size_t alignment) const {
    return alignment != 0u && (alignment & (alignment - 1u)) == 0u;
  }
