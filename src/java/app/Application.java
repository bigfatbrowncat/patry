package app;

import vam.VorbisFileReader;

public class Application
{
	public static void main(String[] args)
	{
		System.out.println("hello, world!");
		int p = callNative(3);
		System.out.println("callNative returned " + p);
		
		VorbisFileReader vfr = new VorbisFileReader("test/test_44100_1.ogg", 64);
		float[] ff = vfr.readSample();

		vfr.close();
	}
	
	private native static int callNative(int v);	
}