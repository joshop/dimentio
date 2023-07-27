#include "helper.h"
s32 SIN_LUT[256] = {
    0,      1608,   3216,   4821,   6424,   8022,   9616,   11204,  12785,
    14359,  15924,  17479,  19024,  20557,  22078,  23586,  25080,  26558,
    28020,  29466,  30893,  32303,  33692,  35062,  36410,  37736,  39040,
    40320,  41576,  42806,  44011,  45190,  46341,  47464,  48559,  49624,
    50660,  51665,  52639,  53581,  54491,  55368,  56212,  57022,  57798,
    58538,  59244,  59914,  60547,  61145,  61705,  62228,  62714,  63162,
    63572,  63944,  64277,  64571,  64827,  65043,  65220,  65358,  65457,
    65516,  65536,  65516,  65457,  65358,  65220,  65043,  64827,  64571,
    64277,  63944,  63572,  63162,  62714,  62228,  61705,  61145,  60547,
    59914,  59244,  58538,  57798,  57022,  56212,  55368,  54491,  53581,
    52639,  51665,  50660,  49624,  48559,  47464,  46341,  45190,  44011,
    42806,  41576,  40320,  39040,  37736,  36410,  35062,  33692,  32303,
    30893,  29466,  28020,  26558,  25080,  23586,  22078,  20557,  19024,
    17479,  15924,  14359,  12785,  11204,  9616,   8022,   6424,   4821,
    3216,   1608,   0,      -1608,  -3216,  -4821,  -6424,  -8022,  -9616,
    -11204, -12785, -14359, -15924, -17479, -19024, -20557, -22078, -23586,
    -25080, -26558, -28020, -29466, -30893, -32303, -33692, -35062, -36410,
    -37736, -39040, -40320, -41576, -42806, -44011, -45190, -46341, -47464,
    -48559, -49624, -50660, -51665, -52639, -53581, -54491, -55368, -56212,
    -57022, -57798, -58538, -59244, -59914, -60547, -61145, -61705, -62228,
    -62714, -63162, -63572, -63944, -64277, -64571, -64827, -65043, -65220,
    -65358, -65457, -65516, -65536, -65516, -65457, -65358, -65220, -65043,
    -64827, -64571, -64277, -63944, -63572, -63162, -62714, -62228, -61705,
    -61145, -60547, -59914, -59244, -58538, -57798, -57022, -56212, -55368,
    -54491, -53581, -52639, -51665, -50660, -49624, -48559, -47464, -46341,
    -45190, -44011, -42806, -41576, -40320, -39040, -37736, -36410, -35062,
    -33692, -32303, -30893, -29466, -28020, -26558, -25080, -23586, -22078,
    -20557, -19024, -17479, -15924, -14359, -12785, -11204, -9616,  -8022,
    -6424,  -4821,  -3216,  -1608};
s32 COS_LUT[256] = {
    65536,  65516,  65457,  65358,  65220,  65043,  64827,  64571,  64277,
    63944,  63572,  63162,  62714,  62228,  61705,  61145,  60547,  59914,
    59244,  58538,  57798,  57022,  56212,  55368,  54491,  53581,  52639,
    51665,  50660,  49624,  48559,  47464,  46341,  45190,  44011,  42806,
    41576,  40320,  39040,  37736,  36410,  35062,  33692,  32303,  30893,
    29466,  28020,  26558,  25080,  23586,  22078,  20557,  19024,  17479,
    15924,  14359,  12785,  11204,  9616,   8022,   6424,   4821,   3216,
    1608,   0,      -1608,  -3216,  -4821,  -6424,  -8022,  -9616,  -11204,
    -12785, -14359, -15924, -17479, -19024, -20557, -22078, -23586, -25080,
    -26558, -28020, -29466, -30893, -32303, -33692, -35062, -36410, -37736,
    -39040, -40320, -41576, -42806, -44011, -45190, -46341, -47464, -48559,
    -49624, -50660, -51665, -52639, -53581, -54491, -55368, -56212, -57022,
    -57798, -58538, -59244, -59914, -60547, -61145, -61705, -62228, -62714,
    -63162, -63572, -63944, -64277, -64571, -64827, -65043, -65220, -65358,
    -65457, -65516, -65536, -65516, -65457, -65358, -65220, -65043, -64827,
    -64571, -64277, -63944, -63572, -63162, -62714, -62228, -61705, -61145,
    -60547, -59914, -59244, -58538, -57798, -57022, -56212, -55368, -54491,
    -53581, -52639, -51665, -50660, -49624, -48559, -47464, -46341, -45190,
    -44011, -42806, -41576, -40320, -39040, -37736, -36410, -35062, -33692,
    -32303, -30893, -29466, -28020, -26558, -25080, -23586, -22078, -20557,
    -19024, -17479, -15924, -14359, -12785, -11204, -9616,  -8022,  -6424,
    -4821,  -3216,  -1608,  0,      1608,   3216,   4821,   6424,   8022,
    9616,   11204,  12785,  14359,  15924,  17479,  19024,  20557,  22078,
    23586,  25080,  26558,  28020,  29466,  30893,  32303,  33692,  35062,
    36410,  37736,  39040,  40320,  41576,  42806,  44011,  45190,  46341,
    47464,  48559,  49624,  50660,  51665,  52639,  53581,  54491,  55368,
    56212,  57022,  57798,  58538,  59244,  59914,  60547,  61145,  61705,
    62228,  62714,  63162,  63572,  63944,  64277,  64571,  64827,  65043,
    65220,  65358,  65457,  65516};