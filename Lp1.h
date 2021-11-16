#pragma once

#include <cmath>

#define x_PI 3.141592653589793238462643383

namespace Revo
{
	class Lp1
	{
	public:
		float Output;

	public:
		Lp1(float fs)
		{
			this->fs = fs;
		}

		float GetSamplerate()
		{
			return fs;
		}

		void SetSamplerate(float samplerate)
		{
			fs = samplerate;
		}

		float GetCutoffHz()
		{
			return cutoffHz;
		}

		void SetCutoffHz(float hz)
		{
			cutoffHz = hz;
			Update();
		}

		void Update()
		{
			// Prevent going over the Nyquist frequency
			if (cutoffHz >= fs * 0.5f)
				cutoffHz = fs * 0.499f;

			auto x = 2.0f * x_PI * cutoffHz / fs;
			auto nn = (2.0f - cosf(x));
			auto alpha = nn - sqrtf(nn * nn - 1);

			a1 = alpha;
			b0 = 1 - alpha;
		}

		float Process(float input)
		{
			if (input == 0 && Output < 0.0000001f)
			{
				Output = 0;
			}
			else
			{
				Output = b0 * input + a1 * Output;
			}
			return Output;
		}

		void Process(float* input, float* output, int len)
		{
			for (int i = 0; i < len; i++)
				output[i] = Process(input[i]);
		}


	private:
		float fs;
		float b0, a1;
		float cutoffHz;
	};
}

