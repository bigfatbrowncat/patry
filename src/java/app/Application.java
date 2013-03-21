package app;

import vam.VorbisFileReader;
import vam.VorbisFileReader.Error;

public class Application
{
	public static void main(String[] args)
	{
		try
		{
			VorbisFileReader vfr = new VorbisFileReader("test/test_44100_1.ogg", 64);
			float[] ff = vfr.readSample();
			vfr.close();
		} 
		catch (Error e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}