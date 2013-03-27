package vam;

public class MovedSound extends SoundSource
{
	private SoundSource sound;
	
	private native static long createNativeInstance();
	
	/**
	 * Frees the native allocated object
	 */
	private native void destroyNativeInstance();
	
	public MovedSound()
	{
		super(createNativeInstance());
	}

	/**
	 * Closes the file and releases all native resources.
	 */
	public void close()
	{
		if (nativeInstance != 0)
		{
			destroyNativeInstance();
			nativeInstance = 0;
		}
	}
	
	@Override
	protected void finalize() throws Throwable
	{
		try
		{
			close();
		}
		finally
		{
			super.finalize();
		}
	}
		
	
	@Override
	public native float[] readSample() throws Error;

	@Override
	public native int getChannels();

	@Override
	public native void rewind(double position) throws Error;

	@Override
	public native double getPlayhead();
	
	@Override
	public native double getLength();

	@Override
	public native int getRate();

	private native void setSoundNative(SoundSource sound);

	public void setSound(SoundSource sound)
	{
		setSoundNative(sound);
		this.sound = sound; 
	}
	
	public native void setDelay(double value);
}
