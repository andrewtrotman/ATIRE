
/* This file was generated automatically by the Snowball to ANSI C compiler */

#include "../runtime/header.h"

#ifdef __cplusplus
extern "C" {
#endif
extern int basque_ISO_8859_1_stem(struct SN_env * z);
#ifdef __cplusplus
}
#endif
static int r_R1(struct SN_env * z);
static int r_R2(struct SN_env * z);
static int r_RV(struct SN_env * z);
static int r_mark_regions(struct SN_env * z);
static int r_adjetiboak(struct SN_env * z);
static int r_izenak(struct SN_env * z);
static int r_aditzak(struct SN_env * z);
#ifdef __cplusplus
extern "C" {
#endif


extern struct SN_env * basque_ISO_8859_1_create_env(void);
extern void basque_ISO_8859_1_close_env(struct SN_env * z);


#ifdef __cplusplus
}
#endif
static const symbol s_0_0[4] = { 'i', 'd', 'e', 'a' };
static const symbol s_0_1[5] = { 'b', 'i', 'd', 'e', 'a' };
static const symbol s_0_2[5] = { 'k', 'i', 'd', 'e', 'a' };
static const symbol s_0_3[5] = { 'p', 'i', 'd', 'e', 'a' };
static const symbol s_0_4[6] = { 'k', 'u', 'n', 'd', 'e', 'a' };
static const symbol s_0_5[5] = { 'g', 'a', 'l', 'e', 'a' };
static const symbol s_0_6[6] = { 't', 'a', 'i', 'l', 'e', 'a' };
static const symbol s_0_7[7] = { 't', 'z', 'a', 'i', 'l', 'e', 'a' };
static const symbol s_0_8[5] = { 'g', 'u', 'n', 'e', 'a' };
static const symbol s_0_9[5] = { 'k', 'u', 'n', 'e', 'a' };
static const symbol s_0_10[5] = { 't', 'z', 'a', 'g', 'a' };
static const symbol s_0_11[4] = { 'g', 'a', 'i', 'a' };
static const symbol s_0_12[5] = { 'a', 'l', 'd', 'i', 'a' };
static const symbol s_0_13[6] = { 't', 'a', 'l', 'd', 'i', 'a' };
static const symbol s_0_14[5] = { 'k', 'a', 'r', 'i', 'a' };
static const symbol s_0_15[6] = { 'g', 'a', 'r', 'r', 'i', 'a' };
static const symbol s_0_16[6] = { 'k', 'a', 'r', 'r', 'i', 'a' };
static const symbol s_0_17[2] = { 'k', 'a' };
static const symbol s_0_18[5] = { 't', 'z', 'a', 'k', 'a' };
static const symbol s_0_19[2] = { 'l', 'a' };
static const symbol s_0_20[4] = { 'm', 'e', 'n', 'a' };
static const symbol s_0_21[4] = { 'p', 'e', 'n', 'a' };
static const symbol s_0_22[4] = { 'k', 'i', 'n', 'a' };
static const symbol s_0_23[5] = { 'e', 'z', 'i', 'n', 'a' };
static const symbol s_0_24[6] = { 't', 'e', 'z', 'i', 'n', 'a' };
static const symbol s_0_25[4] = { 'k', 'u', 'n', 'a' };
static const symbol s_0_26[4] = { 't', 'u', 'n', 'a' };
static const symbol s_0_27[6] = { 'k', 'i', 'z', 'u', 'n', 'a' };
static const symbol s_0_28[3] = { 'e', 'r', 'a' };
static const symbol s_0_29[4] = { 'b', 'e', 'r', 'a' };
static const symbol s_0_30[7] = { 'a', 'r', 'a', 'b', 'e', 'r', 'a' };
static const symbol s_0_31[4] = { 'k', 'e', 'r', 'a' };
static const symbol s_0_32[4] = { 'p', 'e', 'r', 'a' };
static const symbol s_0_33[4] = { 'o', 'r', 'r', 'a' };
static const symbol s_0_34[5] = { 'k', 'o', 'r', 'r', 'a' };
static const symbol s_0_35[4] = { 'd', 'u', 'r', 'a' };
static const symbol s_0_36[4] = { 'g', 'u', 'r', 'a' };
static const symbol s_0_37[4] = { 'k', 'u', 'r', 'a' };
static const symbol s_0_38[4] = { 't', 'u', 'r', 'a' };
static const symbol s_0_39[3] = { 'e', 't', 'a' };
static const symbol s_0_40[4] = { 'k', 'e', 't', 'a' };
static const symbol s_0_41[6] = { 'g', 'a', 'i', 'l', 'u', 'a' };
static const symbol s_0_42[3] = { 'e', 'z', 'a' };
static const symbol s_0_43[6] = { 'e', 'r', 'r', 'e', 'z', 'a' };
static const symbol s_0_44[3] = { 't', 'z', 'a' };
static const symbol s_0_45[6] = { 'g', 'a', 'i', 't', 'z', 'a' };
static const symbol s_0_46[6] = { 'k', 'a', 'i', 't', 'z', 'a' };
static const symbol s_0_47[6] = { 'k', 'u', 'n', 't', 'z', 'a' };
static const symbol s_0_48[3] = { 'i', 'd', 'e' };
static const symbol s_0_49[4] = { 'b', 'i', 'd', 'e' };
static const symbol s_0_50[4] = { 'k', 'i', 'd', 'e' };
static const symbol s_0_51[4] = { 'p', 'i', 'd', 'e' };
static const symbol s_0_52[5] = { 'k', 'u', 'n', 'd', 'e' };
static const symbol s_0_53[5] = { 't', 'z', 'a', 'k', 'e' };
static const symbol s_0_54[5] = { 't', 'z', 'e', 'k', 'e' };
static const symbol s_0_55[2] = { 'l', 'e' };
static const symbol s_0_56[4] = { 'g', 'a', 'l', 'e' };
static const symbol s_0_57[5] = { 't', 'a', 'i', 'l', 'e' };
static const symbol s_0_58[6] = { 't', 'z', 'a', 'i', 'l', 'e' };
static const symbol s_0_59[4] = { 'g', 'u', 'n', 'e' };
static const symbol s_0_60[4] = { 'k', 'u', 'n', 'e' };
static const symbol s_0_61[3] = { 't', 'z', 'e' };
static const symbol s_0_62[4] = { 'a', 't', 'z', 'e' };
static const symbol s_0_63[3] = { 'g', 'a', 'i' };
static const symbol s_0_64[4] = { 'a', 'l', 'd', 'i' };
static const symbol s_0_65[5] = { 't', 'a', 'l', 'd', 'i' };
static const symbol s_0_66[2] = { 'k', 'i' };
static const symbol s_0_67[3] = { 'a', 'r', 'i' };
static const symbol s_0_68[4] = { 'k', 'a', 'r', 'i' };
static const symbol s_0_69[4] = { 'l', 'a', 'r', 'i' };
static const symbol s_0_70[4] = { 't', 'a', 'r', 'i' };
static const symbol s_0_71[5] = { 'e', 't', 'a', 'r', 'i' };
static const symbol s_0_72[5] = { 'g', 'a', 'r', 'r', 'i' };
static const symbol s_0_73[5] = { 'k', 'a', 'r', 'r', 'i' };
static const symbol s_0_74[5] = { 'a', 'r', 'a', 'z', 'i' };
static const symbol s_0_75[6] = { 't', 'a', 'r', 'a', 'z', 'i' };
static const symbol s_0_76[2] = { 'a', 'n' };
static const symbol s_0_77[3] = { 'e', 'a', 'n' };
static const symbol s_0_78[4] = { 'r', 'e', 'a', 'n' };
static const symbol s_0_79[3] = { 'k', 'a', 'n' };
static const symbol s_0_80[4] = { 'e', 't', 'a', 'n' };
static const symbol s_0_81[7] = { 'a', 't', 's', 'e', 'd', 'e', 'n' };
static const symbol s_0_82[3] = { 'm', 'e', 'n' };
static const symbol s_0_83[3] = { 'p', 'e', 'n' };
static const symbol s_0_84[3] = { 'k', 'i', 'n' };
static const symbol s_0_85[5] = { 'r', 'e', 'k', 'i', 'n' };
static const symbol s_0_86[4] = { 'e', 'z', 'i', 'n' };
static const symbol s_0_87[5] = { 't', 'e', 'z', 'i', 'n' };
static const symbol s_0_88[3] = { 't', 'u', 'n' };
static const symbol s_0_89[5] = { 'k', 'i', 'z', 'u', 'n' };
static const symbol s_0_90[2] = { 'g', 'o' };
static const symbol s_0_91[3] = { 'a', 'g', 'o' };
static const symbol s_0_92[3] = { 't', 'i', 'o' };
static const symbol s_0_93[4] = { 'd', 'a', 'k', 'o' };
static const symbol s_0_94[2] = { 'o', 'r' };
static const symbol s_0_95[3] = { 'k', 'o', 'r' };
static const symbol s_0_96[4] = { 't', 'z', 'a', 't' };
static const symbol s_0_97[2] = { 'd', 'u' };
static const symbol s_0_98[5] = { 'g', 'a', 'i', 'l', 'u' };
static const symbol s_0_99[2] = { 't', 'u' };
static const symbol s_0_100[3] = { 'a', 't', 'u' };
static const symbol s_0_101[6] = { 'a', 'l', 'd', 'a', 't', 'u' };
static const symbol s_0_102[4] = { 't', 'a', 't', 'u' };
static const symbol s_0_103[6] = { 'b', 'a', 'd', 'i', 't', 'u' };
static const symbol s_0_104[2] = { 'e', 'z' };
static const symbol s_0_105[5] = { 'e', 'r', 'r', 'e', 'z' };
static const symbol s_0_106[4] = { 't', 'z', 'e', 'z' };
static const symbol s_0_107[5] = { 'g', 'a', 'i', 't', 'z' };
static const symbol s_0_108[5] = { 'k', 'a', 'i', 't', 'z' };

