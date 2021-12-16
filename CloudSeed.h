#pragma once

#include "Parameter.h"
#include "Polygons.h"
#include "Controller.h"

namespace CloudSeed
{
    const int PRESET_COUNT = 7;

    float BufferL[BUFFER_SIZE];
    float BufferR[BUFFER_SIZE];

    Controller controller(SAMPLERATE);
    Polygons::PolyOS os;
    uint16_t Presets[Parameter::COUNT * PRESET_COUNT];
    uint8_t currentPreset;
    bool stereo;

    void loadPreset(int number);
    void storePreset(int number);

    const char* ParameterNames[Parameter::COUNT];

    void setNames()
    {
        ParameterNames[Parameter::Mode] = "FX Mode";
        ParameterNames[Parameter::PreDelay] = "Pre-delay";
        ParameterNames[Parameter::LowCut] = "Low Cut";
        ParameterNames[Parameter::HighCut] = "High Cut";
        ParameterNames[Parameter::Dry] = "Dry Gain";
        ParameterNames[Parameter::Wet] = "Wet Gain";

        ParameterNames[Parameter::EarlySize] = "Early Size";
        ParameterNames[Parameter::EarlyMod] = "Mod Amount";
        ParameterNames[Parameter::EarlyRate] = "Mod Rate";
        ParameterNames[Parameter::EarlyGain] = "Early Gain";

        ParameterNames[Parameter::LateSize] = "Late Size";
        ParameterNames[Parameter::LateMod] = "Mod Amount";
        ParameterNames[Parameter::LateRate] = "Mod Rate";
        ParameterNames[Parameter::LateDecay] = "Decay";
        ParameterNames[Parameter::LateGain] = "Late Gain";

        ParameterNames[Parameter::SeedEarly] = "Early Seed";
        ParameterNames[Parameter::SeedDelay] = "Delay Seed";
        ParameterNames[Parameter::SeedDiffuse] = "Diffuser Seed";

        ParameterNames[Parameter::InputMode] = "Input";
        ParameterNames[Parameter::InputGain] = "Input Gain";
        ParameterNames[Parameter::OutputGain] = "Output Gain";
    }

