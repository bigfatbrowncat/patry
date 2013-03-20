/*
 * VorbisFileReader.h
 *
 *  Created on: Mar 20, 2013
 *      Author: imizus
 */

#ifndef VORBISFILEREADER_H_
#define VORBISFILEREADER_H_

#include <string>
#include <vector>

#include <vorbis/vorbisfile.h>

using namespace std;

namespace vam
{

	class VorbisFileReader
	{
	public:
		enum ErrorType
		{
			etCantOpen, etNotAnOgg, etCantRead, etVersionIncorrect, etBadHeader, etMemoryFault,
			etHole, etBadLink, etInvalidArgument, etStrangeError, etCantSeek, etSeekOutOfRange
		};

		class Error
		{
			friend class VorbisFileReader;
		private:
			ErrorType type;
			int code;
			string caller;
			Error(ErrorType type, int code, string caller) : type(type), code(code), caller(caller)
			{
			}
		public:
			ErrorType getType() const { return type; }
			int getCode() const { return code; }
			string getCaller() const { return caller; }
		};

		enum State
		{
			sReady, sEndOfData, sError
		};
	private:
		OggVorbis_File vf;
		int current_section;

		int buffer_position;
		float** buffer;			// This buffer belongs to the Ogg Vorbis decoder
		int buffer_size;

		FILE* file;
		float* read_buffer;		// This buffer is our

		int buffer_size_request;

		State state;
		ErrorType errorType;
		double buffer_start_time;
		double playhead;
		double length;
		int channels;
		int rate;
		int bitsPerSecond;
		string vendor;
		vector<string> comments;

	private:
		void throwVorbisError(int code, string caller);
		void throwStrangeError(int code, string caller);
		void throwError(ErrorType type, string caller);

		void fillBuffer();
		void updatePlayhead();

	public:
		VorbisFileReader(string file_name, int buffer_size_request);
		virtual ~VorbisFileReader();

		const float* readSample();
		void rewind(double position);

		State getState() const { return state; }
		ErrorType getErrorType() const { return errorType; }
		double getPlayhead() const { return playhead; }
		double getLength() const { return length; }
		long getChannels() const { return channels; }
		long getRate() const { return rate; }
		int getBitsPerSecond() const { return bitsPerSecond; }
		const string& getVendor() const { return vendor; }
		const vector<string> getComments() const { return comments; }
	};


} /* namespace va */
#endif /* VORBISFILEREADER_H_ */
