#ifndef SOUNDSOURCE_H_
#define SOUNDSOURCE_H_

namespace vam
{
	class SoundSource
	{
	public:
		virtual const float* readSample() = 0;
		virtual int getChannels() const = 0;
		virtual void rewind(double position) = 0;
		virtual double getPlayhead() const = 0;
		virtual double getLength() const = 0;
		virtual int getRate() const = 0;

	};
}

#endif
