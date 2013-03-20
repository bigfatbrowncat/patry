/*
 * PortAudioClass.h
 *
 *  Created on: Mar 20, 2013
 *      Author: imizus
 */

#ifndef PORTAUDIOCLASS_H_
#define PORTAUDIOCLASS_H_

#include <portaudio/portaudio.h>

namespace vam
{
	class PortAudioClass
	{
	public:
		class Error
		{
			friend class PortAudioClass;
		private:
			PaError code;
		protected:
			Error(PaError code) : code(code) {}
			virtual ~Error() {}
		public:
			PaError getCode() const { return code; }
		};

	private:
		static int portAudioObjectsCounter;
	protected:
		virtual void checkError(PaError code);
	public:
		PortAudioClass();
		virtual ~PortAudioClass();
	};

} /* namespace va */
#endif /* PORTAUDIOCLASS_H_ */
