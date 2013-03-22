#ifndef SOUNDSOURCE_H_
#define SOUNDSOURCE_H_

namespace vam
{
	class SoundSource
	{
	public:
		virtual const float* readSample() = 0;
		virtual int getChannels() const = 0;
	};
}

#endif
