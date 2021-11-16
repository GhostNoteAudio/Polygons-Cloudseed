#pragma once

#include "Constants.h"
#include "Parameter.h"
#include "Utils.h"
#include "ModulatedDelay.h"
#include "ModulatedAllpass.h"
#include "Lp1.h"
#include "Hp1.h"
#include "Biquad.h"
#include "LcgRandom.h"

namespace CloudSeed
{
	const int EARLY_DIFFUSER_COUNT = 8;
	const int DELAY_LINE_COUNT = 6;
	const int LATE_DIFFUSER_COUNT = 2;

	DMAMEM ModulatedDelay<FS_MAX/4> preDelayL;
	DMAMEM ModulatedDelay<FS_MAX/4> preDelayR;
	DMAMEM ModulatedAllpass earlyDiffusersL[EARLY_DIFFUSER_COUNT];
	ModulatedAllpass earlyDiffusersR[EARLY_DIFFUSER_COUNT];
	DMAMEM ModulatedDelay<FS_MAX/4> delayLinesL[DELAY_LINE_COUNT];
	DMAMEM ModulatedDelay<FS_MAX/4> delayLinesR[DELAY_LINE_COUNT];
	ModulatedAllpass lateDiffusersL[DELAY_LINE_COUNT * LATE_DIFFUSER_COUNT];
	ModulatedAllpass lateDiffusersR[DELAY_LINE_COUNT * LATE_DIFFUSER_COUNT];
	LcgRandom random;

	class Mode
	{
	public:
		static const int Fast = 0; // Quick attack, short decay, fairly sparse
		static const int Plate = 1; // Thick with fairly fast attack,
		static const int Sparse = 2; // Long and sparse
		static const int Swell = 3; // nonlinear attack, swell with lots of allpass delay
		static const int Plains = 4; // Giant slurry of sound
		static const int Wash = 5; // white noise wash
	};

	class ReverbChannel
	{
	private:
		int samplerate;
		float outBuffer[BUFFER_SIZE];
		float tempBuffer[BUFFER_SIZE];
		float tempBuffer2[BUFFER_SIZE];
		bool leftChannel;

		int earlyDiffuseStages;
		int lateDiffuseStages;
		int delayLineCount;
		bool useInterpolation;
		int mode;
		float dryGain;
		float wetGain;
		float earlyGain;
		float lateGain;
		float earlyDiffFeedback;
		float earlyDiffScaler;
		float earlySize;
		float earlyMod;
		float earlyRate;
		float lateDiffFeedback;
		float lateDiffScaler;
		float delayLineScaler;
		float lateSize;
		float lateMod;
		float lateRate;
		float lateDecay;
		float smoothedFreeze;
		float feedbackHiFc;
		float feedbackLoFc;
		float feedbackDampDb;

		int seedEarly;
		int seedDelay;
		int seedDiffuse;

		Biquad lowCut;
		Biquad highCut;
		float feedback[DELAY_LINE_COUNT];
		Biquad feedbackHi[DELAY_LINE_COUNT];
		Biquad feedbackLo[DELAY_LINE_COUNT];
		ModulatedDelay<FS_MAX/4>* preDelay;
		ModulatedAllpass* earlyDiffusers;
		ModulatedDelay<FS_MAX/4>* delayLines;
		ModulatedAllpass* lateDiffusers;

	public:
		bool freeze;

