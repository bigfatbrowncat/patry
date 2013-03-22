package vam;

import vam.VorbisFileReader.Error;

public abstract class SoundSource
{
	/**
	 * The address of the native SoundSource object in memory. Used in native code
	 */
	protected long nativeInstance = 0;

	protected SoundSource(long nativeInstance)
	{
		this.nativeInstance = nativeInstance;
	}
	
	public abstract float[] readSample() throws Error;
	public abstract int getChannels();
}
