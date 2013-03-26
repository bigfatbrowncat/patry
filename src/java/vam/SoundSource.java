package vam;

public abstract class SoundSource
{
	public enum State
	{
		sReady(0),
		sEndOfData(1),
		sError(2);

		@SuppressWarnings("unused")	// used in native code
		private int value;
		
		State(int value) { this.value = value; }
		static State fromValue(int i)
		{
			switch (i)
			{
			case 0: return sReady;
			case 1: return sEndOfData;
			case 2: return sError;
			default:
				throw new RuntimeException("Strange value: " + i);
			}
		}
	}
		
	public static class Error extends Exception
	{
		private static final long serialVersionUID = 8833341961960069603L;

		public Error(String message)
		{
			super(message);
		}
	}
	
	/**
	 * The address of the native SoundSource object in memory. 
	 * This object should be constructed by the real descendant.
	 * 
	 * Used in native code
	 */
	protected long nativeInstance = 0;

	protected SoundSource(long nativeInstance)
	{
		this.nativeInstance = nativeInstance;
	}
	
	public abstract float[] readSample() throws Error;
	public abstract int getChannels();
	public abstract void rewind(double position) throws Error;
	public abstract double getPlayhead();
	public abstract double getLength();
	public abstract int getRate();
	
	public native State getState();

}