		ReverbChannel(int samplerate, bool leftChannel) :
			lowCut(Biquad::FilterType::HighPass, samplerate),
			highCut(Biquad::FilterType::LowPass, samplerate),
			feedbackHi { Biquad(Biquad::FilterType::HighShelf, samplerate), Biquad(Biquad::FilterType::HighShelf, samplerate), Biquad(Biquad::FilterType::HighShelf, samplerate), 
						 Biquad(Biquad::FilterType::HighShelf, samplerate), Biquad(Biquad::FilterType::HighShelf, samplerate), Biquad(Biquad::FilterType::HighShelf, samplerate)},
			feedbackLo { Biquad(Biquad::FilterType::LowShelf, samplerate), Biquad(Biquad::FilterType::LowShelf, samplerate), Biquad(Biquad::FilterType::LowShelf, samplerate), 
			             Biquad(Biquad::FilterType::LowShelf, samplerate), Biquad(Biquad::FilterType::LowShelf, samplerate), Biquad(Biquad::FilterType::LowShelf, samplerate)}
		{
			freeze = false;
			this->leftChannel = leftChannel;
			seedEarly = 0;
			seedDelay = 5;
			seedDiffuse = 10;
			smoothedFreeze = 0;

			if (leftChannel)
			{
				preDelay = &preDelayL;
				earlyDiffusers = &(earlyDiffusersL[0]);
				delayLines = &(delayLinesL[0]);
				lateDiffusers = &(lateDiffusersL[0]);
			}
			else
			{
				preDelay = &preDelayR;
				earlyDiffusers = &(earlyDiffusersR[0]);
				delayLines = &(delayLinesR[0]);
				lateDiffusers = &(lateDiffusersR[0]);
			}

			useInterpolation = true;
			earlyDiffuseStages = 4;
			delayLineCount = DELAY_LINE_COUNT;
			lateDiffuseStages = 1;
			mode = Mode::Fast;
			earlyDiffScaler = 50;
			lateDiffScaler = 50;
			delayLineScaler = 240;
			dryGain = 1;
			wetGain = 1;
			earlyGain = 1;
			lateGain = 0;
			earlySize = 1;
			earlyRate = 0;
			lateSize = 1;
			lateRate = 0;
			lateDecay = 1;

			this->samplerate = samplerate;
			preDelay->SampleDelay = 0;

			lowCut.SetQ(0.7);
			highCut.SetQ(0.7);
			lowCut.Frequency = 20;
			highCut.Frequency = 20000;
			lowCut.Update();
			highCut.Update();

			feedbackHiFc = 20000;
			feedbackLoFc = 20;

			for (size_t i = 0; i < DELAY_LINE_COUNT; i++)
			{
				feedback[i] = 0.0f;
				feedbackHi[i].SetGainDb(-2);
				feedbackLo[i].SetGainDb(-2);
				feedbackHi[i].Frequency = feedbackHiFc;
				feedbackLo[i].Frequency = feedbackLoFc;
				feedbackHi[i].Update();
				feedbackLo[i].Update();
				feedbackHi[i].ClearBuffers();
				feedbackLo[i].ClearBuffers();
				delayLines[i].SampleDelay = 10000;
				delayLines[i].ModAmount = 0;
				delayLines[i].ModRate = 0;
			}

			UpdateFeedbackFilters();
			UpdateEarlyDiffusers(true);
			UpdateLateDiffusers(true);
			UpdateDelayLines(true);
			UpdateInterpolation();
			UpdateMode();
		}

		float* GetOutput()
		{
			return outBuffer;
		}