    void RegisterParams()
    {
        if (os.controlBoard == ControlBoard::Alpha)
        {
            os.Register(Parameter::Mode,        60,   Polygons::ControlMode::Encoded, 0, 1);
            os.Register(Parameter::PreDelay,    1023, Polygons::ControlMode::Encoded, 1, 4);
            os.Register(Parameter::LowCut,      1023, Polygons::ControlMode::Encoded, 2, 4);
            os.Register(Parameter::HighCut,     1023, Polygons::ControlMode::Encoded, 3, 4);
            os.Register(Parameter::Dry,         1023, Polygons::ControlMode::Encoded, 4, 4);
            os.Register(Parameter::Wet,         1023, Polygons::ControlMode::Encoded, 6, 4);

            os.Register(Parameter::EarlySize,   1023, Polygons::ControlMode::Encoded, 8+0, 4);
            os.Register(Parameter::EarlyMod,    1023, Polygons::ControlMode::Encoded, 8+1, 4);
            os.Register(Parameter::EarlyRate,   1023, Polygons::ControlMode::Encoded, 8+2, 4);
            os.Register(Parameter::EarlyGain,   1023, Polygons::ControlMode::Encoded, 8+4, 4);

            os.Register(Parameter::LateSize,    1023, Polygons::ControlMode::Encoded, 16+0, 4);
            os.Register(Parameter::LateMod,     1023, Polygons::ControlMode::Encoded, 16+1, 4);
            os.Register(Parameter::LateRate,    1023, Polygons::ControlMode::Encoded, 16+2, 4);
            os.Register(Parameter::LateDecay,   1023, Polygons::ControlMode::Encoded, 16+3, 4);
            os.Register(Parameter::LateGain,    1023, Polygons::ControlMode::Encoded, 16+4, 4);

            os.Register(Parameter::SeedEarly,   1023, Polygons::ControlMode::Encoded, 24+0, 4);
            os.Register(Parameter::SeedDelay,   1023, Polygons::ControlMode::Encoded, 24+1, 4);
            os.Register(Parameter::SeedDiffuse, 1023, Polygons::ControlMode::Encoded, 24+2, 4);

            os.Register(Parameter::InputMode,   10,   Polygons::ControlMode::Encoded, 32+0, 1);
            os.Register(Parameter::InputGain,   1023, Polygons::ControlMode::Encoded, 32+4, 4);
            os.Register(Parameter::OutputGain,  1023, Polygons::ControlMode::Encoded, 32+6, 4);
        }
        else if (os.controlBoard == ControlBoard::Sigma)
        {
            os.Register(Parameter::Mode,        60,   Polygons::ControlMode::Encoded, 0, 1);
            os.Register(Parameter::HighCut,     1023, Polygons::ControlMode::Encoded, 1, 4);
            os.Register(Parameter::LateDecay,   1023, Polygons::ControlMode::Encoded, 2, 4);
            os.Register(Parameter::Wet,         1023, Polygons::ControlMode::Encoded, 3, 4);

            os.Register(Parameter::EarlySize,   1023, Polygons::ControlMode::Encoded, 4, 4);
            os.Register(Parameter::EarlyMod,    1023, Polygons::ControlMode::Encoded, 5, 4);
            os.Register(Parameter::EarlyRate,   1023, Polygons::ControlMode::Encoded, 6, 4);
            os.Register(Parameter::EarlyGain,   1023, Polygons::ControlMode::Encoded, 7, 4);
            

            os.Register(Parameter::InputGain,   1023, Polygons::ControlMode::Encoded, 8, 4);
            os.Register(Parameter::OutputGain,  1023, Polygons::ControlMode::Encoded, 9, 4);
            os.Register(Parameter::PreDelay,    1023, Polygons::ControlMode::Encoded, 10, 4);
            os.Register(Parameter::Dry,         1023, Polygons::ControlMode::Encoded, 11, 4);

            os.Register(Parameter::LateSize,    1023, Polygons::ControlMode::Encoded, 12, 4);
            os.Register(Parameter::LateMod,     1023, Polygons::ControlMode::Encoded, 13, 4);
            os.Register(Parameter::LateRate,    1023, Polygons::ControlMode::Encoded, 14, 4);
            os.Register(Parameter::LateGain,    1023, Polygons::ControlMode::Encoded, 15, 4);


            os.Register(Parameter::InputMode,   10,   Polygons::ControlMode::Encoded, 16, 1);
            os.Register(Parameter::LowCut,      1023, Polygons::ControlMode::Encoded, 17, 4);
            //
            //
            
            os.Register(Parameter::SeedEarly,   1023, Polygons::ControlMode::Encoded, 20, 4);
            os.Register(Parameter::SeedDelay,   1023, Polygons::ControlMode::Encoded, 21, 4);
            os.Register(Parameter::SeedDiffuse, 1023, Polygons::ControlMode::Encoded, 22, 4);
            //

            
        }


        os.Register(Parameter::Active,      1,    Polygons::ControlMode::DigitalToggle, 9, 0);
        os.Register(Parameter::Freeze,      1,    Polygons::ControlMode::Digital, 10, 0);
        
    }

    inline void getPageName(int page, char* dest)
    {
        if (page == 0)
            strcpy(dest, "Main");
        else if (page == 1)
            strcpy(dest, "Early");
        else if (page == 2)
            strcpy(dest, "Late");
        else if (page == 3)
            strcpy(dest, "Seeds");
        else if (page == 4)
            strcpy(dest, "Input");
        else if (page == 7)
            strcpy(dest, "SAVE");
        else
            strcpy(dest, "");
    }

    inline void getParameterName(int paramId, char* dest)
    {
        if (paramId >= 0)
            strcpy(dest, ParameterNames[paramId]);
        else
            strcpy(dest, "");
    }

