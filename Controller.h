
#pragma once

#include "Constants.h"
#include "Parameter.h"
#include "ReverbChannel.h"
#include "Utils.h"

namespace CloudSeed
{
	class Controller
	{
	private:
		int samplerate;

		ReverbChannel channelL;
		ReverbChannel channelR;
		uint16_t parameters[Parameter::COUNT];
		float* outputs[2];

	public:
		Controller(int samplerate)
			: channelL(samplerate, true)
			, channelR(samplerate, false)
		{
			this->samplerate = samplerate;
		}

		int GetSamplerate()
		{
			return samplerate;
		}

		uint16_t* GetAllParameters()
		{
			return parameters;
		}

		void Freeze(bool enable)
		{
			channelL.freeze = enable;
			channelR.freeze = enable;
		}

		double GetScaledParameter(int param)
		{
			switch (param)
			{
                case Parameter::Mode:                     return (int)(P(Parameter::Mode) * 5.999);
                case Parameter::PreDelay:                 return (int)(P(Parameter::PreDelay) * 240);

                case Parameter::LowCut:                   return 20 + Response4Oct(P(Parameter::LowCut)) * 980;
                case Parameter::HighCut:                  return 400 + Response4Oct(P(Parameter::HighCut)) * 19600;

                case Parameter::Dry:                      return 30 * P(Parameter::Dry) - 30;
				case Parameter::Wet:                      return 30 * P(Parameter::Wet) - 30;
                
				case Parameter::EarlySize:                return 0.9 * P(Parameter::EarlySize) + 0.1; // 0.1 to 1
                case Parameter::EarlyMod:                 return P(Parameter::EarlyMod);
                case Parameter::EarlyRate:                return Response4Oct(P(Parameter::EarlyRate)) * 5;
				case Parameter::EarlyGain:                return 30 * P(Parameter::EarlyGain) - 30;

				case Parameter::LateSize:                 return 0.5 * P(Parameter::LateSize) + 0.5; // 0.5 to 1
                case Parameter::LateMod:                  return P(Parameter::LateMod);
                case Parameter::LateRate:                 return Response4Oct(P(Parameter::LateRate)) * 5;
				case Parameter::LateDecay:                return Response2Dec(P(Parameter::LateDecay)) * 30;
				case Parameter::LateGain:                 return 30 * P(Parameter::LateGain) - 30;

				case Parameter::SeedEarly:                return (int)(P(Parameter::SeedEarly) * 255.99);
				case Parameter::SeedDelay:                return (int)(P(Parameter::SeedDelay) * 255.99);
				case Parameter::SeedDiffuse:              return (int)(P(Parameter::SeedDiffuse) * 255.99);

				case Parameter::InputMode:                return (int)(P(Parameter::InputMode) > 0.5);
				case Parameter::InputGain:                return P(Parameter::InputGain) * 12;
				case Parameter::OutputGain:               return -6 + P(Parameter::OutputGain) * 18;

                default: return 0.0;
			}
		}

		void SetParameter(int param, uint16_t value)
		{
			parameters[param] = value;
			auto scaled = GetScaledParameter(param);
			
			channelL.SetParameter(param, scaled);
			channelR.SetParameter(param, scaled);
		}

		void ClearBuffers()
		{
			channelL.ClearBuffers();
			channelR.ClearBuffers();
		}

		void Process(float** input, int bufferSize)
		{
			auto len = bufferSize;
			channelL.Process(input[0], len);
			channelR.Process(input[1], len);
		}

		float** GetOutput()
		{
			outputs[0] = channelL.GetOutput();
			outputs[1] = channelR.GetOutput();
			return outputs;
		}
		
	private:
		double P(int para)
		{
			auto idx = (int)para;
			return idx >= 0 && idx < Parameter::COUNT ? (parameters[idx] / (double)UINT16_MAX) : 0.0;
		}
	};
}
