#ifdef __OBJC__
@import fixture.SPILeaf;
#endif
int spi_answer(void) { return spi_public() + spi_secret(); }
