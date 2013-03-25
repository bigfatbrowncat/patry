package vam;

import vam.VorbisFileReader.Error;
import vam.VorbisFileReader.ErrorType;

public class MixedSounds extends SoundSource
{
	public enum ErrorType
	{
		etUnsupportedChannelsNumber(0);
		
		@SuppressWarnings("unused")	// used in native code
		private int value;
		
		ErrorType(int value) { this.value = value; }
		static ErrorType fromValue(int i)
		{
			switch (i)
			{
			case 0: return etUnsupportedChannelsNumber;
			default:
				throw new RuntimeException("Strange value");
			}
		}
	}

	public static class Error extends SoundSource.Error
	{
		private static final long serialVersionUID = 6886145280554167342L;
		private ErrorType type;
		private String caller;
		protected Error(ErrorType type, String caller)
		{
			super("error type " + type.toString() + ", caller is " + caller);
			this.type = type;
			this.caller = caller;
		}
		public ErrorType getType() { return type; }
		public String getCaller() { return caller; }		
	}

	private native static long createNativeInstance();
	
	/**
	 * Frees the native allocated object
	 */
	private native void destroyNativeInstance();
	
	
	protected MixedSounds(long nativeInstance)
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

}