static const struct among a_0[109] =
{
/*  0 */ { 4, s_0_0, -1, 1, 0},
/*  1 */ { 5, s_0_1, 0, 1, 0},
/*  2 */ { 5, s_0_2, 0, 1, 0},
/*  3 */ { 5, s_0_3, 0, 1, 0},
/*  4 */ { 6, s_0_4, -1, 1, 0},
/*  5 */ { 5, s_0_5, -1, 1, 0},
/*  6 */ { 6, s_0_6, -1, 1, 0},
/*  7 */ { 7, s_0_7, -1, 1, 0},
/*  8 */ { 5, s_0_8, -1, 1, 0},
/*  9 */ { 5, s_0_9, -1, 1, 0},
/* 10 */ { 5, s_0_10, -1, 1, 0},
/* 11 */ { 4, s_0_11, -1, 1, 0},
/* 12 */ { 5, s_0_12, -1, 1, 0},
/* 13 */ { 6, s_0_13, 12, 1, 0},
/* 14 */ { 5, s_0_14, -1, 1, 0},
/* 15 */ { 6, s_0_15, -1, 2, 0},
/* 16 */ { 6, s_0_16, -1, 1, 0},
/* 17 */ { 2, s_0_17, -1, 1, 0},
/* 18 */ { 5, s_0_18, 17, 1, 0},
/* 19 */ { 2, s_0_19, -1, 1, 0},
/* 20 */ { 4, s_0_20, -1, 1, 0},
/* 21 */ { 4, s_0_21, -1, 1, 0},
/* 22 */ { 4, s_0_22, -1, 1, 0},
/* 23 */ { 5, s_0_23, -1, 1, 0},
/* 24 */ { 6, s_0_24, 23, 1, 0},
/* 25 */ { 4, s_0_25, -1, 1, 0},
/* 26 */ { 4, s_0_26, -1, 1, 0},
/* 27 */ { 6, s_0_27, -1, 1, 0},
/* 28 */ { 3, s_0_28, -1, 1, 0},
/* 29 */ { 4, s_0_29, 28, 1, 0},
/* 30 */ { 7, s_0_30, 29, 4, 0},
/* 31 */ { 4, s_0_31, 28, 1, 0},
/* 32 */ { 4, s_0_32, 28, 1, 0},
/* 33 */ { 4, s_0_33, -1, 1, 0},
/* 34 */ { 5, s_0_34, 33, 1, 0},
/* 35 */ { 4, s_0_35, -1, 1, 0},
/* 36 */ { 4, s_0_36, -1, 1, 0},
/* 37 */ { 4, s_0_37, -1, 1, 0},
/* 38 */ { 4, s_0_38, -1, 1, 0},
/* 39 */ { 3, s_0_39, -1, 1, 0},
/* 40 */ { 4, s_0_40, 39, 1, 0},
/* 41 */ { 6, s_0_41, -1, 1, 0},
/* 42 */ { 3, s_0_42, -1, 1, 0},
/* 43 */ { 6, s_0_43, 42, 1, 0},
/* 44 */ { 3, s_0_44, -1, 2, 0},
/* 45 */ { 6, s_0_45, 44, 1, 0},
/* 46 */ { 6, s_0_46, 44, 1, 0},
/* 47 */ { 6, s_0_47, 44, 1, 0},
/* 48 */ { 3, s_0_48, -1, 1, 0},
/* 49 */ { 4, s_0_49, 48, 1, 0},
/* 50 */ { 4, s_0_50, 48, 1, 0},
/* 51 */ { 4, s_0_51, 48, 1, 0},
/* 52 */ { 5, s_0_52, -1, 1, 0},
/* 53 */ { 5, s_0_53, -1, 1, 0},
/* 54 */ { 5, s_0_54, -1, 1, 0},
/* 55 */ { 2, s_0_55, -1, 1, 0},
/* 56 */ { 4, s_0_56, 55, 1, 0},
/* 57 */ { 5, s_0_57, 55, 1, 0},
/* 58 */ { 6, s_0_58, 55, 1, 0},
/* 59 */ { 4, s_0_59, -1, 1, 0},
/* 60 */ { 4, s_0_60, -1, 1, 0},
/* 61 */ { 3, s_0_61, -1, 1, 0},
/* 62 */ { 4, s_0_62, 61, 1, 0},
/* 63 */ { 3, s_0_63, -1, 1, 0},
/* 64 */ { 4, s_0_64, -1, 1, 0},
/* 65 */ { 5, s_0_65, 64, 1, 0},
/* 66 */ { 2, s_0_66, -1, 1, 0},
/* 67 */ { 3, s_0_67, -1, 1, 0},
/* 68 */ { 4, s_0_68, 67, 1, 0},
/* 69 */ { 4, s_0_69, 67, 1, 0},
/* 70 */ { 4, s_0_70, 67, 1, 0},
/* 71 */ { 5, s_0_71, 70, 1, 0},
/* 72 */ { 5, s_0_72, -1, 2, 0},
/* 73 */ { 5, s_0_73, -1, 1, 0},
/* 74 */ { 5, s_0_74, -1, 1, 0},
/* 75 */ { 6, s_0_75, 74, 1, 0},
/* 76 */ { 2, s_0_76, -1, 1, 0},
/* 77 */ { 3, s_0_77, 76, 1, 0},
/* 78 */ { 4, s_0_78, 77, 1, 0},
/* 79 */ { 3, s_0_79, 76, 1, 0},
/* 80 */ { 4, s_0_80, 76, 1, 0},
/* 81 */ { 7, s_0_81, -1, 3, 0},
/* 82 */ { 3, s_0_82, -1, 1, 0},
/* 83 */ { 3, s_0_83, -1, 1, 0},
/* 84 */ { 3, s_0_84, -1, 1, 0},
/* 85 */ { 5, s_0_85, 84, 1, 0},
/* 86 */ { 4, s_0_86, -1, 1, 0},
/* 87 */ { 5, s_0_87, 86, 1, 0},
/* 88 */ { 3, s_0_88, -1, 1, 0},
/* 89 */ { 5, s_0_89, -1, 1, 0},
/* 90 */ { 2, s_0_90, -1, 1, 0},
/* 91 */ { 3, s_0_91, 90, 1, 0},
/* 92 */ { 3, s_0_92, -1, 1, 0},
/* 93 */ { 4, s_0_93, -1, 1, 0},
/* 94 */ { 2, s_0_94, -1, 1, 0},
/* 95 */ { 3, s_0_95, 94, 1, 0},
/* 96 */ { 4, s_0_96, -1, 1, 0},
/* 97 */ { 2, s_0_97, -1, 1, 0},
/* 98 */ { 5, s_0_98, -1, 1, 0},
/* 99 */ { 2, s_0_99, -1, 1, 0},
/*100 */ { 3, s_0_100, 99, 1, 0},
/*101 */ { 6, s_0_101, 100, 1, 0},
/*102 */ { 4, s_0_102, 100, 1, 0},
/*103 */ { 6, s_0_103, 99, 5, 0},
/*104 */ { 2, s_0_104, -1, 1, 0},
/*105 */ { 5, s_0_105, 104, 1, 0},
/*106 */ { 4, s_0_106, 104, 1, 0},
/*107 */ { 5, s_0_107, -1, 1, 0},
/*108 */ { 5, s_0_108, -1, 1, 0}
};