    inline void getParameterDisplay(int paramId, char* dest)
    {
        double val = controller.GetScaledParameter(paramId);
        if (paramId == -1)
        {
            strcpy(dest, "");
        }
        else if (paramId == Parameter::Mode)
        {
            if (val == Mode::Fast)
                sprintf(dest, "Fast");
            else if (val == Mode::Plate)
                sprintf(dest, "Plate");
            else if (val == Mode::Sparse)
                sprintf(dest, "Sparse");
            else if (val == Mode::Swell)
                sprintf(dest, "Swell");
            else if (val == Mode::Plains)
                sprintf(dest, "Plains");
            else if (val == Mode::Wash)
                sprintf(dest, "Wash");
        }
        else if (paramId == Parameter::PreDelay)
        {
            sprintf(dest, "%dms", (int)val);
        }
        else if (paramId == Parameter::LowCut)
        {
            if (val < 1000)
                sprintf(dest, "%dhz", (int)val);
            else
                sprintf(dest, "%.1fKhz", val/1000);
        }
        else if (paramId == Parameter::HighCut)
        {
            if (val < 1000)
                sprintf(dest, "%dhz", (int)val);
            else
                sprintf(dest, "%.1fKhz", val/1000);
        }
        else if (paramId == Parameter::Dry || paramId == Parameter::Wet || paramId == Parameter::EarlyGain || paramId == Parameter::LateGain)
        {
            if (val <= -29.9)
                sprintf(dest, "Mute");
            else
                sprintf(dest, "%.1fdB", val);
        }
        else if (paramId == Parameter::EarlySize || paramId == Parameter::LateSize || paramId == Parameter::EarlyMod || paramId == Parameter::LateMod)
        {
            sprintf(dest, "%d%%", (int)(val*100));
        }
        else if (paramId == Parameter::EarlyRate || paramId == Parameter::LateRate)
        {
            sprintf(dest, "%.2fHz", val);
        }
        else if (paramId == Parameter::LateDecay)
        {
            sprintf(dest, "%.2fs", val);
        }
        else if (paramId == Parameter::SeedEarly || paramId == Parameter::SeedDelay || paramId == Parameter::SeedDiffuse)
        {
            sprintf(dest, "%d", (int)val);
        }
        else if (paramId == Parameter::InputMode)
        {
            sprintf(dest, val < 0.5 ? "Mono" : "Stereo");
        }
        else if (paramId == Parameter::InputGain || paramId == Parameter::OutputGain)
        {
            sprintf(dest, "%ddB", (int)val);
        }
        else
        {
            sprintf(dest, "%.0f", val*100);
        }
    }

    inline void setPresetLed()
    {
        // changes the different RGB colours of the LED to indicate preset number
        Polygons::pushDigital(0, (currentPreset + 1) & 0b001);
        Polygons::pushDigital(1, (currentPreset + 1) & 0b010);
        Polygons::pushDigital(2, (currentPreset + 1) & 0b100);
    }

    inline void setActiveFreezeLeds()
    {
        bool active = controller.GetScaledParameter(Parameter::Active) == 1;
        bool freeze = controller.GetScaledParameter(Parameter::Freeze) == 1;
        // Uses the Red LED to indicate active and freeze states
        Polygons::pushDigital(3, active);
        Polygons::pushDigital(6, freeze);
    }

    inline void setIOConfig()
    {
        stereo = controller.GetScaledParameter(Parameter::InputMode) > 0.5;
        int gainIn = (int8_t)controller.GetScaledParameter(Parameter::InputGain) * 2;
        int gainOut = (int8_t)controller.GetScaledParameter(Parameter::OutputGain);
        Polygons::codec.analogInGain(gainIn, gainIn);
        Serial.print("Setting output gain ");
        Serial.println(gainOut);
        Polygons::codec.lineOutGain(gainOut, gainOut, false);
        Polygons::codec.headphoneGain(gainOut, gainOut, false);
    }

