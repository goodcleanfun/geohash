#include <stdio.h>
#include <string.h>
#include "geohash.h"
#include "greatest/greatest.h"

// Test values were computed to exact precision using arbitrary precision
static const struct {
    int64_t len;
    char *h;
    double lat, lon;
} tests[] = {
    {1, "y", 67.5, 112.5},
    {1, "6", -22.5, -67.5},
    {1, "e", 22.5, -22.5},
    {1, "f", 67.5, -67.5},
    {1, "k", -22.5, 22.5},
    {1, "u", 67.5, 22.5},
    {1, "q", -22.5, 112.5},
    {1, "d", 22.5, -67.5},
    {2, "zv", 75.9375, 174.375},
    {2, "z1", 53.4375, 140.625},
    {2, "mc", -36.5625, 84.375},
    {2, "vk", 70.3125, 61.875},
    {2, "2z", -2.8125, -140.625},
    {2, "kv", -14.0625, 39.375},
    {2, "zt", 75.9375, 163.125},
    {2, "qz", -2.8125, 129.375},
    {3, "q2q", -42.890625, 110.390625},
    {3, "81e", 9.140625, -175.078125},
    {3, "epn", 40.078125, -35.859375},
    {3, "v67", 58.359375, 61.171875},
    {3, "5bn", -89.296875, -2.109375},
    {3, "cgy", 66.796875, -92.109375},
    {3, "qfq", -31.640625, 132.890625},
    {3, "hw1", -55.546875, 24.609375},
    {4, "pxxs", -47.021484375, 168.22265625},
    {4, "g9dw", 54.580078125, -18.80859375},
    {4, "4d9f", -75.498046875, -64.86328125},
    {4, "x2qb", 1.494140625, 155.91796875},
    {4, "h7mp", -70.400390625, 18.45703125},
    {4, "sqs5", 37.177734375, 17.05078125},
    {4, "u646", 56.689453125, 14.58984375},
    {4, "uyf6", 83.408203125, 37.08984375},
    {5, "edcct", 15.75439453125, -19.79736328125},
    {5, "e54d9", 17.33642578125, -41.41845703125},
    {5, "d66m6", 13.60107421875, -75.47607421875},
    {5, "1qpcc", -55.92041015625, -112.78564453125},
    {5, "t7857", 20.28076171875, 56.40380859375},
    {5, "2xnzz", -4.24072265625, -147.67822265625},
    {5, "z00ry", 46.38427734375, 135.63720703125},
    {5, "ytx9t", 76.22314453125, 123.28857421875},
    {6, "zksr9z", 71.67205810546875, 152.3089599609375},
    {6, "e1k1t6", 7.30865478515625, -39.1387939453125},
    {6, "dnwd1y", 36.94976806640625, -80.7769775390625},
    {6, "yccrng", 56.09344482421875, 125.8099365234375},
    {6, "uswux8", 71.10626220703125, 32.3272705078125},
    {6, "uwkksc", 80.95550537109375, 28.6907958984375},
    {6, "mq3j2s", -8.89617919921875, 57.6837158203125},
    {6, "zrfspt", 89.32708740234375, 150.1007080078125},
    {7, "b9mh6g9", 52.79823303222656, -150.34584045410156},
    {7, "egse2zw", 20.300674438476562, -4.8799896240234375},
    {7, "wu0m6bs", 23.426284790039062, 124.22859191894531},
    {7, "jzg2b7g", -46.25312805175781, 83.33610534667969},
    {7, "1g8ryn2", -68.91517639160156, -100.63407897949219},
    {7, "f9mjshu", 53.02482604980469, -60.28678894042969},
    {7, "bf1g2pv", 56.86454772949219, -143.78150939941406},
    {7, "jsdtg7f", -63.65547180175781, 71.16188049316406},
    {8, "h96ym45x", -81.85784339904785, 26.59189224243164},
    {8, "um0gb5ku", 73.80280494689941, 12.311382293701172},
    {8, "8jybftw3", 32.50605583190918, -170.3891944885254},
    {8, "su74ppev", 24.299955368041992, 38.28168869018555},
    {8, "e4kjubc4", 13.671541213989258, -39.16471481323242},
    {8, "rgg502sx", -23.3748722076416, 172.98608779907227},
    {8, "01jt30rz", -83.44948768615723, -172.21086502075195},
    {8, "cqz25716", 82.98565864562988, -113.40997695922852},
    {9, "1evyv5c37", -67.69889116287231, -104.19246912002563},
    {9, "z3xggwugd", 54.13438081741333, 157.30887651443481},
    {9, "52szqdt3m", -85.89911699295044, -26.777222156524658},
    {9, "454xcmbqw", -71.72997236251831, -86.42882108688354},
    {9, "jpjd5s5sq", -50.25071382522583, 52.893269062042236},
    {9, "37nntnmmb", -26.947081089019775, -115.08554220199585},
    {9, "8245dx4vh", 0.6545662879943848, -165.82366704940796},
    {9, "pghzrdgzt", -71.83417081832886, 175.7646632194519},
    {10, "4y7u1k3pey", -54.11595672369003, -50.92009127140045},
    {10, "wxqt5t422v", 41.6876944899559, 121.79756104946136},
    {10, "cm7kebvdhx", 75.326769053936, -119.00714099407196},
    {10, "xtx2vcnfzb", 31.075303852558136, 167.95760571956635},
    {10, "j3v57qnnzd", -79.55082982778549, 63.43263924121857},
    {10, "fep9snz5vw", 62.17642933130264, -56.767489314079285},
    {10, "kb0b8yt29p", -44.87627774477005, 34.8449045419693},
    {10, "3qhubjezuh", -10.38347214460373, -117.06498563289642},
    {11, "7r0fvynppdv", -5.1074255257844925, -32.43405796587467},
    {11, "s44ny2wmzne", 12.440292611718178, 3.0960463732481003},
    {11, "6cgqvv3nv3w", -33.93972001969814, -51.425394639372826},
    {11, "f4b1z8krrub", 60.77901117503643, -89.6642404049635},
    {11, "81gnjcspgmu", 10.908039137721062, -175.52292577922344},
    {11, "dxtzrgft071", 43.483388498425484, -59.07004036009312},
    {11, "41ryucru4zb", -81.77463121712208, -78.882060572505},
    {11, "pxsjbh94urk", -46.77652694284916, 163.12656171619892},
    {12, "6738wm9rqbxk", -26.59939899109304, -76.36395061388612},
    {12, "re6rv439vb5u", -25.34378453157842, 160.88610095903277},
    {12, "suqbzyesf1nq", 24.07461334951222, 43.5876645706594},
    {12, "engkf9kmk2z6", 38.81148305721581, -40.31379545107484},
    {12, "mjq87eeuvb0n", -15.40476213209331, 54.29983036592603},
    {12, "1811x3uy327k", -89.72564010880888, -110.76856518164277},
    {12, "3ftq3m9mwdu3", -29.80769564397633, -93.8102544285357},
    {12, "eh2utyk3dnz7", 24.731892189010978, -43.686694744974375},
    {13, "us12qmc7zw9tr", 67.57621050579473, 24.53451150795445},
    {13, "7jqbr77bsg38r", -15.406848576385528, -35.183852969203144},
    {13, "eq7ndje9dw7s9", 36.32933232234791, -29.438440811354667},
    {13, "8m5yp4z48qymd", 29.195260007400066, -163.1593130598776},
    {13, "6n62596c95c2s", -9.836605831515044, -86.67830757563934},
    {13, "hjp13zh4k5mg8", -61.61641700891778, 9.926326854620129},
    {13, "krxdzn8zjb7pq", -2.292191789019853, 22.105773214716464},
    {13, "z04m8ewx4nq04", 45.987258127424866, 138.19505552994087},
    {14, "6rzfkqkpw29dte", -0.9751193321426399, -67.65903128951322},
    {14, "qqzn5enebxc8ks", -5.959397637925576, 111.25650864851195},
    {14, "ykpbp299n7uzf9", 67.50293966702884, 112.46937565330882},
    {14, "pypbe22j9seums", -56.15976707049413, 179.7913294035243},
    {14, "8sm27c03smexvd", 23.95597678521881, -149.95186193555128},
    {14, "xzhwuncqfs610c", 40.59978398348903, 175.25573287100997},
    {14, "mkveqqsxkrhzde", -17.672971999563742, 64.26540188316721},
    {14, "3dnev0qpy8q00b", -33.088115302089136, -103.13112165837083},
    {15, "8nbwe360bsjyn9p", 39.118345795504865, -179.15127670108632},
    {15, "u4r62pq8rm1jrhv", 58.091653895025956, 10.204551669521607},
    {15, "0gnknh1yq4j1wbv", -72.39881847526703, -137.19459787804226},
    {15, "wuz8ugph5kw9e6g", 26.867776481594774, 134.51537660432223},
    {15, "rbfqxyrzhmfme8m", -39.603106200593174, 172.26546723883075},
    {15, "ng4we6wz041gg1c", -71.96747596790374, 127.41771945460641},
    {15, "2ssy4qunnffj12s", -18.594688080702326, -150.71564996374946},
    {15, "78gcqccd26vx71y", -40.551508184944396, -16.927853464949294},
    {16, "ycke272q6d48bmtk", 52.62147561342317, 130.08956530951764},
    {16, "hhm2dgvv8dzthr3f", -65.98430024346271, 7.511590229241847},
    {16, "dubf7hcgetyjpm5e", 27.14098286697663, -55.060914744299225},
    {16, "f5zkb9prqzc9nrx3", 66.93549042795894, -79.77246652206304},
    {16, "02d7j0ess702m4v6", -86.65662104179319, -165.3612214768691},
    {16, "smycq4csg7udh4kx", 32.57941862007101, 21.008064995057794},
    {16, "zw21pwypg5x4umfj", 80.3704597505839, 157.8379679045065},
    {16, "bm4g8n50pe3wy1mq", 73.773211932903, -164.878368491718},
    {17, "txgx178hfcrpwyh65", 44.84426821034731, 72.47693433415179},
    {17, "tsyr5duk9p57cnx4y", 27.965135641459256, 76.44875490584354},
    {17, "vhgukh10jw6gev7v3", 72.48782711147896, 50.45083192557968},
    {17, "k7ctqsqv3eqqgrj4s", -22.959132664989284, 13.654362143463459},
    {17, "hwwkcdxkmdye48hh9", -52.58806133567077, 31.36518169025976},
    {17, "10q9j5tc60w9177y0", -88.39852523118452, -125.63165740897375},
    {17, "q6djkt4up1hdxw2hb", -29.98649015462661, 104.26433721326475},
    {17, "dwyxdzzc28p89zupx", 39.32989609887272, -58.227850790410685},
    {18, "87fe3tc177u8330hjv", 21.696859496979357, -165.16693817776797},
    {18, "pv5rgc6uhpx1jq6bj0", -60.50510177839641, 173.48906524511278},
    {18, "71m06pk755mfzk6hdc", -37.88444634554395, -37.87488612421896},
    {18, "d3e1esdzkr41p35q1b", 8.727173383051934, -74.37347947535784},
    {18, "3zm3e9jx0k7vgg026t", -3.9483595528182036, -93.70580989009056},
    {18, "13tcrjm87wvdqyg4ns", -81.3138547881087, -115.34873434504505},
    {18, "88dpse23bvqtg6ec5y", 4.14904170602, -154.48936172603283},
    {18, "6suhg5p9rrwwejuufh", -17.42955363988557, -61.732544394421325},
    {19, "2v32wm95nkp0ggud4d3", -15.35011038514007, -144.21587823406787},
    {19, "gv167nckx0neve4mfbm", 73.55835979142022, -9.358325590572818},
    {19, "7ffxusv8v400zrvd6xn", -28.142713141601163, -7.528853105285052},
    {19, "eqmhqhmdvds5frmmqy5", 35.927152407111365, -26.447283524149796},
    {19, "mcsj5hf86596evz0yrx", -35.65743908051637, 84.51035683870195},
    {19, "zke7y5dyjhw34xv55qm", 70.99196007862325, 151.08798399537955},
    {19, "eh01u9jf8v3se8dbmbc", 22.813568425235573, -44.79431532172306},
    {19, "zfcvj2btqhpw7ur8hvb", 61.35269987298038, 171.4426047536603},
    {20, "50kb4162ks10g95emn5v", -88.58681904037465, -38.22913735858451},
    {20, "vp5tcwqhv2wpvtf42e8n", 85.42089386634491, 49.99626630017035},
    {20, "0djemt78ev4qkwv96b8x", -78.14975809008581, -149.5189551167599},
    {20, "8rzxugxxpp8cb8s4gj1n", 44.97652292095542, -157.98378364345209},
    {20, "e8dg6qxny5mf6fhqpnu1", 3.4206711662393285, -18.524057185240235},
    {20, "kxtsr7j1fn3qm9z1em4t", -2.0486152031125204, 30.55993271757865},
    {20, "emsr9e2ukef1tre7dzww", 32.27446212698244, -27.706293537327777},
    {20, "rgv11mqxe8cczbq5m7y6", -23.700337859803284, 175.8452609157441},
};
static const size_t num_tests = sizeof(tests) / sizeof(*tests);

TEST test_geohash_encode_decode(void) {
    for (size_t i = 0; i < num_tests; i++) {
        double lat, lon;
        geohash_decode(&lat, &lon, tests[i].h, tests[i].len);
        ASSERT_EQ(tests[i].lat, lat);
        ASSERT_EQ(tests[i].lon, lon);
    }

    for (size_t i = 0; i < num_tests; i++) {
        char buf[22] = {0};
        geohash_encode(tests[i].lat, tests[i].lon, strlen(tests[i].h), buf);
        ASSERT_EQ(memcmp(buf, tests[i].h, strlen(tests[i].h)), 0);
    }

    PASS();
}


GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

    RUN_TEST(test_geohash_encode_decode);

    GREATEST_MAIN_END();        /* display results */
}