		void SetParameter(int para, double value)
		{
			switch (para)
			{
			case Parameter::Mode:
				mode = value;
				UpdateMode();
				break;
			case Parameter::PreDelay:
				preDelay->SampleDelay = Ms2Samples(value);
				break;
			case Parameter::LowCut:
				lowCut.Frequency = value;
				lowCut.Update();
				break;
			case Parameter::HighCut:
				highCut.Frequency = value;
				highCut.Update();
				break;
			case Parameter::Dry:
				dryGain = value <= -29.99 ? 0 : DB2gain(value);
				break;
			case Parameter::Wet:
				wetGain = value <= -29.99 ? 0 : DB2gain(value);
				break;
			case Parameter::EarlyGain:
				earlyGain = value <= -29.99 ? 0 : DB2gain(value);
				break;
			case Parameter::LateGain:
				lateGain = value <= -29.99 ? 0 : DB2gain(value);
				break;
			case Parameter::EarlySize:
				earlySize = value;
				UpdateEarlyDiffusers();
				break;
			case Parameter::EarlyMod:
				earlyMod = value;
				UpdateEarlyDiffusers();
				break;
			case Parameter::EarlyRate:
				earlyRate = value;
				UpdateEarlyDiffusers();
				break;
			case Parameter::LateSize:
				lateSize = value;
				UpdateLateDiffusers();
				UpdateDelayLines();
				break;
			case Parameter::LateMod:
				lateMod = value;
				UpdateLateDiffusers();
				UpdateDelayLines();
				break;
			case Parameter::LateRate:
				lateRate = value;
				UpdateLateDiffusers();
				UpdateDelayLines();
				break;
			case Parameter::LateDecay:
				lateDecay = value;
				UpdateLateDiffusers();
				UpdateDelayLines();
				break;
			case Parameter::SeedEarly:
				seedEarly = (int)value;
				UpdateEarlyDiffusers();
				break;
			case Parameter::SeedDelay:
				seedDelay = (int)value;
				UpdateDelayLines();
				break;
			case Parameter::SeedDiffuse:
				seedDiffuse = (int)value;
				UpdateLateDiffusers();
				break;
			}
		}

		void Process(float* input, int sampleCount)
		{
			// Jumping straight between 100% feedback (freeze) and the selected feedback causes a click, needs to be smoothed
			smoothedFreeze = smoothedFreeze * 0.95 + (int)freeze * 0.05;
			float activeFeedback[DELAY_LINE_COUNT];
			for (size_t i = 0; i < DELAY_LINE_COUNT; i++)
			{
				activeFeedback[i] = smoothedFreeze + (1-smoothedFreeze) * feedback[i];
			}
			
			auto preDelayOut = preDelay->GetOutput();

			for (int i = 0; i < sampleCount; i++)
			{
				float x = input[i];
				x = lowCut.Process(x);
				x = highCut.Process(x);
				tempBuffer[i] = x;
			}

			preDelay->Process(tempBuffer, sampleCount);
			
			float* currentDiffuser = preDelayOut;
			for (int i = 0; i < earlyDiffuseStages; i++)
			{
				earlyDiffusers[i].Process(currentDiffuser, sampleCount);
				currentDiffuser = earlyDiffusers[i].GetOutput();
			}

			ZeroBuffer(tempBuffer, sampleCount); // tempbuffer becomes the out buffer
			for (int i = 0; i < delayLineCount; i++)
			{
				int lastDiffuserIdx;
				if (lateDiffuseStages == 0)
				 	lastDiffuserIdx = i;
				else if (lateDiffuseStages == 1)
					lastDiffuserIdx = i;
				else
					lastDiffuserIdx = DELAY_LINE_COUNT + i;
				
				float* lateDiffOut = lateDiffusers[lastDiffuserIdx].GetOutput();
				Mix(tempBuffer, lateDiffOut, GetPerLineGain(), sampleCount);

				// mix the input into the line
				ZeroBuffer(tempBuffer2, sampleCount);
				Mix(tempBuffer2, currentDiffuser, 1.0f, sampleCount);
				Mix(tempBuffer2, lateDiffOut, activeFeedback[i], sampleCount);

				delayLines[i].Process(tempBuffer2, sampleCount);
				float* lineOut = delayLines[i].GetOutput();

				// apply filters directly to delayLine output
				feedbackHi[i].Process(lineOut, lineOut, sampleCount);
				feedbackLo[i].Process(lineOut, lineOut, sampleCount);

				if (lateDiffuseStages == 0)
				{
					// if no late diffusers active, we just write the delay line output into the diffuse buffer
					ZeroBuffer(lateDiffOut, sampleCount);
					Mix(lateDiffOut, lineOut, 1.0f, sampleCount);
				}
				else if (lateDiffuseStages == 1)
				{
					lateDiffusers[i].Process(lineOut, sampleCount);
				}
				else
				{
					lateDiffusers[i].Process(lineOut, sampleCount);
					auto lateDiffOut = lateDiffusers[i].GetOutput();
					lateDiffusers[DELAY_LINE_COUNT + i].Process(lateDiffOut, sampleCount);
				}
			}
			
			for (int i = 0; i < sampleCount; i++)
			{
				outBuffer[i] = dryGain * input[i] + wetGain * (earlyGain * currentDiffuser[i] + lateGain * tempBuffer[i]);
			}
		}

