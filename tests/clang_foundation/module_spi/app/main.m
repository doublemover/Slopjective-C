#ifdef __OBJC__
@import fixture.SPITop;
#endif
int main(void) { return spi_answer() == 42 && spi_public() == 20 ? 0 : 1; }
