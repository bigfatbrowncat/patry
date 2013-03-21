package vam;

public class VorbisFileReader
{
	public enum ErrorType
	{
		etCantOpen(0), 
		etNotAnOgg(1), 
		etCantRead(2),
		etVersionIncorrect(3),
		etBadHeader(4),
		etMemoryFault(5),
		etHole(6),
		etBadLink(7),
		etInvalidArgument(8),
		etStrangeError(9),
		etCantSeek(10),
		etSeekOutOfRange(11);
		
		private int value;
		ErrorType(int value) { this.value = value; }
		int getValue() { return value; }
	}
	
	public class Error extends Exception
	{
		private static final long serialVersionUID = 334769002082148556L;
		private ErrorType type;
		private int code;
		private String caller;
		protected Error(ErrorType type, int code, String caller)
		{
			this.type = type;
			this.code = code;
			this.caller = caller;
		}
		public ErrorType getType() { return type; }
		public int getCode() { return code; }
		public String getCaller() { return caller; }		
	}
	
	public enum State
	{
		sReady(0),
		sEndOfData(1),
		sError(2);

		private int value;
		State(int value) { this.value = value; }
		int getValue() { return value; }
		static State fromValue(int i)
		{
			switch (i)
			{
			case 0: return sReady;
			case 1: return sEndOfData;
			case 2: return sError;
			default:
				throw new RuntimeException("Strange value");
			}
		}
	}
	
	// Private fields and methods are implemented in C++ class
	private long nativeInstance = 0;
	private native void createNativeInstance(String file_name, int buffer_size_request);
	private native void destroyNativeInstance();
	
	public VorbisFileReader(String file_name, int buffer_size_request)
	{
		createNativeInstance(file_name, buffer_size_request);
	}
	
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
	
	public native float[] readSample();
	public native void rewind(double position);
	
	public native State getState();
	public native ErrorType getErrorType();
	public native double getPlayhead();
	public native double getLength();
	public native long getChannels();
	public native long getRate();
	public native int getBitsPerSecond();
	public native String getVendor();
	public native String[] getComments();
}
