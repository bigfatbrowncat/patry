#ifndef SOUNDSOURCE_H_
#define SOUNDSOURCE_H_

namespace vam
{
	class SoundSource
	{
	public:
		enum State
		{
			sReady = 0,
			sEndOfData = 1,
			sError = 2
		};

	protected:
		State state;

	public:
		SoundSource() : state(sReady) {}

		virtual const float* readSample() = 0;
		virtual int getChannels() const = 0;
		virtual void rewind(double position) = 0;
		virtual double getPlayhead() const = 0;
		virtual double getLength() const = 0;
		virtual int getRate() const = 0;
		virtual State getState() const { return state; }
	};
}

#endif