		void ClearBuffers()
		{
			for (int i = 0; i < BUFFER_SIZE; i++)
			{
				outBuffer[i] = 0.0;
			}
		}


	private:
		FLASHMEM void UpdateMode()
		{
			if (mode == Mode::Fast)
            {
				earlyDiffuseStages = 6;
				earlyDiffFeedback = 0.85;
				lateDiffFeedback = 0.7;
				lateDiffuseStages = 1;
				useInterpolation = true;
				earlyDiffScaler = 95;
				lateDiffScaler = 95;
				delayLineCount = 6;
				delayLineScaler = 150;
				feedbackHiFc = 8000;
				feedbackLoFc = 40;
				feedbackDampDb = -2;
			}
            else if (mode == Mode::Plate)
            {
				earlyDiffuseStages = 6;
				earlyDiffFeedback = 0.8;
				lateDiffFeedback = 0.8;
				lateDiffuseStages = 2;
				useInterpolation = true;
				earlyDiffScaler = 95;
				lateDiffScaler = 95;
				delayLineCount = 6;
				delayLineScaler = 120;
				feedbackHiFc = 1500;
				feedbackLoFc = 20;
				feedbackDampDb = -6;
			}
            else if (mode == Mode::Sparse)
			{
				earlyDiffuseStages = 2;
				earlyDiffFeedback = 0.7;
				lateDiffFeedback = 0.7;
				lateDiffuseStages = 0;
				useInterpolation = true;
				earlyDiffScaler = 95;
				lateDiffScaler = 95;
				delayLineCount = 3;
				delayLineScaler = 240;
				feedbackHiFc = 1200;
				feedbackLoFc = 150;
				feedbackDampDb = -0;
			}
            else if (mode == Mode::Swell)
            {
				earlyDiffuseStages = 8;
				earlyDiffFeedback = 0.45;
				lateDiffFeedback = 0.5;
				lateDiffuseStages = 2;
				useInterpolation = true;
				earlyDiffScaler = 95;
				lateDiffScaler = 95;
				delayLineCount = 6;
				delayLineScaler = 240;
				feedbackHiFc = 2000;
				feedbackLoFc = 20;
				feedbackDampDb = -2;
			}
            else if (mode == Mode::Plains)
            {
				earlyDiffuseStages = 6;
				earlyDiffFeedback = 0.6;
				lateDiffFeedback = 0.6;
				lateDiffuseStages = 2;
				useInterpolation = true;
				earlyDiffScaler = 95;
				lateDiffScaler = 95;
				delayLineCount = 6;
				delayLineScaler = 240;
				feedbackHiFc = 3000;
				feedbackLoFc = 40;
				feedbackDampDb = -2;
			}
            else if (mode == Mode::Wash)
            {
				earlyDiffuseStages = 6;
				earlyDiffFeedback = 0.7;
				lateDiffFeedback = 0.7;
				lateDiffuseStages = 1;
				useInterpolation = false;
				earlyDiffScaler = 95;
				lateDiffScaler = 95;
				delayLineCount = 6;
				delayLineScaler = 180;
				feedbackHiFc = 20000;
				feedbackLoFc = 20;
				feedbackDampDb = 0;
			}

			UpdateFeedbackFilters();
			UpdateLateDiffusers();
			UpdateEarlyDiffusers();
			UpdateDelayLines();
			UpdateInterpolation();
		}