static const symbol s_1_0[3] = { 'a', 'd', 'a' };
static const symbol s_1_1[4] = { 'k', 'a', 'd', 'a' };
static const symbol s_1_2[4] = { 'a', 'n', 'd', 'a' };
static const symbol s_1_3[5] = { 'd', 'e', 'n', 'd', 'a' };
static const symbol s_1_4[5] = { 'g', 'a', 'b', 'e', 'a' };
static const symbol s_1_5[5] = { 'k', 'a', 'b', 'e', 'a' };
static const symbol s_1_6[5] = { 'a', 'l', 'd', 'e', 'a' };
static const symbol s_1_7[6] = { 'k', 'a', 'l', 'd', 'e', 'a' };
static const symbol s_1_8[6] = { 't', 'a', 'l', 'd', 'e', 'a' };
static const symbol s_1_9[5] = { 'o', 'r', 'd', 'e', 'a' };
static const symbol s_1_10[5] = { 'z', 'a', 'l', 'e', 'a' };
static const symbol s_1_11[6] = { 't', 'z', 'a', 'l', 'e', 'a' };
static const symbol s_1_12[5] = { 'g', 'i', 'l', 'e', 'a' };
static const symbol s_1_13[4] = { 'e', 'm', 'e', 'a' };
static const symbol s_1_14[5] = { 'k', 'u', 'm', 'e', 'a' };
static const symbol s_1_15[3] = { 'n', 'e', 'a' };
static const symbol s_1_16[4] = { 'e', 'n', 'e', 'a' };
static const symbol s_1_17[6] = { 'z', 'i', 'o', 'n', 'e', 'a' };
static const symbol s_1_18[4] = { 'u', 'n', 'e', 'a' };
static const symbol s_1_19[5] = { 'g', 'u', 'n', 'e', 'a' };
static const symbol s_1_20[3] = { 'p', 'e', 'a' };
static const symbol s_1_21[6] = { 'a', 'u', 'r', 'r', 'e', 'a' };
static const symbol s_1_22[3] = { 't', 'e', 'a' };
static const symbol s_1_23[5] = { 'k', 'o', 't', 'e', 'a' };
static const symbol s_1_24[5] = { 'a', 'r', 't', 'e', 'a' };
static const symbol s_1_25[5] = { 'o', 's', 't', 'e', 'a' };
static const symbol s_1_26[5] = { 'e', 't', 'x', 'e', 'a' };
static const symbol s_1_27[2] = { 'g', 'a' };
static const symbol s_1_28[4] = { 'a', 'n', 'g', 'a' };
static const symbol s_1_29[4] = { 'g', 'a', 'i', 'a' };
static const symbol s_1_30[5] = { 'a', 'l', 'd', 'i', 'a' };
static const symbol s_1_31[6] = { 't', 'a', 'l', 'd', 'i', 'a' };
static const symbol s_1_32[6] = { 'h', 'a', 'n', 'd', 'i', 'a' };
static const symbol s_1_33[6] = { 'm', 'e', 'n', 'd', 'i', 'a' };
static const symbol s_1_34[4] = { 'g', 'e', 'i', 'a' };
static const symbol s_1_35[4] = { 'e', 'g', 'i', 'a' };
static const symbol s_1_36[5] = { 'd', 'e', 'g', 'i', 'a' };
static const symbol s_1_37[5] = { 't', 'e', 'g', 'i', 'a' };
static const symbol s_1_38[5] = { 'n', 'a', 'h', 'i', 'a' };
static const symbol s_1_39[4] = { 'o', 'h', 'i', 'a' };
static const symbol s_1_40[3] = { 'k', 'i', 'a' };
static const symbol s_1_41[5] = { 't', 'o', 'k', 'i', 'a' };
static const symbol s_1_42[3] = { 'o', 'i', 'a' };
static const symbol s_1_43[4] = { 'k', 'o', 'i', 'a' };
static const symbol s_1_44[4] = { 'a', 'r', 'i', 'a' };
static const symbol s_1_45[5] = { 'k', 'a', 'r', 'i', 'a' };
static const symbol s_1_46[5] = { 'l', 'a', 'r', 'i', 'a' };
static const symbol s_1_47[5] = { 't', 'a', 'r', 'i', 'a' };
static const symbol s_1_48[4] = { 'e', 'r', 'i', 'a' };
static const symbol s_1_49[5] = { 'k', 'e', 'r', 'i', 'a' };
static const symbol s_1_50[5] = { 't', 'e', 'r', 'i', 'a' };
static const symbol s_1_51[6] = { 'g', 'a', 'r', 'r', 'i', 'a' };
static const symbol s_1_52[6] = { 'l', 'a', 'r', 'r', 'i', 'a' };
static const symbol s_1_53[6] = { 'k', 'i', 'r', 'r', 'i', 'a' };
static const symbol s_1_54[5] = { 'd', 'u', 'r', 'i', 'a' };
static const symbol s_1_55[4] = { 'a', 's', 'i', 'a' };
static const symbol s_1_56[3] = { 't', 'i', 'a' };
static const symbol s_1_57[4] = { 'e', 'z', 'i', 'a' };
static const symbol s_1_58[5] = { 'b', 'i', 'z', 'i', 'a' };
static const symbol s_1_59[6] = { 'o', 'n', 't', 'z', 'i', 'a' };
static const symbol s_1_60[2] = { 'k', 'a' };
static const symbol s_1_61[4] = { 'j', 'o', 'k', 'a' };
static const symbol s_1_62[5] = { 'a', 'u', 'r', 'k', 'a' };
static const symbol s_1_63[3] = { 's', 'k', 'a' };
static const symbol s_1_64[3] = { 'x', 'k', 'a' };
static const symbol s_1_65[3] = { 'z', 'k', 'a' };
static const symbol s_1_66[6] = { 'g', 'i', 'b', 'e', 'l', 'a' };
static const symbol s_1_67[4] = { 'g', 'e', 'l', 'a' };
static const symbol s_1_68[5] = { 'k', 'a', 'i', 'l', 'a' };
static const symbol s_1_69[5] = { 's', 'k', 'i', 'l', 'a' };
static const symbol s_1_70[4] = { 't', 'i', 'l', 'a' };
static const symbol s_1_71[3] = { 'o', 'l', 'a' };
static const symbol s_1_72[2] = { 'n', 'a' };
static const symbol s_1_73[4] = { 'k', 'a', 'n', 'a' };
static const symbol s_1_74[3] = { 'e', 'n', 'a' };
static const symbol s_1_75[7] = { 'g', 'a', 'r', 'r', 'e', 'n', 'a' };
static const symbol s_1_76[7] = { 'g', 'e', 'r', 'r', 'e', 'n', 'a' };
static const symbol s_1_77[6] = { 'u', 'r', 'r', 'e', 'n', 'a' };
static const symbol s_1_78[5] = { 'z', 'a', 'i', 'n', 'a' };
static const symbol s_1_79[6] = { 't', 'z', 'a', 'i', 'n', 'a' };
static const symbol s_1_80[4] = { 'k', 'i', 'n', 'a' };
static const symbol s_1_81[4] = { 'm', 'i', 'n', 'a' };
static const symbol s_1_82[5] = { 'g', 'a', 'r', 'n', 'a' };
static const symbol s_1_83[3] = { 'u', 'n', 'a' };
static const symbol s_1_84[4] = { 'd', 'u', 'n', 'a' };
static const symbol s_1_85[5] = { 'a', 's', 'u', 'n', 'a' };
static const symbol s_1_86[6] = { 't', 'a', 's', 'u', 'n', 'a' };
static const symbol s_1_87[5] = { 'o', 'n', 'd', 'o', 'a' };
static const symbol s_1_88[6] = { 'k', 'o', 'n', 'd', 'o', 'a' };
static const symbol s_1_89[4] = { 'n', 'g', 'o', 'a' };
static const symbol s_1_90[4] = { 'z', 'i', 'o', 'a' };
static const symbol s_1_91[3] = { 'k', 'o', 'a' };
static const symbol s_1_92[5] = { 't', 'a', 'k', 'o', 'a' };
static const symbol s_1_93[4] = { 'z', 'k', 'o', 'a' };
static const symbol s_1_94[3] = { 'n', 'o', 'a' };
static const symbol s_1_95[5] = { 'z', 'i', 'n', 'o', 'a' };
static const symbol s_1_96[4] = { 'a', 'r', 'o', 'a' };
static const symbol s_1_97[5] = { 't', 'a', 'r', 'o', 'a' };
static const symbol s_1_98[5] = { 'z', 'a', 'r', 'o', 'a' };
static const symbol s_1_99[4] = { 'e', 'r', 'o', 'a' };
static const symbol s_1_100[4] = { 'o', 'r', 'o', 'a' };
static const symbol s_1_101[4] = { 'o', 's', 'o', 'a' };
static const symbol s_1_102[3] = { 't', 'o', 'a' };
static const symbol s_1_103[4] = { 't', 't', 'o', 'a' };
static const symbol s_1_104[4] = { 'z', 't', 'o', 'a' };
static const symbol s_1_105[4] = { 't', 'x', 'o', 'a' };
static const symbol s_1_106[4] = { 't', 'z', 'o', 'a' };
static const symbol s_1_107[4] = { 0xC3, 0xB1, 'o', 'a' };
static const symbol s_1_108[2] = { 'r', 'a' };
static const symbol s_1_109[3] = { 'a', 'r', 'a' };
static const symbol s_1_110[4] = { 'd', 'a', 'r', 'a' };
static const symbol s_1_111[5] = { 'l', 'i', 'a', 'r', 'a' };
static const symbol s_1_112[5] = { 't', 'i', 'a', 'r', 'a' };
static const symbol s_1_113[4] = { 't', 'a', 'r', 'a' };
static const symbol s_1_114[5] = { 'e', 't', 'a', 'r', 'a' };
static const symbol s_1_115[5] = { 't', 'z', 'a', 'r', 'a' };
static const symbol s_1_116[4] = { 'b', 'e', 'r', 'a' };
static const symbol s_1_117[4] = { 'k', 'e', 'r', 'a' };
static const symbol s_1_118[4] = { 'p', 'e', 'r', 'a' };
static const symbol s_1_119[3] = { 'o', 'r', 'a' };
static const symbol s_1_120[6] = { 't', 'z', 'a', 'r', 'r', 'a' };
static const symbol s_1_121[5] = { 'k', 'o', 'r', 'r', 'a' };
static const symbol s_1_122[3] = { 't', 'r', 'a' };
static const symbol s_1_123[2] = { 's', 'a' };
static const symbol s_1_124[3] = { 'o', 's', 'a' };
static const symbol s_1_125[2] = { 't', 'a' };
static const symbol s_1_126[3] = { 'e', 't', 'a' };
static const symbol s_1_127[4] = { 'k', 'e', 't', 'a' };
static const symbol s_1_128[3] = { 's', 't', 'a' };
static const symbol s_1_129[3] = { 'd', 'u', 'a' };
static const symbol s_1_130[6] = { 'm', 'e', 'n', 'd', 'u', 'a' };
static const symbol s_1_131[5] = { 'o', 'r', 'd', 'u', 'a' };
static const symbol s_1_132[5] = { 'l', 'e', 'k', 'u', 'a' };
static const symbol s_1_133[5] = { 'b', 'u', 'r', 'u', 'a' };
static const symbol s_1_134[5] = { 'd', 'u', 'r', 'u', 'a' };
static const symbol s_1_135[4] = { 't', 's', 'u', 'a' };
static const symbol s_1_136[3] = { 't', 'u', 'a' };
static const symbol s_1_137[6] = { 'm', 'e', 'n', 't', 'u', 'a' };
static const symbol s_1_138[5] = { 'e', 's', 't', 'u', 'a' };
static const symbol s_1_139[4] = { 't', 'x', 'u', 'a' };
static const symbol s_1_140[3] = { 'z', 'u', 'a' };
static const symbol s_1_141[4] = { 't', 'z', 'u', 'a' };
static const symbol s_1_142[2] = { 'z', 'a' };
static const symbol s_1_143[3] = { 'e', 'z', 'a' };
static const symbol s_1_144[5] = { 'e', 'r', 'o', 'z', 'a' };
static const symbol s_1_145[3] = { 't', 'z', 'a' };
static const symbol s_1_146[6] = { 'k', 'o', 'i', 't', 'z', 'a' };
static const symbol s_1_147[5] = { 'a', 'n', 't', 'z', 'a' };
static const symbol s_1_148[6] = { 'g', 'i', 'n', 't', 'z', 'a' };
static const symbol s_1_149[6] = { 'k', 'i', 'n', 't', 'z', 'a' };
static const symbol s_1_150[6] = { 'k', 'u', 'n', 't', 'z', 'a' };
static const symbol s_1_151[4] = { 'g', 'a', 'b', 'e' };
static const symbol s_1_152[4] = { 'k', 'a', 'b', 'e' };
static const symbol s_1_153[4] = { 'k', 'i', 'd', 'e' };
static const symbol s_1_154[4] = { 'a', 'l', 'd', 'e' };
static const symbol s_1_155[5] = { 'k', 'a', 'l', 'd', 'e' };
static const symbol s_1_156[5] = { 't', 'a', 'l', 'd', 'e' };
static const symbol s_1_157[4] = { 'o', 'r', 'd', 'e' };
static const symbol s_1_158[2] = { 'g', 'e' };
static const symbol s_1_159[4] = { 'z', 'a', 'l', 'e' };
static const symbol s_1_160[5] = { 't', 'z', 'a', 'l', 'e' };
static const symbol s_1_161[4] = { 'g', 'i', 'l', 'e' };
static const symbol s_1_162[3] = { 'e', 'm', 'e' };
static const symbol s_1_163[4] = { 'k', 'u', 'm', 'e' };
static const symbol s_1_164[2] = { 'n', 'e' };
static const symbol s_1_165[5] = { 'z', 'i', 'o', 'n', 'e' };
static const symbol s_1_166[3] = { 'u', 'n', 'e' };
static const symbol s_1_167[4] = { 'g', 'u', 'n', 'e' };
static const symbol s_1_168[2] = { 'p', 'e' };
static const symbol s_1_169[5] = { 'a', 'u', 'r', 'r', 'e' };
static const symbol s_1_170[2] = { 't', 'e' };
static const symbol s_1_171[4] = { 'k', 'o', 't', 'e' };
static const symbol s_1_172[4] = { 'a', 'r', 't', 'e' };
static const symbol s_1_173[4] = { 'o', 's', 't', 'e' };
static const symbol s_1_174[4] = { 'e', 't', 'x', 'e' };
static const symbol s_1_175[3] = { 'g', 'a', 'i' };
static const symbol s_1_176[2] = { 'd', 'i' };
static const symbol s_1_177[4] = { 'a', 'l', 'd', 'i' };
static const symbol s_1_178[5] = { 't', 'a', 'l', 'd', 'i' };
static const symbol s_1_179[5] = { 'g', 'e', 'l', 'd', 'i' };
static const symbol s_1_180[5] = { 'h', 'a', 'n', 'd', 'i' };
static const symbol s_1_181[5] = { 'm', 'e', 'n', 'd', 'i' };
static const symbol s_1_182[3] = { 'g', 'e', 'i' };
static const symbol s_1_183[3] = { 'e', 'g', 'i' };
static const symbol s_1_184[4] = { 'd', 'e', 'g', 'i' };
static const symbol s_1_185[4] = { 't', 'e', 'g', 'i' };
static const symbol s_1_186[4] = { 'n', 'a', 'h', 'i' };
static const symbol s_1_187[3] = { 'o', 'h', 'i' };
static const symbol s_1_188[2] = { 'k', 'i' };
static const symbol s_1_189[4] = { 't', 'o', 'k', 'i' };
static const symbol s_1_190[2] = { 'o', 'i' };
static const symbol s_1_191[3] = { 'g', 'o', 'i' };
static const symbol s_1_192[3] = { 'k', 'o', 'i' };
static const symbol s_1_193[3] = { 'a', 'r', 'i' };
static const symbol s_1_194[4] = { 'k', 'a', 'r', 'i' };
static const symbol s_1_195[4] = { 'l', 'a', 'r', 'i' };
static const symbol s_1_196[4] = { 't', 'a', 'r', 'i' };
static const symbol s_1_197[5] = { 'g', 'a', 'r', 'r', 'i' };
static const symbol s_1_198[5] = { 'l', 'a', 'r', 'r', 'i' };
static const symbol s_1_199[5] = { 'k', 'i', 'r', 'r', 'i' };
static const symbol s_1_200[4] = { 'd', 'u', 'r', 'i' };
static const symbol s_1_201[3] = { 'a', 's', 'i' };
static const symbol s_1_202[2] = { 't', 'i' };
static const symbol s_1_203[5] = { 'o', 'n', 't', 'z', 'i' };
static const symbol s_1_204[3] = { 0xC3, 0xB1, 'i' };
static const symbol s_1_205[2] = { 'a', 'k' };
static const symbol s_1_206[2] = { 'e', 'k' };
static const symbol s_1_207[5] = { 't', 'a', 'r', 'i', 'k' };
static const symbol s_1_208[5] = { 'g', 'i', 'b', 'e', 'l' };
static const symbol s_1_209[3] = { 'a', 'i', 'l' };
static const symbol s_1_210[4] = { 'k', 'a', 'i', 'l' };
static const symbol s_1_211[3] = { 'k', 'a', 'n' };
static const symbol s_1_212[3] = { 't', 'a', 'n' };
static const symbol s_1_213[4] = { 'e', 't', 'a', 'n' };
static const symbol s_1_214[2] = { 'e', 'n' };
static const symbol s_1_215[3] = { 'r', 'e', 'n' };
static const symbol s_1_216[6] = { 'g', 'a', 'r', 'r', 'e', 'n' };
static const symbol s_1_217[6] = { 'g', 'e', 'r', 'r', 'e', 'n' };
static const symbol s_1_218[5] = { 'u', 'r', 'r', 'e', 'n' };
static const symbol s_1_219[3] = { 't', 'e', 'n' };
static const symbol s_1_220[4] = { 't', 'z', 'e', 'n' };
static const symbol s_1_221[4] = { 'z', 'a', 'i', 'n' };
static const symbol s_1_222[5] = { 't', 'z', 'a', 'i', 'n' };
static const symbol s_1_223[3] = { 'k', 'i', 'n' };
static const symbol s_1_224[3] = { 'm', 'i', 'n' };
static const symbol s_1_225[3] = { 'd', 'u', 'n' };
static const symbol s_1_226[4] = { 'a', 's', 'u', 'n' };
static const symbol s_1_227[5] = { 't', 'a', 's', 'u', 'n' };
static const symbol s_1_228[5] = { 'a', 'i', 'z', 'u', 'n' };
static const symbol s_1_229[4] = { 'o', 'n', 'd', 'o' };
static const symbol s_1_230[5] = { 'k', 'o', 'n', 'd', 'o' };
static const symbol s_1_231[2] = { 'g', 'o' };
static const symbol s_1_232[3] = { 'n', 'g', 'o' };
static const symbol s_1_233[3] = { 'z', 'i', 'o' };
static const symbol s_1_234[2] = { 'k', 'o' };
static const symbol s_1_235[5] = { 't', 'r', 'a', 'k', 'o' };
static const symbol s_1_236[4] = { 't', 'a', 'k', 'o' };
static const symbol s_1_237[5] = { 'e', 't', 'a', 'k', 'o' };
static const symbol s_1_238[3] = { 'e', 'k', 'o' };
static const symbol s_1_239[6] = { 't', 'a', 'r', 'i', 'k', 'o' };
static const symbol s_1_240[3] = { 's', 'k', 'o' };
static const symbol s_1_241[4] = { 't', 'u', 'k', 'o' };
static const symbol s_1_242[8] = { 'm', 'i', 'n', 'u', 't', 'u', 'k', 'o' };
static const symbol s_1_243[3] = { 'z', 'k', 'o' };
static const symbol s_1_244[2] = { 'n', 'o' };
static const symbol s_1_245[4] = { 'z', 'i', 'n', 'o' };
static const symbol s_1_246[2] = { 'r', 'o' };
static const symbol s_1_247[3] = { 'a', 'r', 'o' };
static const symbol s_1_248[5] = { 'i', 'g', 'a', 'r', 'o' };
static const symbol s_1_249[4] = { 't', 'a', 'r', 'o' };
static const symbol s_1_250[4] = { 'z', 'a', 'r', 'o' };
static const symbol s_1_251[3] = { 'e', 'r', 'o' };
static const symbol s_1_252[4] = { 'g', 'i', 'r', 'o' };
static const symbol s_1_253[3] = { 'o', 'r', 'o' };
static const symbol s_1_254[3] = { 'o', 's', 'o' };
static const symbol s_1_255[2] = { 't', 'o' };
static const symbol s_1_256[3] = { 't', 't', 'o' };
static const symbol s_1_257[3] = { 'z', 't', 'o' };
static const symbol s_1_258[3] = { 't', 'x', 'o' };
static const symbol s_1_259[3] = { 't', 'z', 'o' };
static const symbol s_1_260[6] = { 'g', 'i', 'n', 't', 'z', 'o' };
static const symbol s_1_261[3] = { 0xC3, 0xB1, 'o' };
static const symbol s_1_262[2] = { 'z', 'p' };
static const symbol s_1_263[2] = { 'a', 'r' };
static const symbol s_1_264[3] = { 'd', 'a', 'r' };
static const symbol s_1_265[5] = { 'b', 'e', 'h', 'a', 'r' };
static const symbol s_1_266[5] = { 'z', 'e', 'h', 'a', 'r' };
static const symbol s_1_267[4] = { 'l', 'i', 'a', 'r' };
static const symbol s_1_268[4] = { 't', 'i', 'a', 'r' };
static const symbol s_1_269[3] = { 't', 'a', 'r' };
static const symbol s_1_270[4] = { 't', 'z', 'a', 'r' };
static const symbol s_1_271[2] = { 'o', 'r' };
static const symbol s_1_272[3] = { 'k', 'o', 'r' };
static const symbol s_1_273[2] = { 'o', 's' };
static const symbol s_1_274[3] = { 'k', 'e', 't' };
static const symbol s_1_275[2] = { 'd', 'u' };
static const symbol s_1_276[5] = { 'm', 'e', 'n', 'd', 'u' };
static const symbol s_1_277[4] = { 'o', 'r', 'd', 'u' };
static const symbol s_1_278[4] = { 'l', 'e', 'k', 'u' };
static const symbol s_1_279[4] = { 'b', 'u', 'r', 'u' };
static const symbol s_1_280[4] = { 'd', 'u', 'r', 'u' };
static const symbol s_1_281[3] = { 't', 's', 'u' };
static const symbol s_1_282[2] = { 't', 'u' };
static const symbol s_1_283[4] = { 't', 'a', 't', 'u' };
static const symbol s_1_284[5] = { 'm', 'e', 'n', 't', 'u' };
static const symbol s_1_285[4] = { 'e', 's', 't', 'u' };
static const symbol s_1_286[3] = { 't', 'x', 'u' };
static const symbol s_1_287[2] = { 'z', 'u' };
static const symbol s_1_288[3] = { 't', 'z', 'u' };
static const symbol s_1_289[6] = { 'g', 'i', 'n', 't', 'z', 'u' };
static const symbol s_1_290[1] = { 'z' };
static const symbol s_1_291[2] = { 'e', 'z' };
static const symbol s_1_292[4] = { 'e', 'r', 'o', 'z' };
static const symbol s_1_293[2] = { 't', 'z' };
static const symbol s_1_294[5] = { 'k', 'o', 'i', 't', 'z' };

