#include "libos-prog.h"
struct prog prog_vec = {
    .name = "prog-vec.bin",
    .nbytes = 1276,
    .code = {
		0x78,	0x56,	0x34,	0x12,	0x14,	0x0,	0x0,	0x0,	
		0x0,	0x0,	0x30,	0x0,	0xe8,	0x4,	0x0,	0x0,	
		0xc,	0x0,	0x0,	0x0,	0xce,	0x0,	0x0,	0xea,	
		0x0,	0x10,	0x80,	0xe5,	0x1e,	0xff,	0x2f,	0xe1,	
		0x0,	0x0,	0x90,	0xe5,	0x1e,	0xff,	0x2f,	0xe1,	
		0xf,	0x0,	0x2d,	0xe9,	0x30,	0x40,	0x2d,	0xe9,	
		0xc,	0xd0,	0x4d,	0xe2,	0x18,	0x50,	0x9d,	0xe5,	
		0x1c,	0x30,	0x8d,	0xe2,	0x4,	0x30,	0x8d,	0xe5,	
		0x1d,	0x0,	0x0,	0xea,	0x4,	0x30,	0x9d,	0xe5,	
		0x4,	0x20,	0x83,	0xe2,	0x4,	0x20,	0x8d,	0xe5,	
		0x0,	0x10,	0x93,	0xe5,	0x4,	0x0,	0xa0,	0xe3,	
		0x8d,	0x0,	0x0,	0xeb,	0x16,	0x0,	0x0,	0xea,	
		0xe4,	0x0,	0x9f,	0xe5,	0xef,	0xff,	0xff,	0xeb,	
		0x1,	0x10,	0xa0,	0xe3,	0x0,	0x0,	0xa0,	0xe3,	
		0x87,	0x0,	0x0,	0xeb,	0x10,	0x0,	0x0,	0xea,	
		0x4,	0x20,	0xa0,	0xe1,	0xcc,	0x30,	0x9f,	0xe5,	
		0x3,	0x0,	0xa0,	0xe3,	0x0,	0x10,	0xa0,	0xe1,	
		0x0,	0x0,	0x50,	0xe3,	0x1,	0x0,	0x40,	0xe2,	
		0x4,	0x0,	0x0,	0xa,	0x1,	0x10,	0xd2,	0xe4,	
		0x1,	0xc0,	0xd3,	0xe4,	0xc,	0x0,	0x51,	0xe1,	
		0xf7,	0xff,	0xff,	0xa,	0xc,	0x10,	0x41,	0xe0,	
		0x0,	0x0,	0x51,	0xe3,	0x17,	0x0,	0x0,	0x1a,	
		0x5,	0x0,	0xa0,	0xe3,	0x76,	0x0,	0x0,	0xeb,	
		0x4,	0x50,	0x85,	0xe2,	0x5,	0x40,	0xa0,	0xe1,	
		0x1,	0x10,	0xd4,	0xe4,	0x0,	0x0,	0x51,	0xe3,	
		0x1b,	0x0,	0x0,	0xa,	0x24,	0x0,	0x51,	0xe3,	
		0xe8,	0xff,	0xff,	0xa,	0x25,	0x0,	0x51,	0xe3,	
		0x13,	0x0,	0x0,	0x1a,	0x2,	0x50,	0x85,	0xe2,	
		0x0,	0x30,	0xd4,	0xe5,	0x64,	0x0,	0x53,	0xe3,	
		0xd5,	0xff,	0xff,	0xa,	0x78,	0x0,	0x53,	0xe3,	
		0xda,	0xff,	0xff,	0x1a,	0x4,	0x30,	0x9d,	0xe5,	
		0x4,	0x20,	0x83,	0xe2,	0x4,	0x20,	0x8d,	0xe5,	
		0x0,	0x10,	0x93,	0xe5,	0x3,	0x0,	0xa0,	0xe3,	
		0x61,	0x0,	0x0,	0xeb,	0xea,	0xff,	0xff,	0xea,	
		0x4,	0x10,	0xa0,	0xe1,	0x38,	0x0,	0x9f,	0xe5,	
		0xc2,	0xff,	0xff,	0xeb,	0x1,	0x10,	0xa0,	0xe3,	
		0x0,	0x0,	0xa0,	0xe3,	0x5a,	0x0,	0x0,	0xeb,	
		0xe0,	0xff,	0xff,	0xea,	0x1,	0x0,	0xa0,	0xe3,	
		0x57,	0x0,	0x0,	0xeb,	0x4,	0x50,	0xa0,	0xe1,	
		0xdf,	0xff,	0xff,	0xea,	0xc,	0xd0,	0x8d,	0xe2,	
		0x30,	0x40,	0xbd,	0xe8,	0x10,	0xd0,	0x8d,	0xe2,	
		0x1e,	0xff,	0x2f,	0xe1,	0x30,	0x4,	0x30,	0x0,	
		0x2c,	0x4,	0x30,	0x0,	0x40,	0x4,	0x30,	0x0,	
		0x70,	0x40,	0x2d,	0xe9,	0x8,	0xd0,	0x4d,	0xe2,	
		0x40,	0x0,	0xa0,	0xe3,	0x4e,	0x0,	0x0,	0xeb,	
		0x0,	0x50,	0xa0,	0xe1,	0x0,	0x10,	0xa0,	0xe1,	
		0xf4,	0x0,	0x9f,	0xe5,	0xab,	0xff,	0xff,	0xeb,	
		0x0,	0x30,	0xa0,	0xe3,	0x1,	0x0,	0x0,	0xea,	
		0x3,	0x31,	0x85,	0xe7,	0x1,	0x30,	0x83,	0xe2,	
		0xf,	0x0,	0x53,	0xe3,	0xfb,	0xff,	0xff,	0xda,	
		0x0,	0x40,	0xa0,	0xe3,	0x4,	0x0,	0x0,	0xea,	
		0x6,	0x20,	0x95,	0xe7,	0x4,	0x10,	0xa0,	0xe1,	
		0xc8,	0x0,	0x9f,	0xe5,	0x9f,	0xff,	0xff,	0xeb,	
		0x1,	0x40,	0x84,	0xe2,	0xf,	0x0,	0x54,	0xe3,	
		0x9,	0x0,	0x0,	0xca,	0x4,	0x61,	0xa0,	0xe1,	
		0x4,	0x31,	0x95,	0xe7,	0x3,	0x0,	0x54,	0xe1,	
		0xf4,	0xff,	0xff,	0xa,	0xa8,	0x0,	0x9f,	0xe5,	
		0x96,	0xff,	0xff,	0xeb,	0x1,	0x10,	0xa0,	0xe3,	
		0x0,	0x0,	0xa0,	0xe3,	0x2e,	0x0,	0x0,	0xeb,	
		0xee,	0xff,	0xff,	0xea,	0x0,	0x40,	0xa0,	0xe3,	
		0x1,	0x60,	0xa0,	0xe3,	0x8,	0x0,	0x0,	0xea,	
		0x4,	0x31,	0x95,	0xe7,	0x4,	0x20,	0xa0,	0xe1,	
		0x6,	0x10,	0xa0,	0xe1,	0x7c,	0x0,	0x9f,	0xe5,	
		0x8a,	0xff,	0xff,	0xeb,	0x4,	0x31,	0x95,	0xe7,	
		0x1,	0x30,	0x83,	0xe2,	0x93,	0x6,	0x6,	0xe0,	
		0x1,	0x40,	0x84,	0xe2,	0xf,	0x0,	0x54,	0xe3,	
		0xf4,	0xff,	0xff,	0xda,	0x6,	0x10,	0xa0,	0xe1,	
		0x5c,	0x0,	0x9f,	0xe5,	0x81,	0xff,	0xff,	0xeb,	
		0x58,	0x30,	0x9f,	0xe5,	0x3,	0x0,	0x56,	0xe1,	
		0x4,	0x0,	0x0,	0x1a,	0x6,	0x10,	0xa0,	0xe1,	
		0x0,	0x0,	0xa0,	0xe3,	0x16,	0x0,	0x0,	0xeb,	
		0x8,	0xd0,	0x8d,	0xe2,	0x70,	0x80,	0xbd,	0xe8,	
		0x3c,	0x30,	0x9f,	0xe5,	0x0,	0x30,	0x8d,	0xe5,	
		0x1a,	0x30,	0xa0,	0xe3,	0x34,	0x20,	0x9f,	0xe5,	
		0x34,	0x10,	0x9f,	0xe5,	0x34,	0x0,	0x9f,	0xe5,	
		0x72,	0xff,	0xff,	0xeb,	0x1,	0x10,	0xa0,	0xe3,	
		0x0,	0x0,	0xa0,	0xe3,	0xa,	0x0,	0x0,	0xeb,	
		0xef,	0xff,	0xff,	0xea,	0x58,	0x4,	0x30,	0x0,	
		0x78,	0x4,	0x30,	0x0,	0x68,	0x4,	0x30,	0x0,	
		0x84,	0x4,	0x30,	0x0,	0x98,	0x4,	0x30,	0x0,	
		0x0,	0x80,	0x75,	0x77,	0xe0,	0x4,	0x30,	0x0,	
		0xf4,	0x4,	0x30,	0x0,	0xa4,	0x4,	0x30,	0x0,	
		0xb0,	0x4,	0x30,	0x0,	0x4,	0xe0,	0x2d,	0xe5,	
		0x0,	0x0,	0x0,	0xef,	0x4,	0xe0,	0x9d,	0xe4,	
		0x1e,	0xff,	0x2f,	0xe1,	0x70,	0x40,	0x2d,	0xe9,	
		0x0,	0x40,	0xa0,	0xe1,	0x98,	0x20,	0x9f,	0xe5,	
		0x0,	0x30,	0x92,	0xe5,	0x0,	0x30,	0x83,	0xe0,	
		0x4,	0x20,	0x92,	0xe5,	0x2,	0x0,	0x53,	0xe1,	
		0x14,	0x0,	0x0,	0x3a,	0x84,	0x30,	0x9f,	0xe5,	
		0x0,	0x30,	0x93,	0xe5,	0x0,	0x0,	0x53,	0xe1,	
		0x78,	0x30,	0x9f,	0x35,	0x0,	0x0,	0x83,	0x35,	
		0x70,	0x30,	0x9f,	0xe5,	0x0,	0x10,	0x93,	0xe5,	
		0x81,	0x10,	0xa0,	0xe1,	0x0,	0x10,	0x83,	0xe5,	
		0x6,	0x0,	0xa0,	0xe3,	0xe8,	0xff,	0xff,	0xeb,	
		0x54,	0x30,	0x9f,	0xe5,	0x0,	0x30,	0x93,	0xe5,	
		0x0,	0x0,	0x53,	0xe3,	0xf,	0x0,	0x0,	0xa,	
		0x44,	0x20,	0x9f,	0xe5,	0x0,	0x30,	0x92,	0xe5,	
		0x40,	0x10,	0x9f,	0xe5,	0x0,	0x10,	0x91,	0xe5,	
		0x1,	0x30,	0x83,	0xe0,	0x4,	0x30,	0x82,	0xe5,	
		0x2c,	0x30,	0x9f,	0xe5,	0x0,	0x50,	0x93,	0xe5,	
		0x4,	0x20,	0x85,	0xe0,	0x0,	0x20,	0x83,	0xe5,	
		0x4,	0x20,	0xa0,	0xe1,	0x0,	0x10,	0xa0,	0xe3,	
		0x5,	0x0,	0xa0,	0xe1,	0x15,	0x0,	0x0,	0xeb,	
		0x5,	0x0,	0xa0,	0xe1,	0x70,	0x80,	0xbd,	0xe8,	
		0x4,	0x30,	0x9f,	0xe5,	0x0,	0x0,	0x83,	0xe5,	
		0xec,	0xff,	0xff,	0xea,	0xfc,	0x4,	0x30,	0x0,	
		0x28,	0x4,	0x30,	0x0,	0x10,	0x40,	0x2d,	0xe9,	
		0x28,	0x30,	0x9f,	0xe5,	0x1,	0x0,	0x0,	0xea,	
		0x0,	0x20,	0xa0,	0xe3,	0x4,	0x20,	0x83,	0xe4,	
		0x1c,	0x20,	0x9f,	0xe5,	0x2,	0x0,	0x53,	0xe1,	
		0xfa,	0xff,	0xff,	0x3a,	0x77,	0xff,	0xff,	0xeb,	
		0x0,	0x10,	0xa0,	0xe3,	0x1,	0x0,	0xa0,	0xe1,	
		0xc3,	0xff,	0xff,	0xeb,	0x10,	0x80,	0xbd,	0xe8,	
		0xfc,	0x4,	0x30,	0x0,	0x8,	0x5,	0x30,	0x0,	
		0x0,	0x0,	0x52,	0xe3,	0x1e,	0xff,	0x2f,	0x1,	
		0x0,	0x0,	0x51,	0xe3,	0x7,	0x0,	0x0,	0x1a,	
		0x7,	0x0,	0x10,	0xe3,	0x1,	0x0,	0x0,	0x1a,	
		0x7,	0x0,	0x12,	0xe3,	0x5,	0x0,	0x0,	0xa,	
		0x3,	0x0,	0x10,	0xe3,	0x1,	0x0,	0x0,	0x1a,	
		0x3,	0x0,	0x12,	0xe3,	0xd,	0x0,	0x0,	0xa,	
		0x2,	0x20,	0x80,	0xe0,	0x14,	0x0,	0x0,	0xea,	
		0xa2,	0x21,	0xa0,	0xe1,	0x5,	0x0,	0x0,	0xea,	
		0x0,	0x30,	0xa0,	0xe1,	0xc1,	0x2f,	0xa0,	0xe1,	
		0x8,	0x10,	0x83,	0xe4,	0x4,	0x20,	0x80,	0xe5,	
		0x3,	0x0,	0xa0,	0xe1,	0xc,	0x20,	0xa0,	0xe1,	
		0x1,	0xc0,	0x42,	0xe2,	0x0,	0x0,	0x52,	0xe3,	
		0xf6,	0xff,	0xff,	0x1a,	0x1e,	0xff,	0x2f,	0xe1,	
		0x22,	0x21,	0xa0,	0xe1,	0x1,	0x0,	0x0,	0xea,	
		0x4,	0x10,	0x80,	0xe4,	0x3,	0x20,	0xa0,	0xe1,	
		0x1,	0x30,	0x42,	0xe2,	0x0,	0x0,	0x52,	0xe3,	
		0xfa,	0xff,	0xff,	0x1a,	0x1e,	0xff,	0x2f,	0xe1,	
		0x1,	0x10,	0xc0,	0xe4,	0x2,	0x0,	0x50,	0xe1,	
		0xfc,	0xff,	0xff,	0x3a,	0x1e,	0xff,	0x2f,	0xe1,	
		0x0,	0x4,	0x0,	0x0,	0x70,	0x69,	0x64,	0x0,	
		0x62,	0x61,	0x64,	0x20,	0x63,	0x68,	0x61,	0x72,	
		0x61,	0x63,	0x74,	0x65,	0x72,	0xa,	0x0,	0x0,	
		0x75,	0x6e,	0x6b,	0x6e,	0x6f,	0x77,	0x6e,	0x20,	
		0x66,	0x6f,	0x72,	0x6d,	0x61,	0x74,	0x3a,	0x20,	
		0x3c,	0x25,	0x73,	0x3e,	0xa,	0x0,	0x0,	0x0,	
		0x73,	0x62,	0x72,	0x6b,	0x20,	0x67,	0x69,	0x76,	
		0x65,	0x73,	0x3a,	0x20,	0x25,	0x78,	0xa,	0x0,	
		0x6e,	0x6f,	0x74,	0x20,	0x65,	0x71,	0x75,	0x61,	
		0x6c,	0x3f,	0x3f,	0xa,	0x0,	0x0,	0x0,	0x0,	
		0x76,	0x5b,	0x25,	0x64,	0x5d,	0x3d,	0x25,	0x64,	
		0xa,	0x0,	0x0,	0x0,	0x70,	0x72,	0x6f,	0x64,	
		0x3d,	0x25,	0x64,	0x3a,	0x20,	0x76,	0x5b,	0x25,	
		0x64,	0x5d,	0x3d,	0x25,	0x64,	0xa,	0x0,	0x0,	
		0x70,	0x72,	0x6f,	0x64,	0x3d,	0x25,	0x78,	0xa,	
		0x0,	0x0,	0x0,	0x0,	0x70,	0x72,	0x6f,	0x67,	
		0x2d,	0x76,	0x65,	0x63,	0x2e,	0x63,	0x0,	0x0,	
		0x25,	0x73,	0x3a,	0x25,	0x73,	0x3a,	0x25,	0x64,	
		0x3a,	0x44,	0x45,	0x42,	0x55,	0x47,	0x3a,	0x45,	
		0x52,	0x52,	0x4f,	0x52,	0x3a,	0x20,	0x41,	0x73,	
		0x73,	0x65,	0x72,	0x74,	0x69,	0x6f,	0x6e,	0x20,	
		0x60,	0x25,	0x73,	0x60,	0x20,	0x66,	0x61,	0x69,	
		0x6c,	0x65,	0x64,	0x2e,	0xa,	0x0,	0x0,	0x0,	
		0x70,	0x72,	0x6f,	0x64,	0x20,	0x3d,	0x3d,	0x20,	
		0x30,	0x78,	0x37,	0x37,	0x37,	0x35,	0x38,	0x30,	
		0x30,	0x30,	0x0,	0x0,	0x6e,	0x6f,	0x74,	0x6d,	
		0x61,	0x69,	0x6e,	0x0,	    }
};
