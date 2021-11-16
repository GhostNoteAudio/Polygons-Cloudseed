#pragma once

#include "Constants.h"
#include "Utils.h"
#include <stdint.h>

namespace Revo
{
	class ModulatedAllpass
	{
	public:
		static const int DelayBufferSamples = FS_MAX / 10; // 100ms
		static const int ModulationUpdateRate = 8;
		static constexpr float SAMPLE_MAX16_INV = (float)(1.0 / INT16_MAX);

	private:
		int16_t delayBuffer[DelayBufferSamples];
		float output[BUFFER_SIZE];
		float fsInv;
		int index;
		unsigned int samplesProcessed;

		float modPhase;
		int delayA;
		int delayB;
		float gainA;
		float gainB;

	public:
		int SampleDelay;
		float Feedback;
		float ModAmount; // Sample excursion
		float ModRate; // Frequency / Samplerate
		bool InterpolationEnabled;

		ModulatedAllpass()
		{
			this->InterpolationEnabled = true;
			SampleDelay = 100;
			Feedback = 0;
			index = DelayBufferSamples - 1;
			modPhase = 0.0;
			ModRate = 0.0;
			ModAmount = 0.0;
			ClearBuffers();
			Update();
		}

		FLASHMEM float* GetOutput()
		{
			return output;
		}

		FLASHMEM void ClearBuffers()
		{
			ZeroBuffer(delayBuffer, DelayBufferSamples);
			ZeroBuffer(output, BUFFER_SIZE);
		}

		FLASHMEM void ResetPhase(float phase)
		{
			this->modPhase = fmodf(phase, 1.0f);
		}

		FLASHMEM void Process(float* input, int sampleCount)
		{
			bool interp = InterpolationEnabled;

			for (int i = 0; i < sampleCount; i++)
			{
				if (samplesProcessed >= ModulationUpdateRate)
					Update();

				float bufOut;

				if (interp)
				{
					int idxA = index - delayA;
					int idxB = index - delayB;
					idxA += DelayBufferSamples * (idxA < 0); // modulo
					idxB += DelayBufferSamples * (idxB < 0); // modulo

					bufOut = delayBuffer[idxA] * gainA + delayBuffer[idxB] * gainB;
					bufOut *= SAMPLE_MAX16_INV;
				}
				else
				{
					int idxA = index - delayA;
					idxA += DelayBufferSamples * (idxA < 0); // modulo
					bufOut = delayBuffer[idxA];
					bufOut *= SAMPLE_MAX16_INV;
				}

				auto inVal = Clip1(input[i] + bufOut * Feedback);
				delayBuffer[index] = (int16_t)(inVal * (INT16_MAX-1));
				output[i] = bufOut - inVal * Feedback;

				index++;
				if (index >= DelayBufferSamples) index -= DelayBufferSamples;
				
				samplesProcessed++;
			}
		}

	private:
		FLASHMEM void Update()
		{
			modPhase += ModRate * ModulationUpdateRate;
			if (modPhase > 1)
				modPhase -= 1.0;

			auto mod = sinf(modPhase * 2 * M_PI);

			if (ModAmount >= SampleDelay) // don't modulate to negative value
				ModAmount = SampleDelay - 1;

			auto totalDelay = SampleDelay + ModAmount * mod;
			
			if (totalDelay <= 0) // should no longer be required
				totalDelay = 1;

			delayA = (int)totalDelay;
			delayB = (int)totalDelay + 1;

			auto partial = totalDelay - delayA;

			gainA = 1 - partial;
			gainB = partial;

			samplesProcessed = 0;
		}
	};
}
