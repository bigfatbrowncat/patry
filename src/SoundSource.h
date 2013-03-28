#ifndef SOUNDSOURCE_H_
#define SOUNDSOURCE_H_

#define MAX_CHANNELS			2

namespace vam
{
	class SoundSource
	{
	public:
		enum State
		{
			sBeforeStart = 0,
			sReading = 1,
			sAfterEnd = 2,
			sError = 3
		};

	protected:
		State state;

	public:
		SoundSource() : state(sReading) {}

		virtual const float* readSample() = 0;
		virtual int getChannels() const = 0;
		virtual void rewind(double position) = 0;
		virtual double getPlayhead() const = 0;
		virtual double getStartTime() const = 0;
		virtual double getEndTime() const = 0;
		virtual int getRate() const = 0;
		virtual State getState() const { return state; }

		virtual ~SoundSource() {}
	};
}

#endif