static const struct among a_1[295] =
{
/*  0 */ { 3, s_1_0, -1, 1, 0},
/*  1 */ { 4, s_1_1, 0, 1, 0},
/*  2 */ { 4, s_1_2, -1, 1, 0},
/*  3 */ { 5, s_1_3, -1, 1, 0},
/*  4 */ { 5, s_1_4, -1, 1, 0},
/*  5 */ { 5, s_1_5, -1, 1, 0},
/*  6 */ { 5, s_1_6, -1, 1, 0},
/*  7 */ { 6, s_1_7, 6, 1, 0},
/*  8 */ { 6, s_1_8, 6, 1, 0},
/*  9 */ { 5, s_1_9, -1, 1, 0},
/* 10 */ { 5, s_1_10, -1, 1, 0},
/* 11 */ { 6, s_1_11, 10, 1, 0},
/* 12 */ { 5, s_1_12, -1, 1, 0},
/* 13 */ { 4, s_1_13, -1, 1, 0},
/* 14 */ { 5, s_1_14, -1, 1, 0},
/* 15 */ { 3, s_1_15, -1, 1, 0},
/* 16 */ { 4, s_1_16, 15, 1, 0},
/* 17 */ { 6, s_1_17, 15, 1, 0},
/* 18 */ { 4, s_1_18, 15, 1, 0},
/* 19 */ { 5, s_1_19, 18, 1, 0},
/* 20 */ { 3, s_1_20, -1, 1, 0},
/* 21 */ { 6, s_1_21, -1, 1, 0},
/* 22 */ { 3, s_1_22, -1, 1, 0},
/* 23 */ { 5, s_1_23, 22, 1, 0},
/* 24 */ { 5, s_1_24, 22, 1, 0},
/* 25 */ { 5, s_1_25, 22, 1, 0},
/* 26 */ { 5, s_1_26, -1, 1, 0},
/* 27 */ { 2, s_1_27, -1, 1, 0},
/* 28 */ { 4, s_1_28, 27, 1, 0},
/* 29 */ { 4, s_1_29, -1, 1, 0},
/* 30 */ { 5, s_1_30, -1, 1, 0},
/* 31 */ { 6, s_1_31, 30, 1, 0},
/* 32 */ { 6, s_1_32, -1, 1, 0},
/* 33 */ { 6, s_1_33, -1, 1, 0},
/* 34 */ { 4, s_1_34, -1, 1, 0},
/* 35 */ { 4, s_1_35, -1, 1, 0},
/* 36 */ { 5, s_1_36, 35, 1, 0},
/* 37 */ { 5, s_1_37, 35, 1, 0},
/* 38 */ { 5, s_1_38, -1, 1, 0},
/* 39 */ { 4, s_1_39, -1, 1, 0},
/* 40 */ { 3, s_1_40, -1, 1, 0},
/* 41 */ { 5, s_1_41, 40, 1, 0},
/* 42 */ { 3, s_1_42, -1, 1, 0},
/* 43 */ { 4, s_1_43, 42, 1, 0},
/* 44 */ { 4, s_1_44, -1, 1, 0},
/* 45 */ { 5, s_1_45, 44, 1, 0},
/* 46 */ { 5, s_1_46, 44, 1, 0},
/* 47 */ { 5, s_1_47, 44, 1, 0},
/* 48 */ { 4, s_1_48, -1, 1, 0},
/* 49 */ { 5, s_1_49, 48, 1, 0},
/* 50 */ { 5, s_1_50, 48, 1, 0},
/* 51 */ { 6, s_1_51, -1, 2, 0},
/* 52 */ { 6, s_1_52, -1, 1, 0},
/* 53 */ { 6, s_1_53, -1, 1, 0},
/* 54 */ { 5, s_1_54, -1, 1, 0},
/* 55 */ { 4, s_1_55, -1, 1, 0},
/* 56 */ { 3, s_1_56, -1, 1, 0},
/* 57 */ { 4, s_1_57, -1, 1, 0},
/* 58 */ { 5, s_1_58, -1, 1, 0},
/* 59 */ { 6, s_1_59, -1, 1, 0},
/* 60 */ { 2, s_1_60, -1, 1, 0},
/* 61 */ { 4, s_1_61, 60, 3, 0},
/* 62 */ { 5, s_1_62, 60, 10, 0},
/* 63 */ { 3, s_1_63, 60, 1, 0},
/* 64 */ { 3, s_1_64, 60, 1, 0},
/* 65 */ { 3, s_1_65, 60, 1, 0},
/* 66 */ { 6, s_1_66, -1, 1, 0},
/* 67 */ { 4, s_1_67, -1, 1, 0},
/* 68 */ { 5, s_1_68, -1, 1, 0},
/* 69 */ { 5, s_1_69, -1, 1, 0},
/* 70 */ { 4, s_1_70, -1, 1, 0},
/* 71 */ { 3, s_1_71, -1, 1, 0},
/* 72 */ { 2, s_1_72, -1, 1, 0},
/* 73 */ { 4, s_1_73, 72, 1, 0},
/* 74 */ { 3, s_1_74, 72, 1, 0},
/* 75 */ { 7, s_1_75, 74, 1, 0},
/* 76 */ { 7, s_1_76, 74, 1, 0},
/* 77 */ { 6, s_1_77, 74, 1, 0},
/* 78 */ { 5, s_1_78, 72, 1, 0},
/* 79 */ { 6, s_1_79, 78, 1, 0},
/* 80 */ { 4, s_1_80, 72, 1, 0},
/* 81 */ { 4, s_1_81, 72, 1, 0},
/* 82 */ { 5, s_1_82, 72, 1, 0},
/* 83 */ { 3, s_1_83, 72, 1, 0},
/* 84 */ { 4, s_1_84, 83, 1, 0},
/* 85 */ { 5, s_1_85, 83, 1, 0},
/* 86 */ { 6, s_1_86, 85, 1, 0},
/* 87 */ { 5, s_1_87, -1, 1, 0},
/* 88 */ { 6, s_1_88, 87, 1, 0},
/* 89 */ { 4, s_1_89, -1, 1, 0},
/* 90 */ { 4, s_1_90, -1, 1, 0},
/* 91 */ { 3, s_1_91, -1, 1, 0},
/* 92 */ { 5, s_1_92, 91, 1, 0},
/* 93 */ { 4, s_1_93, 91, 1, 0},
/* 94 */ { 3, s_1_94, -1, 1, 0},
/* 95 */ { 5, s_1_95, 94, 1, 0},
/* 96 */ { 4, s_1_96, -1, 1, 0},
/* 97 */ { 5, s_1_97, 96, 1, 0},
/* 98 */ { 5, s_1_98, 96, 1, 0},
/* 99 */ { 4, s_1_99, -1, 1, 0},
/*100 */ { 4, s_1_100, -1, 1, 0},
/*101 */ { 4, s_1_101, -1, 1, 0},
/*102 */ { 3, s_1_102, -1, 1, 0},
/*103 */ { 4, s_1_103, 102, 1, 0},
/*104 */ { 4, s_1_104, 102, 1, 0},
/*105 */ { 4, s_1_105, -1, 1, 0},
/*106 */ { 4, s_1_106, -1, 1, 0},
/*107 */ { 4, s_1_107, -1, 1, 0},
/*108 */ { 2, s_1_108, -1, 1, 0},
/*109 */ { 3, s_1_109, 108, 1, 0},
/*110 */ { 4, s_1_110, 109, 1, 0},
/*111 */ { 5, s_1_111, 109, 1, 0},
/*112 */ { 5, s_1_112, 109, 1, 0},
/*113 */ { 4, s_1_113, 109, 1, 0},
/*114 */ { 5, s_1_114, 113, 1, 0},
/*115 */ { 5, s_1_115, 109, 1, 0},
/*116 */ { 4, s_1_116, 108, 1, 0},
/*117 */ { 4, s_1_117, 108, 1, 0},
/*118 */ { 4, s_1_118, 108, 1, 0},
/*119 */ { 3, s_1_119, 108, 2, 0},
/*120 */ { 6, s_1_120, 108, 1, 0},
/*121 */ { 5, s_1_121, 108, 1, 0},
/*122 */ { 3, s_1_122, 108, 1, 0},
/*123 */ { 2, s_1_123, -1, 1, 0},
/*124 */ { 3, s_1_124, 123, 1, 0},
/*125 */ { 2, s_1_125, -1, 1, 0},
/*126 */ { 3, s_1_126, 125, 1, 0},
/*127 */ { 4, s_1_127, 126, 1, 0},
/*128 */ { 3, s_1_128, 125, 1, 0},
/*129 */ { 3, s_1_129, -1, 1, 0},
/*130 */ { 6, s_1_130, 129, 1, 0},
/*131 */ { 5, s_1_131, 129, 1, 0},
/*132 */ { 5, s_1_132, -1, 1, 0},
/*133 */ { 5, s_1_133, -1, 1, 0},
/*134 */ { 5, s_1_134, -1, 1, 0},
/*135 */ { 4, s_1_135, -1, 1, 0},
/*136 */ { 3, s_1_136, -1, 1, 0},
/*137 */ { 6, s_1_137, 136, 1, 0},
/*138 */ { 5, s_1_138, 136, 1, 0},
/*139 */ { 4, s_1_139, -1, 1, 0},
/*140 */ { 3, s_1_140, -1, 1, 0},
/*141 */ { 4, s_1_141, 140, 1, 0},
/*142 */ { 2, s_1_142, -1, 1, 0},
/*143 */ { 3, s_1_143, 142, 1, 0},
/*144 */ { 5, s_1_144, 142, 1, 0},
/*145 */ { 3, s_1_145, 142, 2, 0},
/*146 */ { 6, s_1_146, 145, 1, 0},
/*147 */ { 5, s_1_147, 145, 1, 0},
/*148 */ { 6, s_1_148, 145, 1, 0},
/*149 */ { 6, s_1_149, 145, 1, 0},
/*150 */ { 6, s_1_150, 145, 1, 0},
/*151 */ { 4, s_1_151, -1, 1, 0},
/*152 */ { 4, s_1_152, -1, 1, 0},
/*153 */ { 4, s_1_153, -1, 1, 0},
/*154 */ { 4, s_1_154, -1, 1, 0},
/*155 */ { 5, s_1_155, 154, 1, 0},
/*156 */ { 5, s_1_156, 154, 1, 0},
/*157 */ { 4, s_1_157, -1, 1, 0},
/*158 */ { 2, s_1_158, -1, 1, 0},
/*159 */ { 4, s_1_159, -1, 1, 0},
/*160 */ { 5, s_1_160, 159, 1, 0},
/*161 */ { 4, s_1_161, -1, 1, 0},
/*162 */ { 3, s_1_162, -1, 1, 0},
/*163 */ { 4, s_1_163, -1, 1, 0},
/*164 */ { 2, s_1_164, -1, 1, 0},
/*165 */ { 5, s_1_165, 164, 1, 0},
/*166 */ { 3, s_1_166, 164, 1, 0},
/*167 */ { 4, s_1_167, 166, 1, 0},
/*168 */ { 2, s_1_168, -1, 1, 0},
/*169 */ { 5, s_1_169, -1, 1, 0},
/*170 */ { 2, s_1_170, -1, 1, 0},
/*171 */ { 4, s_1_171, 170, 1, 0},
/*172 */ { 4, s_1_172, 170, 1, 0},
/*173 */ { 4, s_1_173, 170, 1, 0},
/*174 */ { 4, s_1_174, -1, 1, 0},
/*175 */ { 3, s_1_175, -1, 1, 0},
/*176 */ { 2, s_1_176, -1, 1, 0},
/*177 */ { 4, s_1_177, 176, 1, 0},
/*178 */ { 5, s_1_178, 177, 1, 0},
/*179 */ { 5, s_1_179, 176, 8, 0},
/*180 */ { 5, s_1_180, 176, 1, 0},
/*181 */ { 5, s_1_181, 176, 1, 0},
/*182 */ { 3, s_1_182, -1, 1, 0},
/*183 */ { 3, s_1_183, -1, 1, 0},
/*184 */ { 4, s_1_184, 183, 1, 0},
/*185 */ { 4, s_1_185, 183, 1, 0},
/*186 */ { 4, s_1_186, -1, 1, 0},
/*187 */ { 3, s_1_187, -1, 1, 0},
/*188 */ { 2, s_1_188, -1, 1, 0},
/*189 */ { 4, s_1_189, 188, 1, 0},
/*190 */ { 2, s_1_190, -1, 1, 0},
/*191 */ { 3, s_1_191, 190, 1, 0},
/*192 */ { 3, s_1_192, 190, 1, 0},
/*193 */ { 3, s_1_193, -1, 1, 0},
/*194 */ { 4, s_1_194, 193, 1, 0},
/*195 */ { 4, s_1_195, 193, 1, 0},
/*196 */ { 4, s_1_196, 193, 1, 0},
/*197 */ { 5, s_1_197, -1, 2, 0},
/*198 */ { 5, s_1_198, -1, 1, 0},
/*199 */ { 5, s_1_199, -1, 1, 0},
/*200 */ { 4, s_1_200, -1, 1, 0},
/*201 */ { 3, s_1_201, -1, 1, 0},
/*202 */ { 2, s_1_202, -1, 1, 0},
/*203 */ { 5, s_1_203, -1, 1, 0},
/*204 */ { 3, s_1_204, -1, 1, 0},
/*205 */ { 2, s_1_205, -1, 1, 0},
/*206 */ { 2, s_1_206, -1, 1, 0},
/*207 */ { 5, s_1_207, -1, 1, 0},
/*208 */ { 5, s_1_208, -1, 1, 0},
/*209 */ { 3, s_1_209, -1, 1, 0},
/*210 */ { 4, s_1_210, 209, 1, 0},
/*211 */ { 3, s_1_211, -1, 1, 0},
/*212 */ { 3, s_1_212, -1, 1, 0},
/*213 */ { 4, s_1_213, 212, 1, 0},
/*214 */ { 2, s_1_214, -1, 4, 0},
/*215 */ { 3, s_1_215, 214, 2, 0},
/*216 */ { 6, s_1_216, 215, 1, 0},
/*217 */ { 6, s_1_217, 215, 1, 0},
/*218 */ { 5, s_1_218, 215, 1, 0},
/*219 */ { 3, s_1_219, 214, 4, 0},
/*220 */ { 4, s_1_220, 214, 4, 0},
/*221 */ { 4, s_1_221, -1, 1, 0},
/*222 */ { 5, s_1_222, 221, 1, 0},
/*223 */ { 3, s_1_223, -1, 1, 0},
/*224 */ { 3, s_1_224, -1, 1, 0},
/*225 */ { 3, s_1_225, -1, 1, 0},
/*226 */ { 4, s_1_226, -1, 1, 0},
/*227 */ { 5, s_1_227, 226, 1, 0},
/*228 */ { 5, s_1_228, -1, 1, 0},
/*229 */ { 4, s_1_229, -1, 1, 0},
/*230 */ { 5, s_1_230, 229, 1, 0},
/*231 */ { 2, s_1_231, -1, 1, 0},
/*232 */ { 3, s_1_232, 231, 1, 0},
/*233 */ { 3, s_1_233, -1, 1, 0},
/*234 */ { 2, s_1_234, -1, 1, 0},
/*235 */ { 5, s_1_235, 234, 5, 0},
/*236 */ { 4, s_1_236, 234, 1, 0},
/*237 */ { 5, s_1_237, 236, 1, 0},
/*238 */ { 3, s_1_238, 234, 1, 0},
/*239 */ { 6, s_1_239, 234, 1, 0},
/*240 */ { 3, s_1_240, 234, 1, 0},
/*241 */ { 4, s_1_241, 234, 1, 0},
/*242 */ { 8, s_1_242, 241, 6, 0},
/*243 */ { 3, s_1_243, 234, 1, 0},
/*244 */ { 2, s_1_244, -1, 1, 0},
/*245 */ { 4, s_1_245, 244, 1, 0},
/*246 */ { 2, s_1_246, -1, 1, 0},
/*247 */ { 3, s_1_247, 246, 1, 0},
/*248 */ { 5, s_1_248, 247, 9, 0},
/*249 */ { 4, s_1_249, 247, 1, 0},
/*250 */ { 4, s_1_250, 247, 1, 0},
/*251 */ { 3, s_1_251, 246, 1, 0},
/*252 */ { 4, s_1_252, 246, 1, 0},
/*253 */ { 3, s_1_253, 246, 1, 0},
/*254 */ { 3, s_1_254, -1, 1, 0},
/*255 */ { 2, s_1_255, -1, 1, 0},
/*256 */ { 3, s_1_256, 255, 1, 0},
/*257 */ { 3, s_1_257, 255, 1, 0},
/*258 */ { 3, s_1_258, -1, 1, 0},
/*259 */ { 3, s_1_259, -1, 1, 0},
/*260 */ { 6, s_1_260, 259, 1, 0},
/*261 */ { 3, s_1_261, -1, 1, 0},
/*262 */ { 2, s_1_262, -1, 1, 0},
/*263 */ { 2, s_1_263, -1, 1, 0},
/*264 */ { 3, s_1_264, 263, 1, 0},
/*265 */ { 5, s_1_265, 263, 1, 0},
/*266 */ { 5, s_1_266, 263, 7, 0},
/*267 */ { 4, s_1_267, 263, 1, 0},
/*268 */ { 4, s_1_268, 263, 1, 0},
/*269 */ { 3, s_1_269, 263, 1, 0},
/*270 */ { 4, s_1_270, 263, 1, 0},
/*271 */ { 2, s_1_271, -1, 2, 0},
/*272 */ { 3, s_1_272, 271, 1, 0},
/*273 */ { 2, s_1_273, -1, 1, 0},
/*274 */ { 3, s_1_274, -1, 1, 0},
/*275 */ { 2, s_1_275, -1, 1, 0},
/*276 */ { 5, s_1_276, 275, 1, 0},
/*277 */ { 4, s_1_277, 275, 1, 0},
/*278 */ { 4, s_1_278, -1, 1, 0},
/*279 */ { 4, s_1_279, -1, 2, 0},
/*280 */ { 4, s_1_280, -1, 1, 0},
/*281 */ { 3, s_1_281, -1, 1, 0},
/*282 */ { 2, s_1_282, -1, 1, 0},
/*283 */ { 4, s_1_283, 282, 4, 0},
/*284 */ { 5, s_1_284, 282, 1, 0},
/*285 */ { 4, s_1_285, 282, 1, 0},
/*286 */ { 3, s_1_286, -1, 1, 0},
/*287 */ { 2, s_1_287, -1, 1, 0},
/*288 */ { 3, s_1_288, 287, 1, 0},
/*289 */ { 6, s_1_289, 288, 1, 0},
/*290 */ { 1, s_1_290, -1, 1, 0},
/*291 */ { 2, s_1_291, 290, 1, 0},
/*292 */ { 4, s_1_292, 290, 1, 0},
/*293 */ { 2, s_1_293, 290, 1, 0},
/*294 */ { 5, s_1_294, 293, 1, 0}
};

