#define _USE_MATH_DEFINES
#include <math.h>
#include "Biquad.h"

namespace Revo
{
	Biquad::Biquad() 
	{
		ClearBuffers();
	}

	Biquad::Biquad(FilterType filterType, float samplerate)
	{
		Type = filterType;
		Slope = 1;
		SetSamplerate(samplerate);

		SetGainDb(0.0);
		Frequency = samplerate / 4;
		SetQ(0.5);
		ClearBuffers();
	}

	float Biquad::GetSamplerate() 
	{
		return samplerate;
	}

	void Biquad::SetSamplerate(float value)
	{
		samplerate = value; 
		Update();
	}

	float Biquad::GetGainDb() 
	{
		return log10f(gain) * 20;
	}

	void Biquad::SetGainDb(float value) 
	{
		SetGain(powf(10, value / 20));
	}

	float Biquad::GetGain() 
	{
		return gain;
	}

	void Biquad::SetGain(float value) 
	{
		if (value == 0)
			value = 0.001; // -60dB
		
		gain = value;
	}

	float Biquad::GetQ()
	{
		return _q;
	}

	void Biquad::SetQ(float value) 
	{
		if (value == 0)
			value = 1e-12;
		_q = value;
	}

	float* Biquad::GetA() 
	{
		aVec[0] = 0;
		aVec[1] = a1;
		aVec[2] = a2;
		return aVec;
	}

	float* Biquad::GetB()
	{
		bVec[0] = b0;
		bVec[1] = b1;
		bVec[2] = b2;
		return bVec;
	}

	void Biquad::Update()
	{
		float omega = 2.0f * M_PI * Frequency / samplerate;
		float sinOmega = sinf(omega);
		float cosOmega = cosf(omega);

		float sqrtGain = 0.0f;
		float alpha = 0.0f;

		if (Type == FilterType::LowShelf || Type == FilterType::HighShelf)
		{
			alpha = sinOmega / 2.0f * sqrtf((gain + 1.0f / gain) * (1.0f / Slope - 1) + 2);
			sqrtGain = sqrtf(gain);
		}
		else
		{
			alpha = sinOmega / (2.0f * _q);
		}

		switch (Type)
		{
		case FilterType::LowPass:
			b0 = (1 - cosOmega) / 2;
			b1 = 1 - cosOmega;
			b2 = (1 - cosOmega) / 2;
			a0 = 1 + alpha;
			a1 = -2 * cosOmega;
			a2 = 1 - alpha;
			break;
		case FilterType::HighPass:
			b0 = (1 + cosOmega) / 2;
			b1 = -(1 + cosOmega);
			b2 = (1 + cosOmega) / 2;
			a0 = 1 + alpha;
			a1 = -2 * cosOmega;
			a2 = 1 - alpha;
			break;
		case FilterType::BandPass:
			b0 = alpha;
			b1 = 0;
			b2 = -alpha;
			a0 = 1 + alpha;
			a1 = -2 * cosOmega;
			a2 = 1 - alpha;
			break;
		case FilterType::Notch:
			b0 = 1;
			b1 = -2 * cosOmega;
			b2 = 1;
			a0 = 1 + alpha;
			a1 = -2 * cosOmega;
			a2 = 1 - alpha;
			break;
		case FilterType::Peak:
			b0 = 1 + (alpha * gain);
			b1 = -2 * cosOmega;
			b2 = 1 - (alpha * gain);
			a0 = 1 + (alpha / gain);
			a1 = -2 * cosOmega;
			a2 = 1 - (alpha / gain);
			break;
		case FilterType::LowShelf:
			b0 = gain * ((gain + 1) - (gain - 1) * cosOmega + 2 * sqrtGain * alpha);
			b1 = 2 * gain * ((gain - 1) - (gain + 1) * cosOmega);
			b2 = gain * ((gain + 1) - (gain - 1) * cosOmega - 2 * sqrtGain * alpha);
			a0 = (gain + 1) + (gain - 1) * cosOmega + 2 * sqrtGain * alpha;
			a1 = -2 * ((gain - 1) + (gain + 1) * cosOmega);
			a2 = (gain + 1) + (gain - 1) * cosOmega - 2 * sqrtGain * alpha;
			break;
		case FilterType::HighShelf:
			b0 = gain * ((gain + 1) + (gain - 1) * cosOmega + 2 * sqrtGain * alpha);
			b1 = -2 * gain * ((gain - 1) + (gain + 1) * cosOmega);
			b2 = gain * ((gain + 1) + (gain - 1) * cosOmega - 2 * sqrtGain * alpha);
			a0 = (gain + 1) - (gain - 1) * cosOmega + 2 * sqrtGain * alpha;
			a1 = 2 * ((gain - 1) - (gain + 1) * cosOmega);
			a2 = (gain + 1) - (gain - 1) * cosOmega - 2 * sqrtGain * alpha;
			break;
		}

		float g = 1 / a0;

		b0 = b0 * g;
		b1 = b1 * g;
		b2 = b2 * g;
		a1 = a1 * g;
		a2 = a2 * g;
	}

	float Biquad::GetResponse(float freq)
	{
		float phi = powf((sinf(2 * M_PI * freq / (2.0 * samplerate))), 2);
		return (powf(b0 + b1 + b2, 2.0) - 4.0 * (b0 * b1 + 4.0 * b0 * b2 + b1 * b2) * phi + 16.0 * b0 * b2 * phi * phi) / (powf(1.0 + a1 + a2, 2.0) - 4.0 * (a1 + 4.0 * a2 + a1 * a2) * phi + 16.0 * a2 * phi * phi);
	}

	void Biquad::ClearBuffers() 
	{
		y = 0;
		x2 = 0;
		y2 = 0;
		x1 = 0;
		y1 = 0;
	}
}
