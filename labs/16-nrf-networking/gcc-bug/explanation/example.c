struct inner_bytes {
  char y;
  char z;
};

struct outer_bytes {
  char x;
  struct inner_bytes inner;
};

void foo(struct outer_bytes *a) { a->inner = (struct inner_bytes){0}; }

struct inner_short {
  short y;
};

struct outer_short {
  char x;
  struct inner_short inner;
};

void bar(struct outer_short *a) { a->inner = (struct inner_short){0}; }
