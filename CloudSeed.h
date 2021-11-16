#pragma once

#include "Parameter.h"
#include "Polygons.h"
#include "menuManager.h"
#include "Controller.h"

namespace CloudSeed
{
    const int PRESET_COUNT = 7;

    float BufferL[BUFFER_SIZE];
    float BufferR[BUFFER_SIZE];

    Polygons::MenuManager menu;
    Controller controller(SAMPLERATE);
    uint16_t Presets[Parameter::COUNT * PRESET_COUNT];
    uint8_t currentPreset;
    bool active;
    bool freeze;
    bool stereo;

    void loadPreset(int number);
    void storePreset(int number);

    const char* ParameterShortNames[Parameter::COUNT]
    {
        "Mode",
        "PreD",
        "LoCut",
        "HiCut",
        "Dry",
        "Wet",
        
        "Size",
        "Mod",
        "Rate",
        "Gain",

        "Size",
        "Mod",
        "Rate",
        "Decay",
        "Gain",

        "Early",
        "Delay",
        "Diff",

        "Input",
        "Input Gain",
        "Output Gain",
    };

    const char* ParameterLongNames[Parameter::COUNT]
    {
        "Reverb Mode",
        "Pre-delay",
        "Input Low Cut",
        "Input High Cut",
        "Dry Gain",
        "Wet Gain",
        
        "Early Size",
        "Early Mod Amount",
        "Early Mod Rate",
        "Early Gain",

        "Late Size",
        "Late Mod Amount",
        "Late Mod Rate",
        "Decay",
        "Late Gain",

        "Early Seed",
        "Delay Line Seed",
        "Diffuser Seed",

        "Input",
        "Input Gain",
        "Output Gain",
    };

