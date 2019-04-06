#include "cmap.h"

const Cmap &cmap(CmapName name)
{
	static const Cmap cmap_grey {
		0x000000,
		0x010101,
		0x020202,
		0x030303,
		0x040404,
		0x050505,
		0x060606,
		0x070707,
		0x080808,
		0x090909,
		0x0a0a0a,
		0x0b0b0b,
		0x0c0c0c,
		0x0d0d0d,
		0x0e0e0e,
		0x0f0f0f,
		0x101010,
		0x111111,
		0x121212,
		0x131313,
		0x141414,
		0x151515,
		0x161616,
		0x171717,
		0x181818,
		0x191919,
		0x1a1a1a,
		0x1b1b1b,
		0x1c1c1c,
		0x1d1d1d,
		0x1e1e1e,
		0x1f1f1f,
		0x202020,
		0x212121,
		0x222222,
		0x232323,
		0x242424,
		0x252525,
		0x262626,
		0x272727,
		0x282828,
		0x292929,
		0x2a2a2a,
		0x2b2b2b,
		0x2c2c2c,
		0x2d2d2d,
		0x2e2e2e,
		0x2f2f2f,
		0x303030,
		0x313131,
		0x323232,
		0x333333,
		0x343434,
		0x353535,
		0x363636,
		0x373737,
		0x383838,
		0x393939,
		0x3a3a3a,
		0x3b3b3b,
		0x3c3c3c,
		0x3d3d3d,
		0x3e3e3e,
		0x3f3f3f,
		0x404040,
		0x414141,
		0x424242,
		0x434343,
		0x444444,
		0x454545,
		0x464646,
		0x474747,
		0x484848,
		0x494949,
		0x4a4a4a,
		0x4b4b4b,
		0x4c4c4c,
		0x4d4d4d,
		0x4e4e4e,
		0x4f4f4f,
		0x505050,
		0x515151,
		0x525252,
		0x535353,
		0x545454,
		0x555555,
		0x565656,
		0x575757,
		0x585858,
		0x595959,
		0x5a5a5a,
		0x5b5b5b,
		0x5c5c5c,
		0x5d5d5d,
		0x5e5e5e,
		0x5f5f5f,
		0x606060,
		0x616161,
		0x626262,
		0x636363,
		0x646464,
		0x656565,
		0x666666,
		0x676767,
		0x686868,
		0x696969,
		0x6a6a6a,
		0x6b6b6b,
		0x6c6c6c,
		0x6d6d6d,
		0x6e6e6e,
		0x6f6f6f,
		0x707070,
		0x717171,
		0x727272,
		0x737373,
		0x747474,
		0x757575,
		0x767676,
		0x777777,
		0x787878,
		0x797979,
		0x7a7a7a,
		0x7b7b7b,
		0x7c7c7c,
		0x7d7d7d,
		0x7e7e7e,
		0x7f7f7f,
		0x808080,
		0x818181,
		0x828282,
		0x838383,
		0x848484,
		0x858585,
		0x868686,
		0x878787,
		0x888888,
		0x898989,
		0x8a8a8a,
		0x8b8b8b,
		0x8c8c8c,
		0x8d8d8d,
		0x8e8e8e,
		0x8f8f8f,
		0x909090,
		0x919191,
		0x929292,
		0x939393,
		0x949494,
		0x959595,
		0x969696,
		0x979797,
		0x989898,
		0x999999,
		0x9a9a9a,
		0x9b9b9b,
		0x9c9c9c,
		0x9d9d9d,
		0x9e9e9e,
		0x9f9f9f,
		0xa0a0a0,
		0xa1a1a1,
		0xa2a2a2,
		0xa3a3a3,
		0xa4a4a4,
		0xa5a5a5,
		0xa6a6a6,
		0xa7a7a7,
		0xa8a8a8,
		0xa9a9a9,
		0xaaaaaa,
		0xababab,
		0xacacac,
		0xadadad,
		0xaeaeae,
		0xafafaf,
		0xb0b0b0,
		0xb1b1b1,
		0xb2b2b2,
		0xb3b3b3,
		0xb4b4b4,
		0xb5b5b5,
		0xb6b6b6,
		0xb7b7b7,
		0xb8b8b8,
		0xb9b9b9,
		0xbababa,
		0xbbbbbb,
		0xbcbcbc,
		0xbdbdbd,
		0xbebebe,
		0xbfbfbf,
		0xc0c0c0,
		0xc1c1c1,
		0xc2c2c2,
		0xc3c3c3,
		0xc4c4c4,
		0xc5c5c5,
		0xc6c6c6,
		0xc7c7c7,
		0xc8c8c8,
		0xc9c9c9,
		0xcacaca,
		0xcbcbcb,
		0xcccccc,
		0xcdcdcd,
		0xcecece,
		0xcfcfcf,
		0xd0d0d0,
		0xd1d1d1,
		0xd2d2d2,
		0xd3d3d3,
		0xd4d4d4,
		0xd5d5d5,
		0xd6d6d6,
		0xd7d7d7,
		0xd8d8d8,
		0xd9d9d9,
		0xdadada,
		0xdbdbdb,
		0xdcdcdc,
		0xdddddd,
		0xdedede,
		0xdfdfdf,
		0xe0e0e0,
		0xe1e1e1,
		0xe2e2e2,
		0xe3e3e3,
		0xe4e4e4,
		0xe5e5e5,
		0xe6e6e6,
		0xe7e7e7,
		0xe8e8e8,
		0xe9e9e9,
		0xeaeaea,
		0xebebeb,
		0xececec,
		0xededed,
		0xeeeeee,
		0xefefef,
		0xf0f0f0,
		0xf1f1f1,
		0xf2f2f2,
		0xf3f3f3,
		0xf4f4f4,
		0xf5f5f5,
		0xf6f6f6,
		0xf7f7f7,
		0xf8f8f8,
		0xf9f9f9,
		0xfafafa,
		0xfbfbfb,
		0xfcfcfc,
		0xfdfdfd,
		0xfefefe,
		0xffffff
	};

	static const Cmap cmap_jet{
		0x00008f,
		0x00008f,
		0x00008f,
		0x00008f,
		0x00009f,
		0x00009f,
		0x00009f,
		0x00009f,
		0x0000af,
		0x0000af,
		0x0000af,
		0x0000af,
		0x0000bf,
		0x0000bf,
		0x0000bf,
		0x0000bf,
		0x0000cf,
		0x0000cf,
		0x0000cf,
		0x0000cf,
		0x0000df,
		0x0000df,
		0x0000df,
		0x0000df,
		0x0000ef,
		0x0000ef,
		0x0000ef,
		0x0000ef,
		0x0000ff,
		0x0000ff,
		0x0000ff,
		0x0000ff,
		0x000fff,
		0x000fff,
		0x000fff,
		0x000fff,
		0x001fff,
		0x001fff,
		0x001fff,
		0x001fff,
		0x002fff,
		0x002fff,
		0x002fff,
		0x002fff,
		0x003fff,
		0x003fff,
		0x003fff,
		0x003fff,
		0x004fff,
		0x004fff,
		0x004fff,
		0x004fff,
		0x005fff,
		0x005fff,
		0x005fff,
		0x005fff,
		0x006fff,
		0x006fff,
		0x006fff,
		0x006fff,
		0x007fff,
		0x007fff,
		0x007fff,
		0x007fff,
		0x008fff,
		0x008fff,
		0x008fff,
		0x008fff,
		0x009fff,
		0x009fff,
		0x009fff,
		0x009fff,
		0x00afff,
		0x00afff,
		0x00afff,
		0x00afff,
		0x00bfff,
		0x00bfff,
		0x00bfff,
		0x00bfff,
		0x00cfff,
		0x00cfff,
		0x00cfff,
		0x00cfff,
		0x00dfff,
		0x00dfff,
		0x00dfff,
		0x00dfff,
		0x00efff,
		0x00efff,
		0x00efff,
		0x00efff,
		0x00ffff,
		0x00ffff,
		0x00ffff,
		0x00ffff,
		0x0fffef,
		0x0fffef,
		0x0fffef,
		0x0fffef,
		0x1fffdf,
		0x1fffdf,
		0x1fffdf,
		0x1fffdf,
		0x2fffcf,
		0x2fffcf,
		0x2fffcf,
		0x2fffcf,
		0x3fffbf,
		0x3fffbf,
		0x3fffbf,
		0x3fffbf,
		0x4fffaf,
		0x4fffaf,
		0x4fffaf,
		0x4fffaf,
		0x5fff9f,
		0x5fff9f,
		0x5fff9f,
		0x5fff9f,
		0x6fff8f,
		0x6fff8f,
		0x6fff8f,
		0x6fff8f,
		0x7fff7f,
		0x7fff7f,
		0x7fff7f,
		0x7fff7f,
		0x8fff6f,
		0x8fff6f,
		0x8fff6f,
		0x8fff6f,
		0x9fff5f,
		0x9fff5f,
		0x9fff5f,
		0x9fff5f,
		0xafff4f,
		0xafff4f,
		0xafff4f,
		0xafff4f,
		0xbfff3f,
		0xbfff3f,
		0xbfff3f,
		0xbfff3f,
		0xcfff2f,
		0xcfff2f,
		0xcfff2f,
		0xcfff2f,
		0xdfff1f,
		0xdfff1f,
		0xdfff1f,
		0xdfff1f,
		0xefff0f,
		0xefff0f,
		0xefff0f,
		0xefff0f,
		0xffff00,
		0xffff00,
		0xffff00,
		0xffff00,
		0xffef00,
		0xffef00,
		0xffef00,
		0xffef00,
		0xffdf00,
		0xffdf00,
		0xffdf00,
		0xffdf00,
		0xffcf00,
		0xffcf00,
		0xffcf00,
		0xffcf00,
		0xffbf00,
		0xffbf00,
		0xffbf00,
		0xffbf00,
		0xffaf00,
		0xffaf00,
		0xffaf00,
		0xffaf00,
		0xff9f00,
		0xff9f00,
		0xff9f00,
		0xff9f00,
		0xff8f00,
		0xff8f00,
		0xff8f00,
		0xff8f00,
		0xff7f00,
		0xff7f00,
		0xff7f00,
		0xff7f00,
		0xff6f00,
		0xff6f00,
		0xff6f00,
		0xff6f00,
		0xff5f00,
		0xff5f00,
		0xff5f00,
		0xff5f00,
		0xff4f00,
		0xff4f00,
		0xff4f00,
		0xff4f00,
		0xff3f00,
		0xff3f00,
		0xff3f00,
		0xff3f00,
		0xff2f00,
		0xff2f00,
		0xff2f00,
		0xff2f00,
		0xff1f00,
		0xff1f00,
		0xff1f00,
		0xff1f00,
		0xff0f00,
		0xff0f00,
		0xff0f00,
		0xff0f00,
		0xff0000,
		0xff0000,
		0xff0000,
		0xff0000,
		0xef0000,
		0xef0000,
		0xef0000,
		0xef0000,
		0xdf0000,
		0xdf0000,
		0xdf0000,
		0xdf0000,
		0xcf0000,
		0xcf0000,
		0xcf0000,
		0xcf0000,
		0xbf0000,
		0xbf0000,
		0xbf0000,
		0xbf0000,
		0xaf0000,
		0xaf0000,
		0xaf0000,
		0xaf0000,
		0x9f0000,
		0x9f0000,
		0x9f0000,
		0x9f0000,
		0x8f0000,
		0x8f0000,
		0x8f0000,
		0x8f0000,
		0x7f0000,
		0x7f0000,
		0x7f0000,
		0x7f0000
	};

	static const Cmap cmap_topo {
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0x77B36B,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xACD0A5,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xEFEBC0,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xDED6A3,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xD3CA9D,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xCAB982,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xC3A76B,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xB9985A,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA8753,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xAA9F8D,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8,
		0xE0DED8
	};

	switch (name) {
	case CMAP_JET:
		return cmap_jet;
	case CMAP_TOPO:
		return cmap_topo;
	default:
		return cmap_grey;
	}
}