static const symbol s_2_0[4] = { 'z', 'l', 'e', 'a' };
static const symbol s_2_1[5] = { 'k', 'e', 'r', 'i', 'a' };
static const symbol s_2_2[2] = { 'l', 'a' };
static const symbol s_2_3[3] = { 'e', 'r', 'a' };
static const symbol s_2_4[4] = { 'd', 'a', 'd', 'e' };
static const symbol s_2_5[4] = { 't', 'a', 'd', 'e' };
static const symbol s_2_6[4] = { 'd', 'a', 't', 'e' };
static const symbol s_2_7[4] = { 't', 'a', 't', 'e' };
static const symbol s_2_8[2] = { 'g', 'i' };
static const symbol s_2_9[2] = { 'k', 'i' };
static const symbol s_2_10[2] = { 'i', 'k' };
static const symbol s_2_11[5] = { 'l', 'a', 'n', 'i', 'k' };
static const symbol s_2_12[3] = { 'r', 'i', 'k' };
static const symbol s_2_13[5] = { 'l', 'a', 'r', 'i', 'k' };
static const symbol s_2_14[4] = { 'z', 't', 'i', 'k' };
static const symbol s_2_15[2] = { 'g', 'o' };
static const symbol s_2_16[2] = { 'r', 'o' };
static const symbol s_2_17[3] = { 'e', 'r', 'o' };
static const symbol s_2_18[2] = { 't', 'o' };

