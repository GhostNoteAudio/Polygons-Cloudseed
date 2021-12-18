#pragma once

#include "Constants.h"
#include "Utils.h"
#include <stdint.h>

using namespace Polygons;

namespace CloudSeed
{
	template<int N>
	class ModulatedDelay
	{
	private:

		static const int ModulationUpdateRate = 8;
		static const int DelayBufferSizeSamples = N;
		static constexpr float SAMPLE_MAX16_INV = (float)(1.0 / INT16_MAX);

		int16_t delayBuffer[N];
		float output[BUFFER_SIZE];
		int writeIndex;
		int readIndexA;
		int readIndexB;
		int samplesProcessed;
		

		double modPhase;
		double gainA;
		double gainB;

	public:
		int SampleDelay;
		double ModAmount; // Sample excursion
		double ModRate; // Frequency / Samplerate

		ModulatedDelay()
		{
			SampleDelay = 1000;
			writeIndex = 0;
			modPhase = 0.0;
			ModRate = 0.0;
			ModAmount = 0.0;
			ClearBuffers();
			Update();
		}

		float* GetOutput()
		{
			return output;
		}

		FLASHMEM void Process(float* input, int sampleCount)
		{
			for (int i = 0; i < sampleCount; i++)
			{
				if (samplesProcessed == ModulationUpdateRate)
					Update();

				// sampleCount must be an even number!
				if ((i & 0x01) == 0) // only update every other sample, delay line runs at half the sample frequency
				{
					delayBuffer[writeIndex] = (int16_t)(Clip1(input[i]*0.5) * (INT16_MAX-1));
					output[i] = delayBuffer[readIndexA] * gainA + delayBuffer[readIndexB] * gainB;
					output[i] *= SAMPLE_MAX16_INV * 2;

					writeIndex++;
					readIndexA++;
					readIndexB++;
					if (writeIndex >= DelayBufferSizeSamples) writeIndex -= DelayBufferSizeSamples;
					if (readIndexA >= DelayBufferSizeSamples) readIndexA -= DelayBufferSizeSamples;
					if (readIndexB >= DelayBufferSizeSamples) readIndexB -= DelayBufferSizeSamples;
				}
				else
				{
					output[i] = output[i-1];
				}
				samplesProcessed++;
			}
		}

		FLASHMEM void ClearBuffers()
		{
			ZeroBuffer(delayBuffer, DelayBufferSizeSamples);
			ZeroBuffer(output, BUFFER_SIZE);
		}

		FLASHMEM void ResetPhase(float phase)
		{
			this->modPhase = fmodf(phase, 1.0f);
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
			totalDelay /= 2; // Half the total delay because delay line runs at 0.5*Fs

			auto delayA = (int)totalDelay;
			auto delayB = (int)totalDelay + 1;

			auto partial = totalDelay - delayA;

			gainA = 1 - partial;
			gainB = partial;

			readIndexA = writeIndex - delayA;
			readIndexB = writeIndex - delayB;
			if (readIndexA < 0) readIndexA += DelayBufferSizeSamples;
			if (readIndexB < 0) readIndexB += DelayBufferSizeSamples;

			samplesProcessed = 0;
		}
	};
}
