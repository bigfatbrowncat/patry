/*
 * VorbisFileReader.cpp
 *
 *  Created on: Mar 20, 2013
 *      Author: imizus
 */

#include "VorbisFileReader.h"

namespace vam
{

	void VorbisFileReader::throwVorbisError(int code, string caller)
	{
		state = sError;

		switch (code)
		{
		case OV_HOLE:
			errorType = etHole;
			break;
		case OV_ENOSEEK:
			errorType = etCantSeek;
			break;
		case OV_EREAD:
			errorType = etCantRead;
			break;
		case OV_EBADLINK:
			errorType = etBadLink;
			break;
		case OV_EINVAL:
			errorType = etInvalidArgument;
			break;
		case OV_ENOTVORBIS:
			errorType = etNotAnOgg;
			break;
		case OV_EVERSION:
			errorType = etVersionIncorrect;
			break;
		case OV_EBADHEADER:
			errorType = etBadHeader;
			break;
		case OV_EFAULT:
			errorType = etMemoryFault;
			break;

		default:
			break;
		}

		throw Error(errorType, code, caller);
	}

	void VorbisFileReader::throwStrangeError(int code, string caller)
	{
		state = sError;
		errorType = etStrangeError;
		throw Error(errorType, code, caller);
	}

	void VorbisFileReader::throwError(ErrorType type, string caller)
	{
		state = sError;
		errorType = type;
		throw Error(errorType, 0, caller);
	}

	void VorbisFileReader::fillBuffer()
	{
		buffer_start_time = ov_time_tell(&vf);
		if (buffer_start_time < 0)
		{
			throwVorbisError((int)buffer_start_time, "fillBuffer (1)");
		}

		int ret = ov_read_float(&vf, &buffer, buffer_size_request, &current_section);
		if (ret < 0)
		{
			throwVorbisError(ret, "fillBuffer (2)");
		}
		else if (ret == 0)
		{
			state = sEndOfData;
		}
		else
		{
			buffer_position = 0;
			buffer_size = ret;
			state = sReady;
		}
	}

	void VorbisFileReader::updatePlayhead()
	{
		playhead = buffer_start_time + (double)buffer_position / rate;
	}


	VorbisFileReader::VorbisFileReader(string file_name, int buffer_size_request) :
		buffer_size_request(buffer_size_request)
	{
		file = fopen(file_name.c_str(), "rb");

		if (file == NULL)
		{
			throwError(etCantOpen, "constructor (1)");
		}

		// Opening vorbis file

		int ret = ov_open_callbacks(file, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE);
		if (ret < 0)
		{
			throwVorbisError(ret, "constructor (2)");
		}


		if (ret == 0)
		{

			// Reading the comments
			char **ptr = ov_comment(&vf, -1)->user_comments;
			vorbis_info *vi = ov_info(&vf, -1);
			while (*ptr)
			{
				comments.push_back(string(*ptr));
				++ptr;
			}

			bitsPerSecond = ov_bitrate(&vf, -1);
			channels = vi->channels;
			rate = vi->rate;
			read_buffer = new float[channels];
			vendor = string(ov_comment(&vf,-1)->vendor);
			length = ov_time_total(&vf, -1);
			playhead = 0;

			fillBuffer();

		}

	}

	VorbisFileReader::~VorbisFileReader()
	{
		delete [] read_buffer;
		ov_clear(&vf);
		if (file != NULL) fclose(file);
	}

	const float* VorbisFileReader::readSample()
	{
		bool output_silence = false;
		if (state == sReady)
		{
			// Check if the buffer is filled
			if (buffer_position == buffer_size)
			{
				fillBuffer();
			}

			// Buffer is ready, reading a sample
			if (state == sReady)
			{
				for (int i = 0; i < channels; i++)
				{
					read_buffer[i] = buffer[i][buffer_position];
				}
				buffer_position++;
				updatePlayhead();

				return read_buffer;
			}
			else
			{
				output_silence = true;
			}

		}
		else
		{
			output_silence = true;
		}

		if (output_silence)
		{
			// If the reader isn't in the Ready state, returning zero
			for (int i = 0; i < channels; i++)
			{
				read_buffer[i] = 0.f;
			}
			return read_buffer;
		}

	}

	void VorbisFileReader::rewind(double position)
	{
		if (position > length || position < 0)
		{
			throwError(etSeekOutOfRange, "rewind (1)");
		}

		int ret = ov_time_seek(&vf, position);
		if (ret < 0)
		{
			throwVorbisError(ret, "rewind (2)");
		}

		if (ret == 0)
		{
			fillBuffer();
		}
		updatePlayhead();
	}


} /* namespace va */
