#include "geohash.h"

bool geohash_decode(double *lat, double *lon, const char *buf, size_t len) {
    static const signed char b32[256] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        +0, +1, +2, +3, +4, +5, +6, +7, +8, +9, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, 10, 11, 12, 13, 14, 15, 16, -1, 17, 18, -1, 19, 20, -1,
        21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    };
    static const short deinterleave[1024] = {
        0x000, 0x001, 0x020, 0x021, 0x002, 0x003, 0x022, 0x023,
        0x040, 0x041, 0x060, 0x061, 0x042, 0x043, 0x062, 0x063,
        0x004, 0x005, 0x024, 0x025, 0x006, 0x007, 0x026, 0x027,
        0x044, 0x045, 0x064, 0x065, 0x046, 0x047, 0x066, 0x067,
        0x080, 0x081, 0x0a0, 0x0a1, 0x082, 0x083, 0x0a2, 0x0a3,
        0x0c0, 0x0c1, 0x0e0, 0x0e1, 0x0c2, 0x0c3, 0x0e2, 0x0e3,
        0x084, 0x085, 0x0a4, 0x0a5, 0x086, 0x087, 0x0a6, 0x0a7,
        0x0c4, 0x0c5, 0x0e4, 0x0e5, 0x0c6, 0x0c7, 0x0e6, 0x0e7,
        0x008, 0x009, 0x028, 0x029, 0x00a, 0x00b, 0x02a, 0x02b,
        0x048, 0x049, 0x068, 0x069, 0x04a, 0x04b, 0x06a, 0x06b,
        0x00c, 0x00d, 0x02c, 0x02d, 0x00e, 0x00f, 0x02e, 0x02f,
        0x04c, 0x04d, 0x06c, 0x06d, 0x04e, 0x04f, 0x06e, 0x06f,
        0x088, 0x089, 0x0a8, 0x0a9, 0x08a, 0x08b, 0x0aa, 0x0ab,
        0x0c8, 0x0c9, 0x0e8, 0x0e9, 0x0ca, 0x0cb, 0x0ea, 0x0eb,
        0x08c, 0x08d, 0x0ac, 0x0ad, 0x08e, 0x08f, 0x0ae, 0x0af,
        0x0cc, 0x0cd, 0x0ec, 0x0ed, 0x0ce, 0x0cf, 0x0ee, 0x0ef,
        0x100, 0x101, 0x120, 0x121, 0x102, 0x103, 0x122, 0x123,
        0x140, 0x141, 0x160, 0x161, 0x142, 0x143, 0x162, 0x163,
        0x104, 0x105, 0x124, 0x125, 0x106, 0x107, 0x126, 0x127,
        0x144, 0x145, 0x164, 0x165, 0x146, 0x147, 0x166, 0x167,
        0x180, 0x181, 0x1a0, 0x1a1, 0x182, 0x183, 0x1a2, 0x1a3,
        0x1c0, 0x1c1, 0x1e0, 0x1e1, 0x1c2, 0x1c3, 0x1e2, 0x1e3,
        0x184, 0x185, 0x1a4, 0x1a5, 0x186, 0x187, 0x1a6, 0x1a7,
        0x1c4, 0x1c5, 0x1e4, 0x1e5, 0x1c6, 0x1c7, 0x1e6, 0x1e7,
        0x108, 0x109, 0x128, 0x129, 0x10a, 0x10b, 0x12a, 0x12b,
        0x148, 0x149, 0x168, 0x169, 0x14a, 0x14b, 0x16a, 0x16b,
        0x10c, 0x10d, 0x12c, 0x12d, 0x10e, 0x10f, 0x12e, 0x12f,
        0x14c, 0x14d, 0x16c, 0x16d, 0x14e, 0x14f, 0x16e, 0x16f,
        0x188, 0x189, 0x1a8, 0x1a9, 0x18a, 0x18b, 0x1aa, 0x1ab,
        0x1c8, 0x1c9, 0x1e8, 0x1e9, 0x1ca, 0x1cb, 0x1ea, 0x1eb,
        0x18c, 0x18d, 0x1ac, 0x1ad, 0x18e, 0x18f, 0x1ae, 0x1af,
        0x1cc, 0x1cd, 0x1ec, 0x1ed, 0x1ce, 0x1cf, 0x1ee, 0x1ef,
        0x010, 0x011, 0x030, 0x031, 0x012, 0x013, 0x032, 0x033,
        0x050, 0x051, 0x070, 0x071, 0x052, 0x053, 0x072, 0x073,
        0x014, 0x015, 0x034, 0x035, 0x016, 0x017, 0x036, 0x037,
        0x054, 0x055, 0x074, 0x075, 0x056, 0x057, 0x076, 0x077,
        0x090, 0x091, 0x0b0, 0x0b1, 0x092, 0x093, 0x0b2, 0x0b3,
        0x0d0, 0x0d1, 0x0f0, 0x0f1, 0x0d2, 0x0d3, 0x0f2, 0x0f3,
        0x094, 0x095, 0x0b4, 0x0b5, 0x096, 0x097, 0x0b6, 0x0b7,
        0x0d4, 0x0d5, 0x0f4, 0x0f5, 0x0d6, 0x0d7, 0x0f6, 0x0f7,
        0x018, 0x019, 0x038, 0x039, 0x01a, 0x01b, 0x03a, 0x03b,
        0x058, 0x059, 0x078, 0x079, 0x05a, 0x05b, 0x07a, 0x07b,
        0x01c, 0x01d, 0x03c, 0x03d, 0x01e, 0x01f, 0x03e, 0x03f,
        0x05c, 0x05d, 0x07c, 0x07d, 0x05e, 0x05f, 0x07e, 0x07f,
        0x098, 0x099, 0x0b8, 0x0b9, 0x09a, 0x09b, 0x0ba, 0x0bb,
        0x0d8, 0x0d9, 0x0f8, 0x0f9, 0x0da, 0x0db, 0x0fa, 0x0fb,
        0x09c, 0x09d, 0x0bc, 0x0bd, 0x09e, 0x09f, 0x0be, 0x0bf,
        0x0dc, 0x0dd, 0x0fc, 0x0fd, 0x0de, 0x0df, 0x0fe, 0x0ff,
        0x110, 0x111, 0x130, 0x131, 0x112, 0x113, 0x132, 0x133,
        0x150, 0x151, 0x170, 0x171, 0x152, 0x153, 0x172, 0x173,
        0x114, 0x115, 0x134, 0x135, 0x116, 0x117, 0x136, 0x137,
        0x154, 0x155, 0x174, 0x175, 0x156, 0x157, 0x176, 0x177,
        0x190, 0x191, 0x1b0, 0x1b1, 0x192, 0x193, 0x1b2, 0x1b3,
        0x1d0, 0x1d1, 0x1f0, 0x1f1, 0x1d2, 0x1d3, 0x1f2, 0x1f3,
        0x194, 0x195, 0x1b4, 0x1b5, 0x196, 0x197, 0x1b6, 0x1b7,
        0x1d4, 0x1d5, 0x1f4, 0x1f5, 0x1d6, 0x1d7, 0x1f6, 0x1f7,
        0x118, 0x119, 0x138, 0x139, 0x11a, 0x11b, 0x13a, 0x13b,
        0x158, 0x159, 0x178, 0x179, 0x15a, 0x15b, 0x17a, 0x17b,
        0x11c, 0x11d, 0x13c, 0x13d, 0x11e, 0x11f, 0x13e, 0x13f,
        0x15c, 0x15d, 0x17c, 0x17d, 0x15e, 0x15f, 0x17e, 0x17f,
        0x198, 0x199, 0x1b8, 0x1b9, 0x19a, 0x19b, 0x1ba, 0x1bb,
        0x1d8, 0x1d9, 0x1f8, 0x1f9, 0x1da, 0x1db, 0x1fa, 0x1fb,
        0x19c, 0x19d, 0x1bc, 0x1bd, 0x19e, 0x19f, 0x1be, 0x1bf,
        0x1dc, 0x1dd, 0x1fc, 0x1fd, 0x1de, 0x1df, 0x1fe, 0x1ff,
        0x200, 0x201, 0x220, 0x221, 0x202, 0x203, 0x222, 0x223,
        0x240, 0x241, 0x260, 0x261, 0x242, 0x243, 0x262, 0x263,
        0x204, 0x205, 0x224, 0x225, 0x206, 0x207, 0x226, 0x227,
        0x244, 0x245, 0x264, 0x265, 0x246, 0x247, 0x266, 0x267,
        0x280, 0x281, 0x2a0, 0x2a1, 0x282, 0x283, 0x2a2, 0x2a3,
        0x2c0, 0x2c1, 0x2e0, 0x2e1, 0x2c2, 0x2c3, 0x2e2, 0x2e3,
        0x284, 0x285, 0x2a4, 0x2a5, 0x286, 0x287, 0x2a6, 0x2a7,
        0x2c4, 0x2c5, 0x2e4, 0x2e5, 0x2c6, 0x2c7, 0x2e6, 0x2e7,
        0x208, 0x209, 0x228, 0x229, 0x20a, 0x20b, 0x22a, 0x22b,
        0x248, 0x249, 0x268, 0x269, 0x24a, 0x24b, 0x26a, 0x26b,
        0x20c, 0x20d, 0x22c, 0x22d, 0x20e, 0x20f, 0x22e, 0x22f,
        0x24c, 0x24d, 0x26c, 0x26d, 0x24e, 0x24f, 0x26e, 0x26f,
        0x288, 0x289, 0x2a8, 0x2a9, 0x28a, 0x28b, 0x2aa, 0x2ab,
        0x2c8, 0x2c9, 0x2e8, 0x2e9, 0x2ca, 0x2cb, 0x2ea, 0x2eb,
        0x28c, 0x28d, 0x2ac, 0x2ad, 0x28e, 0x28f, 0x2ae, 0x2af,
        0x2cc, 0x2cd, 0x2ec, 0x2ed, 0x2ce, 0x2cf, 0x2ee, 0x2ef,
        0x300, 0x301, 0x320, 0x321, 0x302, 0x303, 0x322, 0x323,
        0x340, 0x341, 0x360, 0x361, 0x342, 0x343, 0x362, 0x363,
        0x304, 0x305, 0x324, 0x325, 0x306, 0x307, 0x326, 0x327,
        0x344, 0x345, 0x364, 0x365, 0x346, 0x347, 0x366, 0x367,
        0x380, 0x381, 0x3a0, 0x3a1, 0x382, 0x383, 0x3a2, 0x3a3,
        0x3c0, 0x3c1, 0x3e0, 0x3e1, 0x3c2, 0x3c3, 0x3e2, 0x3e3,
        0x384, 0x385, 0x3a4, 0x3a5, 0x386, 0x387, 0x3a6, 0x3a7,
        0x3c4, 0x3c5, 0x3e4, 0x3e5, 0x3c6, 0x3c7, 0x3e6, 0x3e7,
        0x308, 0x309, 0x328, 0x329, 0x30a, 0x30b, 0x32a, 0x32b,
        0x348, 0x349, 0x368, 0x369, 0x34a, 0x34b, 0x36a, 0x36b,
        0x30c, 0x30d, 0x32c, 0x32d, 0x30e, 0x30f, 0x32e, 0x32f,
        0x34c, 0x34d, 0x36c, 0x36d, 0x34e, 0x34f, 0x36e, 0x36f,
        0x388, 0x389, 0x3a8, 0x3a9, 0x38a, 0x38b, 0x3aa, 0x3ab,
        0x3c8, 0x3c9, 0x3e8, 0x3e9, 0x3ca, 0x3cb, 0x3ea, 0x3eb,
        0x38c, 0x38d, 0x3ac, 0x3ad, 0x38e, 0x38f, 0x3ae, 0x3af,
        0x3cc, 0x3cd, 0x3ec, 0x3ed, 0x3ce, 0x3cf, 0x3ee, 0x3ef,
        0x210, 0x211, 0x230, 0x231, 0x212, 0x213, 0x232, 0x233,
        0x250, 0x251, 0x270, 0x271, 0x252, 0x253, 0x272, 0x273,
        0x214, 0x215, 0x234, 0x235, 0x216, 0x217, 0x236, 0x237,
        0x254, 0x255, 0x274, 0x275, 0x256, 0x257, 0x276, 0x277,
        0x290, 0x291, 0x2b0, 0x2b1, 0x292, 0x293, 0x2b2, 0x2b3,
        0x2d0, 0x2d1, 0x2f0, 0x2f1, 0x2d2, 0x2d3, 0x2f2, 0x2f3,
        0x294, 0x295, 0x2b4, 0x2b5, 0x296, 0x297, 0x2b6, 0x2b7,
        0x2d4, 0x2d5, 0x2f4, 0x2f5, 0x2d6, 0x2d7, 0x2f6, 0x2f7,
        0x218, 0x219, 0x238, 0x239, 0x21a, 0x21b, 0x23a, 0x23b,
        0x258, 0x259, 0x278, 0x279, 0x25a, 0x25b, 0x27a, 0x27b,
        0x21c, 0x21d, 0x23c, 0x23d, 0x21e, 0x21f, 0x23e, 0x23f,
        0x25c, 0x25d, 0x27c, 0x27d, 0x25e, 0x25f, 0x27e, 0x27f,
        0x298, 0x299, 0x2b8, 0x2b9, 0x29a, 0x29b, 0x2ba, 0x2bb,
        0x2d8, 0x2d9, 0x2f8, 0x2f9, 0x2da, 0x2db, 0x2fa, 0x2fb,
        0x29c, 0x29d, 0x2bc, 0x2bd, 0x29e, 0x29f, 0x2be, 0x2bf,
        0x2dc, 0x2dd, 0x2fc, 0x2fd, 0x2de, 0x2df, 0x2fe, 0x2ff,
        0x310, 0x311, 0x330, 0x331, 0x312, 0x313, 0x332, 0x333,
        0x350, 0x351, 0x370, 0x371, 0x352, 0x353, 0x372, 0x373,
        0x314, 0x315, 0x334, 0x335, 0x316, 0x317, 0x336, 0x337,
        0x354, 0x355, 0x374, 0x375, 0x356, 0x357, 0x376, 0x377,
        0x390, 0x391, 0x3b0, 0x3b1, 0x392, 0x393, 0x3b2, 0x3b3,
        0x3d0, 0x3d1, 0x3f0, 0x3f1, 0x3d2, 0x3d3, 0x3f2, 0x3f3,
        0x394, 0x395, 0x3b4, 0x3b5, 0x396, 0x397, 0x3b6, 0x3b7,
        0x3d4, 0x3d5, 0x3f4, 0x3f5, 0x3d6, 0x3d7, 0x3f6, 0x3f7,
        0x318, 0x319, 0x338, 0x339, 0x31a, 0x31b, 0x33a, 0x33b,
        0x358, 0x359, 0x378, 0x379, 0x35a, 0x35b, 0x37a, 0x37b,
        0x31c, 0x31d, 0x33c, 0x33d, 0x31e, 0x31f, 0x33e, 0x33f,
        0x35c, 0x35d, 0x37c, 0x37d, 0x35e, 0x35f, 0x37e, 0x37f,
        0x398, 0x399, 0x3b8, 0x3b9, 0x39a, 0x39b, 0x3ba, 0x3bb,
        0x3d8, 0x3d9, 0x3f8, 0x3f9, 0x3da, 0x3db, 0x3fa, 0x3fb,
        0x39c, 0x39d, 0x3bc, 0x3bd, 0x39e, 0x39f, 0x3be, 0x3bf,
        0x3dc, 0x3dd, 0x3fc, 0x3fd, 0x3de, 0x3df, 0x3fe, 0x3ff,
    };

    len = len > 21 ? 21 : len;
    long long blat = 0;
    long long blon = 0;

    // Note: This has been optimized for decoding shorter geohashes by
    // bailing out early when input ends. The benchmark shows that this is
    // faster than unconditionally parsing the full 20 bytes (with care not
    // to overread) and discarding. If the input length is known statically,
    // these conditional branches could be eliminated and perhaps even the
    // loop unrolled.

    // Decode two bytes at a time
    for (int i = 0; i+1 < len; i += 2) {
        int hi = b32[buf[i+0]&0xff];
        int lo = b32[buf[i+1]&0xff];
        if (hi < 0 || lo < 0) {
            return false;
        }
        int c = deinterleave[hi<<5 | lo];
        blat |= (long long)(c & 0x01f) << (55 - i/2*5);
        blon |= (long long)(c & 0x3e0) << (50 - i/2*5);
    }

    // Handle tail byte, if any
    if (len & 1) {
        int hi = b32[buf[len-1]&0xff];
        if (hi < 0) {
            return false;
        }
        int c = deinterleave[hi<<5];
        blat |= (long long)(c & 0x01f) << (55 - len/2*5);
        blon |= (long long)(c & 0x3e0) << (50 - len/2*5);
    }

    // The bottom 5 bits are unused. Everything was done higher to simplify
    // shifting (no prior right shifts were necessary). We'll need more
    // overhead below.
    blat >>= 5;
    blon >>= 5;

    // Compute the center (mean) between the decoded value (lower bound) and
    // the next neighbor up (upper bound). Shortcut: Set the bit just below
    // the last decoded bit to split the difference. Then shift the range by
    // half to center it around zero.
    int64_t hlat = blat + (1LL << (54 - (0 + len*5)/2)) - (1LL << 54);
    int64_t hlon = blon + (1LL << (54 - (1 + len*5)/2)) - (1LL << 54);
    *lat = 180 * hlat / (double)(1LL << 55);
    *lon = 360 * hlon / (double)(1LL << 55);
    return true;
}