static const struct among a_2[19] =
{
/*  0 */ { 4, s_2_0, -1, 2, 0},
/*  1 */ { 5, s_2_1, -1, 1, 0},
/*  2 */ { 2, s_2_2, -1, 1, 0},
/*  3 */ { 3, s_2_3, -1, 1, 0},
/*  4 */ { 4, s_2_4, -1, 1, 0},
/*  5 */ { 4, s_2_5, -1, 1, 0},
/*  6 */ { 4, s_2_6, -1, 1, 0},
/*  7 */ { 4, s_2_7, -1, 1, 0},
/*  8 */ { 2, s_2_8, -1, 1, 0},
/*  9 */ { 2, s_2_9, -1, 1, 0},
/* 10 */ { 2, s_2_10, -1, 1, 0},
/* 11 */ { 5, s_2_11, 10, 1, 0},
/* 12 */ { 3, s_2_12, 10, 1, 0},
/* 13 */ { 5, s_2_13, 12, 1, 0},
/* 14 */ { 4, s_2_14, 10, 1, 0},
/* 15 */ { 2, s_2_15, -1, 1, 0},
/* 16 */ { 2, s_2_16, -1, 1, 0},
/* 17 */ { 3, s_2_17, 16, 1, 0},
/* 18 */ { 2, s_2_18, -1, 1, 0}
};