    inline bool handleUpdate(Polygons::ParameterUpdate* update)
    {
        if (update->Type == Polygons::MessageType::Digital && update->Index == 7)
        {
            storePreset(currentPreset);
            return true;
        }
        else if (update->Type == Polygons::MessageType::Digital && update->Index == 8 && update->Value > 0)
        {
            loadPreset((currentPreset + 1) % PRESET_COUNT);
            return true;
        }

       return false;
    }

    inline void setParameter(uint8_t paramId, uint16_t value)
    {
        controller.SetParameter(paramId, (uint16_t)value);
        setActiveFreezeLeds();
        if (paramId >= Parameter::InputMode && paramId <= Parameter::OutputGain)
            setIOConfig();
    }

    inline void audioCallback(int32_t** inputs, int32_t** outputs)
    {
        if (!stereo)
            inputs[1] = inputs[0];

        float scaler = (float)(1.0 / (double)SAMPLE_32_MAX);
        for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
        {
            BufferL[i] = inputs[0][i] * scaler;
            BufferR[i] = inputs[1][i] * scaler;
        }

        float* ins[2] = {BufferL, BufferR};
        controller.Process(ins, BUFFER_SIZE);
        float** outs = controller.GetOutput();

        if (controller.GetScaledParameter(Parameter::Active) == 1)
        {
            for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
            {
                outputs[0][i] = (int)(outs[0][i] * SAMPLE_32_MAX);
                outputs[1][i] = (int)(outs[1][i] * SAMPLE_32_MAX);
            }
        }
        else
        {
            for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
            {
                outputs[0][i] = inputs[0][i];
                outputs[1][i] = inputs[1][i];
            }
        }
    }

    void loadPreset(int number)
    {
        currentPreset = number;
        auto preset = &Presets[number * Parameter::COUNT];
        for (size_t i = 0; i < Parameter::COUNT; i++)
        {
            if (i != Parameter::Active && i != Parameter::Freeze)
                controller.SetParameter(i, preset[i]);
        }
        setPresetLed();
        setIOConfig();
    }

    void storePreset(int number)
    {
        auto preset = &Presets[number * Parameter::COUNT];
        auto rawParams = controller.GetAllParameters();
        for (size_t i = 0; i < Parameter::COUNT; i++)
        {
            preset[i] = rawParams[i];
        }
        bool ok = Storage::WriteFile("Cloudseed/presets.bin", (uint8_t*)Presets, sizeof(Presets));
        if (ok)
            os.menu.setMessage("Preset Stored", 1000);
        else
            os.menu.setMessage("Error writing preset!", 1000);
    }

    inline void LoadPresetsSD()
    {
        if (Storage::FileExists("Cloudseed/presets.bin"))
        {
            Serial.println("Reading presets from SD Card...");
            Storage::ReadFile("Cloudseed/presets.bin", (uint8_t*)Presets, sizeof(Presets));
            Serial.println("Done reading presets");
        }
        else
        {
            Serial.println("No presets are present, writing to SD Card...");
            Storage::CreateFolder("Cloudseed");
            Storage::WriteFile("Cloudseed/presets.bin", (uint8_t*)Presets, sizeof(Presets));
            Serial.println("Done writing presets");
        }
    }

    inline void start()
    {
        Serial.println("Starting up - waiting for controller signal...");
        os.waitForControllerSignal();
        setNames();
        RegisterParams();

        for (size_t i = 0; i < Parameter::COUNT * PRESET_COUNT; i++)
            Presets[i] = 0;
        
        os.HandleUpdateCallback = handleUpdate;
        os.SetParameterCallback = setParameter;
        os.PageCount = os.controlBoard == ControlBoard::Alpha ? 5 : 3;
        os.menu.getPageName = getPageName;
        os.menu.getParameterName = getParameterName;
        os.menu.getParameterDisplay = getParameterDisplay;
        i2sAudioCallback = audioCallback;
        
        LoadPresetsSD();
        loadPreset(0);
        controller.SetParameter(Parameter::Active, 1);
        setActiveFreezeLeds();

        Serial.println("Enable pin 6");
        pinMode(6, OUTPUT);
        digitalWrite(6, LOW);
    }

    inline void loop()
    {
        os.loop();
    }
}