bool geohash_encode(double lat, double lon, size_t precision, char *buf) {
    static const short interleave_b32[1024] = {
        0x3030, 0x3031, 0x3034, 0x3035, 0x3068, 0x306a, 0x306e, 0x3070,
        0x3230, 0x3231, 0x3234, 0x3235, 0x3268, 0x326a, 0x326e, 0x3270,
        0x3830, 0x3831, 0x3834, 0x3835, 0x3868, 0x386a, 0x386e, 0x3870,
        0x6230, 0x6231, 0x6234, 0x6235, 0x6268, 0x626a, 0x626e, 0x6270,
        0x3032, 0x3033, 0x3036, 0x3037, 0x306b, 0x306d, 0x3071, 0x3072,
        0x3232, 0x3233, 0x3236, 0x3237, 0x326b, 0x326d, 0x3271, 0x3272,
        0x3832, 0x3833, 0x3836, 0x3837, 0x386b, 0x386d, 0x3871, 0x3872,
        0x6232, 0x6233, 0x6236, 0x6237, 0x626b, 0x626d, 0x6271, 0x6272,
        0x3038, 0x3039, 0x3064, 0x3065, 0x3073, 0x3074, 0x3077, 0x3078,
        0x3238, 0x3239, 0x3264, 0x3265, 0x3273, 0x3274, 0x3277, 0x3278,
        0x3838, 0x3839, 0x3864, 0x3865, 0x3873, 0x3874, 0x3877, 0x3878,
        0x6238, 0x6239, 0x6264, 0x6265, 0x6273, 0x6274, 0x6277, 0x6278,
        0x3062, 0x3063, 0x3066, 0x3067, 0x3075, 0x3076, 0x3079, 0x307a,
        0x3262, 0x3263, 0x3266, 0x3267, 0x3275, 0x3276, 0x3279, 0x327a,
        0x3862, 0x3863, 0x3866, 0x3867, 0x3875, 0x3876, 0x3879, 0x387a,
        0x6262, 0x6263, 0x6266, 0x6267, 0x6275, 0x6276, 0x6279, 0x627a,
        0x3130, 0x3131, 0x3134, 0x3135, 0x3168, 0x316a, 0x316e, 0x3170,
        0x3330, 0x3331, 0x3334, 0x3335, 0x3368, 0x336a, 0x336e, 0x3370,
        0x3930, 0x3931, 0x3934, 0x3935, 0x3968, 0x396a, 0x396e, 0x3970,
        0x6330, 0x6331, 0x6334, 0x6335, 0x6368, 0x636a, 0x636e, 0x6370,
        0x3132, 0x3133, 0x3136, 0x3137, 0x316b, 0x316d, 0x3171, 0x3172,
        0x3332, 0x3333, 0x3336, 0x3337, 0x336b, 0x336d, 0x3371, 0x3372,
        0x3932, 0x3933, 0x3936, 0x3937, 0x396b, 0x396d, 0x3971, 0x3972,
        0x6332, 0x6333, 0x6336, 0x6337, 0x636b, 0x636d, 0x6371, 0x6372,
        0x3138, 0x3139, 0x3164, 0x3165, 0x3173, 0x3174, 0x3177, 0x3178,
        0x3338, 0x3339, 0x3364, 0x3365, 0x3373, 0x3374, 0x3377, 0x3378,
        0x3938, 0x3939, 0x3964, 0x3965, 0x3973, 0x3974, 0x3977, 0x3978,
        0x6338, 0x6339, 0x6364, 0x6365, 0x6373, 0x6374, 0x6377, 0x6378,
        0x3162, 0x3163, 0x3166, 0x3167, 0x3175, 0x3176, 0x3179, 0x317a,
        0x3362, 0x3363, 0x3366, 0x3367, 0x3375, 0x3376, 0x3379, 0x337a,
        0x3962, 0x3963, 0x3966, 0x3967, 0x3975, 0x3976, 0x3979, 0x397a,
        0x6362, 0x6363, 0x6366, 0x6367, 0x6375, 0x6376, 0x6379, 0x637a,
        0x3430, 0x3431, 0x3434, 0x3435, 0x3468, 0x346a, 0x346e, 0x3470,
        0x3630, 0x3631, 0x3634, 0x3635, 0x3668, 0x366a, 0x366e, 0x3670,
        0x6430, 0x6431, 0x6434, 0x6435, 0x6468, 0x646a, 0x646e, 0x6470,
        0x6630, 0x6631, 0x6634, 0x6635, 0x6668, 0x666a, 0x666e, 0x6670,
        0x3432, 0x3433, 0x3436, 0x3437, 0x346b, 0x346d, 0x3471, 0x3472,
        0x3632, 0x3633, 0x3636, 0x3637, 0x366b, 0x366d, 0x3671, 0x3672,
        0x6432, 0x6433, 0x6436, 0x6437, 0x646b, 0x646d, 0x6471, 0x6472,
        0x6632, 0x6633, 0x6636, 0x6637, 0x666b, 0x666d, 0x6671, 0x6672,
        0x3438, 0x3439, 0x3464, 0x3465, 0x3473, 0x3474, 0x3477, 0x3478,
        0x3638, 0x3639, 0x3664, 0x3665, 0x3673, 0x3674, 0x3677, 0x3678,
        0x6438, 0x6439, 0x6464, 0x6465, 0x6473, 0x6474, 0x6477, 0x6478,
        0x6638, 0x6639, 0x6664, 0x6665, 0x6673, 0x6674, 0x6677, 0x6678,
        0x3462, 0x3463, 0x3466, 0x3467, 0x3475, 0x3476, 0x3479, 0x347a,
        0x3662, 0x3663, 0x3666, 0x3667, 0x3675, 0x3676, 0x3679, 0x367a,
        0x6462, 0x6463, 0x6466, 0x6467, 0x6475, 0x6476, 0x6479, 0x647a,
        0x6662, 0x6663, 0x6666, 0x6667, 0x6675, 0x6676, 0x6679, 0x667a,
        0x3530, 0x3531, 0x3534, 0x3535, 0x3568, 0x356a, 0x356e, 0x3570,
        0x3730, 0x3731, 0x3734, 0x3735, 0x3768, 0x376a, 0x376e, 0x3770,
        0x6530, 0x6531, 0x6534, 0x6535, 0x6568, 0x656a, 0x656e, 0x6570,
        0x6730, 0x6731, 0x6734, 0x6735, 0x6768, 0x676a, 0x676e, 0x6770,
        0x3532, 0x3533, 0x3536, 0x3537, 0x356b, 0x356d, 0x3571, 0x3572,
        0x3732, 0x3733, 0x3736, 0x3737, 0x376b, 0x376d, 0x3771, 0x3772,
        0x6532, 0x6533, 0x6536, 0x6537, 0x656b, 0x656d, 0x6571, 0x6572,
        0x6732, 0x6733, 0x6736, 0x6737, 0x676b, 0x676d, 0x6771, 0x6772,
        0x3538, 0x3539, 0x3564, 0x3565, 0x3573, 0x3574, 0x3577, 0x3578,
        0x3738, 0x3739, 0x3764, 0x3765, 0x3773, 0x3774, 0x3777, 0x3778,
        0x6538, 0x6539, 0x6564, 0x6565, 0x6573, 0x6574, 0x6577, 0x6578,
        0x6738, 0x6739, 0x6764, 0x6765, 0x6773, 0x6774, 0x6777, 0x6778,
        0x3562, 0x3563, 0x3566, 0x3567, 0x3575, 0x3576, 0x3579, 0x357a,
        0x3762, 0x3763, 0x3766, 0x3767, 0x3775, 0x3776, 0x3779, 0x377a,
        0x6562, 0x6563, 0x6566, 0x6567, 0x6575, 0x6576, 0x6579, 0x657a,
        0x6762, 0x6763, 0x6766, 0x6767, 0x6775, 0x6776, 0x6779, 0x677a,
        0x6830, 0x6831, 0x6834, 0x6835, 0x6868, 0x686a, 0x686e, 0x6870,
        0x6b30, 0x6b31, 0x6b34, 0x6b35, 0x6b68, 0x6b6a, 0x6b6e, 0x6b70,
        0x7330, 0x7331, 0x7334, 0x7335, 0x7368, 0x736a, 0x736e, 0x7370,
        0x7530, 0x7531, 0x7534, 0x7535, 0x7568, 0x756a, 0x756e, 0x7570,
        0x6832, 0x6833, 0x6836, 0x6837, 0x686b, 0x686d, 0x6871, 0x6872,
        0x6b32, 0x6b33, 0x6b36, 0x6b37, 0x6b6b, 0x6b6d, 0x6b71, 0x6b72,
        0x7332, 0x7333, 0x7336, 0x7337, 0x736b, 0x736d, 0x7371, 0x7372,
        0x7532, 0x7533, 0x7536, 0x7537, 0x756b, 0x756d, 0x7571, 0x7572,
        0x6838, 0x6839, 0x6864, 0x6865, 0x6873, 0x6874, 0x6877, 0x6878,
        0x6b38, 0x6b39, 0x6b64, 0x6b65, 0x6b73, 0x6b74, 0x6b77, 0x6b78,
        0x7338, 0x7339, 0x7364, 0x7365, 0x7373, 0x7374, 0x7377, 0x7378,
        0x7538, 0x7539, 0x7564, 0x7565, 0x7573, 0x7574, 0x7577, 0x7578,
        0x6862, 0x6863, 0x6866, 0x6867, 0x6875, 0x6876, 0x6879, 0x687a,
        0x6b62, 0x6b63, 0x6b66, 0x6b67, 0x6b75, 0x6b76, 0x6b79, 0x6b7a,
        0x7362, 0x7363, 0x7366, 0x7367, 0x7375, 0x7376, 0x7379, 0x737a,
        0x7562, 0x7563, 0x7566, 0x7567, 0x7575, 0x7576, 0x7579, 0x757a,
        0x6a30, 0x6a31, 0x6a34, 0x6a35, 0x6a68, 0x6a6a, 0x6a6e, 0x6a70,
        0x6d30, 0x6d31, 0x6d34, 0x6d35, 0x6d68, 0x6d6a, 0x6d6e, 0x6d70,
        0x7430, 0x7431, 0x7434, 0x7435, 0x7468, 0x746a, 0x746e, 0x7470,
        0x7630, 0x7631, 0x7634, 0x7635, 0x7668, 0x766a, 0x766e, 0x7670,
        0x6a32, 0x6a33, 0x6a36, 0x6a37, 0x6a6b, 0x6a6d, 0x6a71, 0x6a72,
        0x6d32, 0x6d33, 0x6d36, 0x6d37, 0x6d6b, 0x6d6d, 0x6d71, 0x6d72,
        0x7432, 0x7433, 0x7436, 0x7437, 0x746b, 0x746d, 0x7471, 0x7472,
        0x7632, 0x7633, 0x7636, 0x7637, 0x766b, 0x766d, 0x7671, 0x7672,
        0x6a38, 0x6a39, 0x6a64, 0x6a65, 0x6a73, 0x6a74, 0x6a77, 0x6a78,
        0x6d38, 0x6d39, 0x6d64, 0x6d65, 0x6d73, 0x6d74, 0x6d77, 0x6d78,
        0x7438, 0x7439, 0x7464, 0x7465, 0x7473, 0x7474, 0x7477, 0x7478,
        0x7638, 0x7639, 0x7664, 0x7665, 0x7673, 0x7674, 0x7677, 0x7678,
        0x6a62, 0x6a63, 0x6a66, 0x6a67, 0x6a75, 0x6a76, 0x6a79, 0x6a7a,
        0x6d62, 0x6d63, 0x6d66, 0x6d67, 0x6d75, 0x6d76, 0x6d79, 0x6d7a,
        0x7462, 0x7463, 0x7466, 0x7467, 0x7475, 0x7476, 0x7479, 0x747a,
        0x7662, 0x7663, 0x7666, 0x7667, 0x7675, 0x7676, 0x7679, 0x767a,
        0x6e30, 0x6e31, 0x6e34, 0x6e35, 0x6e68, 0x6e6a, 0x6e6e, 0x6e70,
        0x7130, 0x7131, 0x7134, 0x7135, 0x7168, 0x716a, 0x716e, 0x7170,
        0x7730, 0x7731, 0x7734, 0x7735, 0x7768, 0x776a, 0x776e, 0x7770,
        0x7930, 0x7931, 0x7934, 0x7935, 0x7968, 0x796a, 0x796e, 0x7970,
        0x6e32, 0x6e33, 0x6e36, 0x6e37, 0x6e6b, 0x6e6d, 0x6e71, 0x6e72,
        0x7132, 0x7133, 0x7136, 0x7137, 0x716b, 0x716d, 0x7171, 0x7172,
        0x7732, 0x7733, 0x7736, 0x7737, 0x776b, 0x776d, 0x7771, 0x7772,
        0x7932, 0x7933, 0x7936, 0x7937, 0x796b, 0x796d, 0x7971, 0x7972,
        0x6e38, 0x6e39, 0x6e64, 0x6e65, 0x6e73, 0x6e74, 0x6e77, 0x6e78,
        0x7138, 0x7139, 0x7164, 0x7165, 0x7173, 0x7174, 0x7177, 0x7178,
        0x7738, 0x7739, 0x7764, 0x7765, 0x7773, 0x7774, 0x7777, 0x7778,
        0x7938, 0x7939, 0x7964, 0x7965, 0x7973, 0x7974, 0x7977, 0x7978,
        0x6e62, 0x6e63, 0x6e66, 0x6e67, 0x6e75, 0x6e76, 0x6e79, 0x6e7a,
        0x7162, 0x7163, 0x7166, 0x7167, 0x7175, 0x7176, 0x7179, 0x717a,
        0x7762, 0x7763, 0x7766, 0x7767, 0x7775, 0x7776, 0x7779, 0x777a,
        0x7962, 0x7963, 0x7966, 0x7967, 0x7975, 0x7976, 0x7979, 0x797a,
        0x7030, 0x7031, 0x7034, 0x7035, 0x7068, 0x706a, 0x706e, 0x7070,
        0x7230, 0x7231, 0x7234, 0x7235, 0x7268, 0x726a, 0x726e, 0x7270,
        0x7830, 0x7831, 0x7834, 0x7835, 0x7868, 0x786a, 0x786e, 0x7870,
        0x7a30, 0x7a31, 0x7a34, 0x7a35, 0x7a68, 0x7a6a, 0x7a6e, 0x7a70,
        0x7032, 0x7033, 0x7036, 0x7037, 0x706b, 0x706d, 0x7071, 0x7072,
        0x7232, 0x7233, 0x7236, 0x7237, 0x726b, 0x726d, 0x7271, 0x7272,
        0x7832, 0x7833, 0x7836, 0x7837, 0x786b, 0x786d, 0x7871, 0x7872,
        0x7a32, 0x7a33, 0x7a36, 0x7a37, 0x7a6b, 0x7a6d, 0x7a71, 0x7a72,
        0x7038, 0x7039, 0x7064, 0x7065, 0x7073, 0x7074, 0x7077, 0x7078,
        0x7238, 0x7239, 0x7264, 0x7265, 0x7273, 0x7274, 0x7277, 0x7278,
        0x7838, 0x7839, 0x7864, 0x7865, 0x7873, 0x7874, 0x7877, 0x7878,
        0x7a38, 0x7a39, 0x7a64, 0x7a65, 0x7a73, 0x7a74, 0x7a77, 0x7a78,
        0x7062, 0x7063, 0x7066, 0x7067, 0x7075, 0x7076, 0x7079, 0x707a,
        0x7262, 0x7263, 0x7266, 0x7267, 0x7275, 0x7276, 0x7279, 0x727a,
        0x7862, 0x7863, 0x7866, 0x7867, 0x7875, 0x7876, 0x7879, 0x787a,
        0x7a62, 0x7a63, 0x7a66, 0x7a67, 0x7a75, 0x7a76, 0x7a79, 0x7a7a,
    };
    int64_t blat = (lat +  90) / 180 * (1LL << 53);
    int64_t blon = (lon + 180) / 360 * (1LL << 53);
    int16_t chunks[11] = {
        interleave_b32[(blon >> 43 & 0x3e0) | (blat >> 48 & 0x01f)],
        interleave_b32[(blon >> 38 & 0x3e0) | (blat >> 43 & 0x01f)],
        interleave_b32[(blon >> 33 & 0x3e0) | (blat >> 38 & 0x01f)],
        interleave_b32[(blon >> 28 & 0x3e0) | (blat >> 33 & 0x01f)],
        interleave_b32[(blon >> 23 & 0x3e0) | (blat >> 28 & 0x01f)],
        interleave_b32[(blon >> 18 & 0x3e0) | (blat >> 23 & 0x01f)],
        interleave_b32[(blon >> 13 & 0x3e0) | (blat >> 18 & 0x01f)],
        interleave_b32[(blon >>  8 & 0x3e0) | (blat >> 13 & 0x01f)],
        interleave_b32[(blon >>  3 & 0x3e0) | (blat >>  8 & 0x01f)],
        interleave_b32[(blon <<  2 & 0x3e0) | (blat >>  3 & 0x01f)],
        interleave_b32[(blon <<  7 & 0x3e0) | (blat <<  2 & 0x01f)],
    };

    for (size_t i = 0; i < precision; i++) {
        if (i % 2 == 0) {
            buf[i] = chunks[i/2] >> 8;
        } else {
            buf[i] = chunks[i/2] >> 0;
        }
    }
    buf[precision] = '\0';

    return true;
}

