typedef unsigned u32;
typedef unsigned char u8;

// load 32-bits at byte offset 16
u32 ld32_off(u32 *ptr) {
  return ptr[16/4];
}
// load 32-bits at byte offset <off>
u32 ld32_reg(u8 *ptr, u32 off) {
  return *(u32*)(ptr+off);
}
// load 8 bits at offset 16
u8 ld8_off(u8 *ptr) {
  return ptr[16];
}
// load 8-bits at byte offset <off>
u8 ld8_reg(u8 *ptr, u32 off) {
  return ptr[off];
}
