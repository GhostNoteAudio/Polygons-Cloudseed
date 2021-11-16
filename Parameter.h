#pragma once

namespace Revo
{
    class Parameter
    {
    public:
        static const int Mode = 0;
        static const int PreDelay = 1;
        static const int LowCut = 2;
        static const int HighCut = 3;
        static const int Dry = 4;
        static const int Wet = 5;        

        static const int EarlySize = 6;
        static const int EarlyMod = 7;
        static const int EarlyRate = 8;
        static const int EarlyGain = 9;

        static const int LateSize = 10;
        static const int LateMod = 11;
        static const int LateRate = 12;
        static const int LateDecay = 13;
        static const int LateGain = 14;

        static const int SeedEarly = 15;
        static const int SeedDelay = 16;
        static const int SeedDiffuse = 17;

        static const int InputMode = 18;
        static const int InputGain = 19;
        static const int OutputGain = 20;

        static const int COUNT = 21;
    };
}
