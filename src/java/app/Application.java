package app;

public class Application
{
	public static void main(String[] args)
	{
		System.out.println("hello, world!");
		int p = callNative(3);
		System.out.println("callNative returned " + p);
	}
	
	private native static int callNative(int v);	
}