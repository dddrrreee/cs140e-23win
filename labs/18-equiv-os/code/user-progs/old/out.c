#include "prog.h"
struct prog prog_vec = {
    .name = "prog-vec.bin",
    .nbytes = 664,
    .data = {
		0x78,	0x56,	0x34,	0x12,	0x10,	0x0,	0x0,	0x0,	
		0x0,	0x0,	0x30,	0x0,	0x88,	0x2,	0x0,	0x0,	
		0x76,	0x0,	0x0,	0xea,	0x0,	0x10,	0x80,	0xe5,	
		0x1e,	0xff,	0x2f,	0xe1,	0x0,	0x0,	0x90,	0xe5,	
		0x1e,	0xff,	0x2f,	0xe1,	0xf,	0x0,	0x2d,	0xe9,	
		0x30,	0x40,	0x2d,	0xe9,	0xc,	0xd0,	0x4d,	0xe2,	
		0x18,	0x30,	0x9d,	0xe5,	0x1c,	0x20,	0x8d,	0xe2,	
		0x4,	0x20,	0x8d,	0xe5,	0x17,	0x0,	0x0,	0xea,	
		0x4,	0x30,	0x9d,	0xe5,	0x4,	0x20,	0x83,	0xe2,	
		0x4,	0x20,	0x8d,	0xe5,	0x0,	0x10,	0x93,	0xe5,	
		0x4,	0x0,	0xa0,	0xe3,	0x74,	0x0,	0x0,	0xeb,	
		0x5,	0x40,	0xa0,	0xe1,	0x4,	0x20,	0xa0,	0xe1,	
		0x0,	0x31,	0x9f,	0xe5,	0x3,	0x0,	0xa0,	0xe3,	
		0x0,	0x10,	0xa0,	0xe1,	0x0,	0x0,	0x50,	0xe3,	
		0x1,	0x0,	0x40,	0xe2,	0x4,	0x0,	0x0,	0xa,	
		0x1,	0x10,	0xd2,	0xe4,	0x1,	0xc0,	0xd3,	0xe4,	
		0xc,	0x0,	0x51,	0xe1,	0xf7,	0xff,	0xff,	0xa,	
		0xc,	0x10,	0x41,	0xe0,	0x0,	0x0,	0x51,	0xe3,	
		0x25,	0x0,	0x0,	0x1a,	0x5,	0x0,	0xa0,	0xe3,	
		0x63,	0x0,	0x0,	0xeb,	0x3,	0x30,	0x84,	0xe2,	
		0x3,	0x40,	0xa0,	0xe1,	0x1,	0x10,	0xd4,	0xe4,	
		0x0,	0x0,	0x51,	0xe3,	0x29,	0x0,	0x0,	0xa,	
		0x24,	0x0,	0x51,	0xe3,	0xe8,	0xff,	0xff,	0xa,	
		0x25,	0x0,	0x51,	0xe3,	0x21,	0x0,	0x0,	0x1a,	
		0x2,	0x50,	0x83,	0xe2,	0x0,	0x30,	0xd4,	0xe5,	
		0x64,	0x0,	0x53,	0xe3,	0xdb,	0xff,	0xff,	0xa,	
		0x78,	0x0,	0x53,	0xe3,	0x12,	0x0,	0x0,	0x1a,	
		0x4,	0x30,	0x9d,	0xe5,	0x4,	0x20,	0x83,	0xe2,	
		0x4,	0x20,	0x8d,	0xe5,	0x0,	0x10,	0x93,	0xe5,	
		0x3,	0x0,	0xa0,	0xe3,	0x4e,	0x0,	0x0,	0xeb,	
		0x5,	0x40,	0xa0,	0xe1,	0xd8,	0xff,	0xff,	0xea,	
		0x1,	0x40,	0x84,	0xe2,	0x1,	0x0,	0xa0,	0xe3,	
		0x49,	0x0,	0x0,	0xeb,	0x0,	0x10,	0xd4,	0xe5,	
		0x0,	0x0,	0x51,	0xe3,	0xf9,	0xff,	0xff,	0x1a,	
		0x1,	0x10,	0xa0,	0xe3,	0x0,	0x0,	0xa0,	0xe3,	
		0x43,	0x0,	0x0,	0xeb,	0x5,	0x40,	0xa0,	0xe1,	
		0xcd,	0xff,	0xff,	0xea,	0x40,	0x40,	0x9f,	0xe5,	
		0xf5,	0xff,	0xff,	0xea,	0x4,	0x10,	0xa0,	0xe1,	
		0x38,	0x0,	0x9f,	0xe5,	0xba,	0xff,	0xff,	0xeb,	
		0x1,	0x10,	0xa0,	0xe3,	0x0,	0x0,	0xa0,	0xe3,	
		0x39,	0x0,	0x0,	0xeb,	0xd2,	0xff,	0xff,	0xea,	
		0x1,	0x0,	0xa0,	0xe3,	0x36,	0x0,	0x0,	0xeb,	
		0x4,	0x30,	0xa0,	0xe1,	0xd1,	0xff,	0xff,	0xea,	
		0xc,	0xd0,	0x8d,	0xe2,	0x30,	0x40,	0xbd,	0xe8,	
		0x10,	0xd0,	0x8d,	0xe2,	0x1e,	0xff,	0x2f,	0xe1,	
		0x4c,	0x2,	0x30,	0x0,	0x3c,	0x2,	0x30,	0x0,	
		0x50,	0x2,	0x30,	0x0,	0x70,	0x40,	0x2d,	0xe9,	
		0x0,	0x10,	0xa0,	0xe3,	0x6,	0x0,	0xa0,	0xe3,	
		0x29,	0x0,	0x0,	0xeb,	0x0,	0x40,	0xa0,	0xe1,	
		0x0,	0x10,	0xa0,	0xe1,	0x54,	0x0,	0x9f,	0xe5,	
		0xa3,	0xff,	0xff,	0xeb,	0x0,	0x30,	0xa0,	0xe3,	
		0x1,	0x0,	0x0,	0xea,	0x3,	0x31,	0x84,	0xe7,	
		0x1,	0x30,	0x83,	0xe2,	0xf,	0x0,	0x53,	0xe3,	
		0xfb,	0xff,	0xff,	0xda,	0x0,	0x30,	0xa0,	0xe3,	
		0x1,	0x50,	0xa0,	0xe3,	0x2,	0x0,	0x0,	0xea,	
		0x3,	0x21,	0x94,	0xe7,	0x92,	0x5,	0x5,	0xe0,	
		0x1,	0x30,	0x83,	0xe2,	0xf,	0x0,	0x53,	0xe3,	
		0xfa,	0xff,	0xff,	0xda,	0x5,	0x10,	0xa0,	0xe1,	
		0x14,	0x0,	0x9f,	0xe5,	0x92,	0xff,	0xff,	0xeb,	
		0x5,	0x10,	0xa0,	0xe1,	0x0,	0x0,	0xa0,	0xe3,	
		0x11,	0x0,	0x0,	0xeb,	0x70,	0x80,	0xbd,	0xe8,	
		0x68,	0x2,	0x30,	0x0,	0x84,	0x2,	0x30,	0x0,	
		0x10,	0x40,	0x2d,	0xe9,	0x28,	0x30,	0x9f,	0xe5,	
		0x1,	0x0,	0x0,	0xea,	0x0,	0x20,	0xa0,	0xe3,	
		0x4,	0x20,	0x83,	0xe4,	0x1c,	0x20,	0x9f,	0xe5,	
		0x2,	0x0,	0x53,	0xe1,	0xfa,	0xff,	0xff,	0x3a,	
		0xd7,	0xff,	0xff,	0xeb,	0x0,	0x10,	0xa0,	0xe3,	
		0x1,	0x0,	0xa0,	0xe1,	0x2,	0x0,	0x0,	0xeb,	
		0x10,	0x80,	0xbd,	0xe8,	0x98,	0x2,	0x30,	0x0,	
		0x98,	0x2,	0x30,	0x0,	0x4,	0xe0,	0x2d,	0xe5,	
		0x0,	0x0,	0x0,	0xef,	0x4,	0xe0,	0x9d,	0xe4,	
		0x1e,	0xff,	0x2f,	0xe1,	0x62,	0x61,	0x64,	0x20,	
		0x63,	0x68,	0x61,	0x72,	0x61,	0x63,	0x74,	0x65,	
		0x72,	0xa,	0x0,	0x0,	0x70,	0x69,	0x64,	0x0,	
		0x75,	0x6e,	0x6b,	0x6e,	0x6f,	0x77,	0x6e,	0x20,	
		0x66,	0x6f,	0x72,	0x6d,	0x61,	0x74,	0x3a,	0x20,	
		0x3c,	0x25,	0x73,	0x3e,	0xa,	0x0,	0x0,	0x0,	
		0x73,	0x62,	0x72,	0x6b,	0x20,	0x67,	0x69,	0x76,	
		0x65,	0x73,	0x3a,	0x20,	0x25,	0x70,	0x20,	0x66,	
		0x6f,	0x72,	0x20,	0x24,	0x70,	0x69,	0x64,	0xa,	
		0x0,	0x0,	0x0,	0x0,	0x70,	0x72,	0x6f,	0x64,	
		0x3d,	0x25,	0x78,	0x20,	0x66,	0x6f,	0x72,	0x20,	
		0x24,	0x70,	0x69,	0x64,	0xa,	0x0,	0x0,	0x0,	    }
};