static const unsigned char g_v[] = { 17, 65, 16 };

static const symbol s_0[] = { 'a', 't', 's', 'e', 'd', 'e', 'n' };
static const symbol s_1[] = { 'a', 'r', 'a', 'b', 'e', 'r', 'a' };
static const symbol s_2[] = { 'b', 'a', 'd', 'i', 't', 'u' };
static const symbol s_3[] = { 'j', 'o', 'k' };
static const symbol s_4[] = { 't', 'r', 'a' };
static const symbol s_5[] = { 'm', 'i', 'n', 'u', 't', 'u' };
static const symbol s_6[] = { 'z', 'e', 'h', 'a', 'r' };
static const symbol s_7[] = { 'g', 'e', 'l', 'd', 'i' };
static const symbol s_8[] = { 'i', 'g', 'a', 'r', 'o' };
static const symbol s_9[] = { 'a', 'u', 'r', 'k', 'a' };
static const symbol s_10[] = { 'z' };

static int r_mark_regions(struct SN_env * z) {
    z->I[0] = z->l;
    z->I[1] = z->l;
    z->I[2] = z->l;
    {   int c1 = z->c; /* do, line 27 */
        {   int c2 = z->c; /* or, line 29 */
            if (in_grouping(z, g_v, 97, 117, 0)) goto lab2;
            {   int c3 = z->c; /* or, line 28 */
                if (out_grouping(z, g_v, 97, 117, 0)) goto lab4;
                {    /* gopast */ /* grouping v, line 28 */
                    int ret = out_grouping(z, g_v, 97, 117, 1);
                    if (ret < 0) goto lab4;
                    z->c += ret;
                }
                goto lab3;
            lab4:
                z->c = c3;
                if (in_grouping(z, g_v, 97, 117, 0)) goto lab2;
                {    /* gopast */ /* non v, line 28 */
                    int ret = in_grouping(z, g_v, 97, 117, 1);
                    if (ret < 0) goto lab2;
                    z->c += ret;
                }
            }
        lab3:
            goto lab1;
        lab2:
            z->c = c2;
            if (out_grouping(z, g_v, 97, 117, 0)) goto lab0;
            {   int c4 = z->c; /* or, line 30 */
                if (out_grouping(z, g_v, 97, 117, 0)) goto lab6;
                {    /* gopast */ /* grouping v, line 30 */
                    int ret = out_grouping(z, g_v, 97, 117, 1);
                    if (ret < 0) goto lab6;
                    z->c += ret;
                }
                goto lab5;
            lab6:
                z->c = c4;
                if (in_grouping(z, g_v, 97, 117, 0)) goto lab0;
                if (z->c >= z->l) goto lab0;
                z->c++; /* next, line 30 */
            }
        lab5:
            ;
        }
    lab1:
        z->I[0] = z->c; /* setmark pV, line 31 */
    lab0:
        z->c = c1;
    }
    {   int c5 = z->c; /* do, line 33 */
        {    /* gopast */ /* grouping v, line 34 */
            int ret = out_grouping(z, g_v, 97, 117, 1);
            if (ret < 0) goto lab7;
            z->c += ret;
        }
        {    /* gopast */ /* non v, line 34 */
            int ret = in_grouping(z, g_v, 97, 117, 1);
            if (ret < 0) goto lab7;
            z->c += ret;
        }
        z->I[1] = z->c; /* setmark p1, line 34 */
        {    /* gopast */ /* grouping v, line 35 */
            int ret = out_grouping(z, g_v, 97, 117, 1);
            if (ret < 0) goto lab7;
            z->c += ret;
        }
        {    /* gopast */ /* non v, line 35 */
            int ret = in_grouping(z, g_v, 97, 117, 1);
            if (ret < 0) goto lab7;
            z->c += ret;
        }
        z->I[2] = z->c; /* setmark p2, line 35 */
    lab7:
        z->c = c5;
    }
    return 1;
}