    int getParam(int page, int index)
    {
        if (page == 0)
        {
            if (index == 0)
                return Parameter::Mode;
            if (index == 1)
                return Parameter::PreDelay;
            if (index == 2)
                return Parameter::LowCut;
            if (index == 3)
                return Parameter::HighCut;

            if (index == 4)
                return Parameter::Dry;
            if (index == 6)
                return Parameter::Wet;
        }

        if (page == 1)
        {
            if (index == 0)
                return Parameter::EarlySize;
            if (index == 1)
                return Parameter::EarlyMod;
            if (index == 2)
                return Parameter::EarlyRate;
            if (index == 4)
                return Parameter::EarlyGain;
        }

        if (page == 2)
        {
            if (index == 0)
                return Parameter::LateSize;
            if (index == 1)
                return Parameter::LateMod;
            if (index == 2)
                return Parameter::LateRate;
            if (index == 3)
                return Parameter::LateDecay;
            if (index == 4)
                return Parameter::LateGain;
        }

        if (page == 3)
        {
            if (index == 0)
                return Parameter::SeedEarly;
            if (index == 1)
                return Parameter::SeedDelay;
            if (index == 2)
                return Parameter::SeedDiffuse;
        }

        if (page == 4)
        {
            if (index == 0)
                return Parameter::InputMode;
            if (index == 4)
                return Parameter::InputGain;
            if (index == 6)
                return Parameter::OutputGain;
        }

        return -1;
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

    inline void getParameterShortName(int page, int index, char* dest)
    {
        auto para = (int)getParam(page, index);
        if (para >= 0)
            strcpy(dest, ParameterShortNames[para]);
        else
            strcpy(dest, "");
    }

    inline void getParameterLongName(int page, int index, char* dest)
    {
        auto para = (int)getParam(page, index);
        if (para >= 0)
            strcpy(dest, ParameterLongNames[para]);
        else
            strcpy(dest, "");
    }

    inline void getParameterDisplay(int page, int index, char* dest)
    {
        int param = getParam(page, index);
        double val = controller.GetScaledParameter(param);
        if (param == -1)
        {
            strcpy(dest, "");
        }
        else if (param == Parameter::Mode)
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
        else if (param == Parameter::PreDelay)
        {
            sprintf(dest, "%dms", (int)val);
        }
        else if (param == Parameter::LowCut)
        {
            if (val < 1000)
                sprintf(dest, "%dhz", (int)val);
            else
                sprintf(dest, "%.1fKhz", val/1000);
        }
        else if (param == Parameter::HighCut)
        {
            if (val < 1000)
                sprintf(dest, "%dhz", (int)val);
            else
                sprintf(dest, "%.1fKhz", val/1000);
        }
        else if (param == Parameter::Dry || param == Parameter::Wet || param == Parameter::EarlyGain || param == Parameter::LateGain)
        {
            if (val <= -29.9)
                sprintf(dest, "Mute");
            else
                sprintf(dest, "%.1fdB", val);
        }
        else if (param == Parameter::EarlySize || param == Parameter::LateSize || param == Parameter::EarlyMod || param == Parameter::LateMod)
        {
            sprintf(dest, "%d%%", (int)(val*100));
        }
        else if (param == Parameter::EarlyRate || param == Parameter::LateRate)
        {
            sprintf(dest, "%.2fHz", val);
        }
        else if (param == Parameter::LateDecay)
        {
            sprintf(dest, "%.2fs", val);
        }
        else if (param == Parameter::SeedEarly || param == Parameter::SeedDelay || param == Parameter::SeedDiffuse)
        {
            sprintf(dest, "%d", (int)val);
        }
        else if (param == Parameter::InputMode)
        {
            sprintf(dest, val < 0.5 ? "Mono" : "Stereo");
        }
        else if (param == Parameter::InputGain || param == Parameter::OutputGain)
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
        Polygons::controls.pushDigital(0, (currentPreset + 1) & 0b001);
        Polygons::controls.pushDigital(1, (currentPreset + 1) & 0b010);
        Polygons::controls.pushDigital(2, (currentPreset + 1) & 0b100);
    }

    inline void setActiveFreezeLeds()
    {
        // Uses the Red LED to indicate active and freeze states
        Polygons::controls.pushDigital(3, active);
        Polygons::controls.pushDigital(6, freeze);
    }

    inline void setIOConfig()
    {
        stereo = controller.GetScaledParameter(Parameter::InputMode) > 0.5;
        int gainIn = (uint8_t)round(controller.GetScaledParameter(Parameter::InputGain) * 2);
        int gainOut = (uint8_t)round(controller.GetScaledParameter(Parameter::OutputGain) * 2);
        Polygons::cctrl.analogInGain(gainIn, gainIn);
        Polygons::cctrl.lineOutGain(gainOut, gainOut, false);
        Polygons::cctrl.headphoneGain(gainOut, gainOut, false);
    }

    inline void handleUpdate(Polygons::ControlType type, int index)
    {
        if (type == Polygons::ControlType::Digital && index == 7)
        {
            storePreset(currentPreset);
        }
        else if (type == Polygons::ControlType::Digital && index < menu.pageCount)
        {
            menu.SelectedPage = index;
            menu.setUpdated();
        }
        else if (type == Polygons::ControlType::Digital && index == 8)
        {
            if (Polygons::controls.Digital[index] > 0)
            {
                loadPreset((currentPreset + 1) % PRESET_COUNT);
            }
        }
        else if (type == Polygons::ControlType::Digital && index == 9)
        {
            if (Polygons::controls.Digital[index] > 0)
            {
                active = !active;
                setActiveFreezeLeds();
            }
        }
        else if (type == Polygons::ControlType::Digital && index == 10)
        {
            freeze = Polygons::controls.Digital[index] > 0;
            controller.Freeze(freeze);
            setActiveFreezeLeds();
        }
        else if (type == Polygons::ControlType::Encoder)
        {
            int delta = (int)Polygons::controls.EncoderDelta[index];
            Polygons::controls.EncoderDelta[index] = 0;
            int paramId = getParam(menu.SelectedPage, index);
            if (paramId >= 0 && paramId < Parameter::COUNT)
            {
                int newVal = controller.GetAllParameters()[paramId] + delta * 256;
                newVal = Clip(newVal, 0, UINT16_MAX);
                controller.SetParameter(paramId, (uint16_t)newVal);
                menu.setUpdated(index);
            }

            if (paramId >= Parameter::InputMode)
                setIOConfig();
        }
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

        if (active)
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
        menu.setMessage("Preset Stored", 1000);
    }

    inline void start()
    {
        for (size_t i = 0; i < Parameter::COUNT * PRESET_COUNT; i++)
        {
            Presets[i] = 0;
        }
        
        active = true;
        freeze = false;
        stereo = true;
        Polygons::controls.onUpdate = handleUpdate;
        menu.pageCount = 8;
        menu.getPageName = getPageName;
        menu.getParameterShortName = getParameterShortName;
        menu.getParameterLongName = getParameterLongName;
        menu.getParameterDisplay = getParameterDisplay;
        i2sAudioCallback = audioCallback;
        
        loadPreset(0);
        setActiveFreezeLeds();
    }

    inline void loop()
    {
        Polygons::controls.readUpdates();
        menu.Draw();
        Polygons::pushDisplay();
        setPresetLed();
        setActiveFreezeLeds();
    }
}