		void UpdateInterpolation()
		{
			for (size_t i = 0; i < EARLY_DIFFUSER_COUNT; i++)
			{
				earlyDiffusers[i].InterpolationEnabled = useInterpolation;
			}
		}

		void UpdateFeedbackFilters()
		{
			for (size_t i = 0; i < DELAY_LINE_COUNT; i++)
			{
				feedbackHi[i].Frequency = feedbackHiFc;
				feedbackHi[i].SetGainDb(feedbackDampDb);
				feedbackHi[i].Update();

				feedbackLo[i].Frequency = feedbackLoFc;
				feedbackLo[i].SetGainDb(feedbackDampDb);
				feedbackLo[i].Update();
			}
		}

		void UpdateDelayLines(bool resetPhase=false)
		{
			random.SetSeed(seedDelay + leftChannel);
			//delayLineScaler = 240.0f;
			for (size_t i = 0; i < DELAY_LINE_COUNT; i++)
			{
				delayLines[i].SampleDelay = Ms2Samples((0.5f + 0.5f * random.NextFloat()) * delayLineScaler * lateSize);
				auto dbAfter1Iteration = delayLines[i].SampleDelay / Ms2Samples(lateDecay*1000) * (-60);
			 	auto gainAfter1Iteration = DB2gain(dbAfter1Iteration);
			 	feedback[i] = (float)gainAfter1Iteration;

				delayLines[i].ModAmount = Ms2Samples(lateMod * 2);
				delayLines[i].ModRate = (lateRate / samplerate) * (0.5f + 0.5f * random.NextFloat());
				if (resetPhase) delayLines[i].ResetPhase(random.NextFloat());
			}
		}

		void UpdateEarlyDiffusers(bool resetPhase=false)
		{
			random.SetSeed(seedEarly + leftChannel);

			for (size_t i = 0; i < EARLY_DIFFUSER_COUNT; i++)
			{
				auto d = powf(10, random.NextFloat()) * 0.1; // random value distributed over range 0.1 ... 1.0
				int sampleDelay = Ms2Samples(d) * earlyDiffScaler * earlySize;
				earlyDiffusers[i].SampleDelay = sampleDelay;
				earlyDiffusers[i].ModAmount = Ms2Samples(earlyMod * 2);
				earlyDiffusers[i].ModRate = (earlyRate / samplerate) * (0.5f + 0.5f * random.NextFloat());
				earlyDiffusers[i].Feedback = earlyDiffFeedback;
				if (resetPhase) earlyDiffusers[i].ResetPhase(random.NextFloat());
			}
		}

		void UpdateLateDiffusers(bool resetPhase=false)
		{
			random.SetSeed(seedDiffuse + leftChannel);

			for (size_t i = 0; i < DELAY_LINE_COUNT * LATE_DIFFUSER_COUNT; i++)
			{
				auto d = powf(10, random.NextFloat()) * 0.1; // random value distributed over range 0.1 ... 1.0
				int sampleDelay = Ms2Samples(d) * lateDiffScaler; // Sized is not user scalable for late diffusers
				lateDiffusers[i].SampleDelay = sampleDelay;
				lateDiffusers[i].ModAmount = Ms2Samples(lateMod * 2);
				lateDiffusers[i].ModRate = (lateRate / samplerate) * (0.5f + 0.5f * random.NextFloat());
				lateDiffusers[i].Feedback = lateDiffFeedback;
				if (resetPhase) lateDiffusers[i].ResetPhase(random.NextFloat());
			}
		}

		double GetPerLineGain()
		{
			return 1.0 / std::sqrt(delayLineCount);
		}

		double Ms2Samples(double value)
		{
			return value / 1000.0 * samplerate;
		}

	};

}