static int r_RV(struct SN_env * z) {
    if (!(z->I[0] <= z->c)) return 0;
    return 1;
}

static int r_R2(struct SN_env * z) {
    if (!(z->I[2] <= z->c)) return 0;
    return 1;
}

static int r_R1(struct SN_env * z) {
    if (!(z->I[1] <= z->c)) return 0;
    return 1;
}

static int r_aditzak(struct SN_env * z) {
    int among_var;
    z->ket = z->c; /* [, line 46 */
    if (z->c - 1 <= z->lb || z->p[z->c - 1] >> 5 != 3 || !((70566434 >> (z->p[z->c - 1] & 0x1f)) & 1)) return 0;
    among_var = find_among_b(z, a_0, 109); /* substring, line 46 */
    if (!(among_var)) return 0;
    z->bra = z->c; /* ], line 46 */
    switch(among_var) {
        case 0: return 0;
        case 1:
            {   int ret = r_RV(z);
                if (ret == 0) return 0; /* call RV, line 57 */
                if (ret < 0) return ret;
            }
            {   int ret = slice_del(z); /* delete, line 57 */
                if (ret < 0) return ret;
            }
            break;
        case 2:
            {   int ret = r_R2(z);
                if (ret == 0) return 0; /* call R2, line 59 */
                if (ret < 0) return ret;
            }
            {   int ret = slice_del(z); /* delete, line 59 */
                if (ret < 0) return ret;
            }
            break;
        case 3:
            {   int ret = slice_from_s(z, 7, s_0); /* <-, line 61 */
                if (ret < 0) return ret;
            }
            break;
        case 4:
            {   int ret = slice_from_s(z, 7, s_1); /* <-, line 63 */
                if (ret < 0) return ret;
            }
            break;
        case 5:
            {   int ret = slice_from_s(z, 6, s_2); /* <-, line 65 */
                if (ret < 0) return ret;
            }
            break;
    }
    return 1;
}

static int r_izenak(struct SN_env * z) {
    int among_var;
    z->ket = z->c; /* [, line 71 */
    if (z->c <= z->lb || z->p[z->c - 1] >> 5 != 3 || !((71162402 >> (z->p[z->c - 1] & 0x1f)) & 1)) return 0;
    among_var = find_among_b(z, a_1, 295); /* substring, line 71 */
    if (!(among_var)) return 0;
    z->bra = z->c; /* ], line 71 */
    switch(among_var) {
        case 0: return 0;
        case 1:
            {   int ret = r_RV(z);
                if (ret == 0) return 0; /* call RV, line 101 */
                if (ret < 0) return ret;
            }
            {   int ret = slice_del(z); /* delete, line 101 */
                if (ret < 0) return ret;
            }
            break;
        case 2:
            {   int ret = r_R2(z);
                if (ret == 0) return 0; /* call R2, line 103 */
                if (ret < 0) return ret;
            }
            {   int ret = slice_del(z); /* delete, line 103 */
                if (ret < 0) return ret;
            }
            break;
        case 3:
            {   int ret = slice_from_s(z, 3, s_3); /* <-, line 105 */
                if (ret < 0) return ret;
            }
            break;
        case 4:
            {   int ret = r_R1(z);
                if (ret == 0) return 0; /* call R1, line 107 */
                if (ret < 0) return ret;
            }
            {   int ret = slice_del(z); /* delete, line 107 */
                if (ret < 0) return ret;
            }
            break;
        case 5:
            {   int ret = slice_from_s(z, 3, s_4); /* <-, line 109 */
                if (ret < 0) return ret;
            }
            break;
        case 6:
            {   int ret = slice_from_s(z, 6, s_5); /* <-, line 111 */
                if (ret < 0) return ret;
            }
            break;
        case 7:
            {   int ret = slice_from_s(z, 5, s_6); /* <-, line 113 */
                if (ret < 0) return ret;
            }
            break;
        case 8:
            {   int ret = slice_from_s(z, 5, s_7); /* <-, line 115 */
                if (ret < 0) return ret;
            }
            break;
        case 9:
            {   int ret = slice_from_s(z, 5, s_8); /* <-, line 117 */
                if (ret < 0) return ret;
            }
            break;
        case 10:
            {   int ret = slice_from_s(z, 5, s_9); /* <-, line 119 */
                if (ret < 0) return ret;
            }
            break;
    }
    return 1;
}

static int r_adjetiboak(struct SN_env * z) {
    int among_var;
    z->ket = z->c; /* [, line 124 */
    if (z->c - 1 <= z->lb || z->p[z->c - 1] >> 5 != 3 || !((35362 >> (z->p[z->c - 1] & 0x1f)) & 1)) return 0;
    among_var = find_among_b(z, a_2, 19); /* substring, line 124 */
    if (!(among_var)) return 0;
    z->bra = z->c; /* ], line 124 */
    switch(among_var) {
        case 0: return 0;
        case 1:
            {   int ret = r_RV(z);
                if (ret == 0) return 0; /* call RV, line 127 */
                if (ret < 0) return ret;
            }
            {   int ret = slice_del(z); /* delete, line 127 */
                if (ret < 0) return ret;
            }
            break;
        case 2:
            {   int ret = slice_from_s(z, 1, s_10); /* <-, line 129 */
                if (ret < 0) return ret;
            }
            break;
    }
    return 1;
}

extern int basque_ISO_8859_1_stem(struct SN_env * z) {
    {   int c1 = z->c; /* do, line 136 */
        {   int ret = r_mark_regions(z);
            if (ret == 0) goto lab0; /* call mark_regions, line 136 */
            if (ret < 0) return ret;
        }
    lab0:
        z->c = c1;
    }
    z->lb = z->c; z->c = z->l; /* backwards, line 137 */

    while(1) { /* repeat, line 138 */
        int m2 = z->l - z->c; (void)m2;
        {   int ret = r_aditzak(z);
            if (ret == 0) goto lab1; /* call aditzak, line 138 */
            if (ret < 0) return ret;
        }
        continue;
    lab1:
        z->c = z->l - m2;
        break;
    }
    while(1) { /* repeat, line 139 */
        int m3 = z->l - z->c; (void)m3;
        {   int ret = r_izenak(z);
            if (ret == 0) goto lab2; /* call izenak, line 139 */
            if (ret < 0) return ret;
        }
        continue;
    lab2:
        z->c = z->l - m3;
        break;
    }
    {   int m4 = z->l - z->c; (void)m4; /* do, line 140 */
        {   int ret = r_adjetiboak(z);
            if (ret == 0) goto lab3; /* call adjetiboak, line 140 */
            if (ret < 0) return ret;
        }
    lab3:
        z->c = z->l - m4;
    }
    z->c = z->lb;
    return 1;
}

extern struct SN_env * basque_ISO_8859_1_create_env(void) { return SN_create_env(0, 3, 0); }

extern void basque_ISO_8859_1_close_env(struct SN_env * z) { SN_close_env(z, 0); }

