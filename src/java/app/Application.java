package app;

import vam.MixedSounds;
import vam.PortAudioPlayer;
import vam.VorbisFileReader;

public class Application
{
	public static final int RESULT_SUCCESS				= 0;
	public static final int RESULT_NO_INPUT_FILE		= 1;
	public static final int RESULT_VORBIS_ERROR			= 2;
	public static final int RESULT_MIXED_ERROR			= 3;

	private static String twoDig(int value)
	{
		if (value >= 10) 
			return String.valueOf(value);
		else 
			return "0" + String.valueOf(value);
	}
	
	public static void main(String[] args)
	{
		try
		{
			MixedSounds ms = new MixedSounds();
			VorbisFileReader vfr1 = new VorbisFileReader("../Drums.ogg", 64);
			VorbisFileReader vfr2 = new VorbisFileReader("../Bass.ogg", 64);
			ms.addSound(vfr1);
			ms.addSound(vfr2);

			ms.rewind(-3);
			
			PortAudioPlayer pap = new PortAudioPlayer(ms.getChannels(), ms.getRate(), 1024);

			pap.setSoundSource(ms);
			pap.play();
			
			while (ms.getState() != VorbisFileReader.State.sAfterEnd)
			{
				try
				{
					Thread.sleep(10);
					
					double playhead = Math.abs(ms.getPlayhead());
					
					int min = (int)(playhead) / 60;
					int sec = (int)(playhead) % 60;
					int sp10 = (int)(playhead * 100) % 100;
					
					int lmin = (int)(ms.getLength()) / 60;
					int lsec = (int)(ms.getLength()) % 60;
					int lsp10 = (int)(ms.getLength() * 100) % 100;
					
					System.out.print("\rPlaying the file... [ " + (ms.getPlayhead() < 0 ? "-" : " ") + twoDig(min) + ":" + twoDig(sec) + "." + twoDig(sp10) + " / " + twoDig(lmin) + ":" + twoDig(lsec) + "." + twoDig(lsp10) + "  ]");
					
				} 
				catch (InterruptedException e)
				{
					e.printStackTrace();
				}
			}
			
			pap.close();
			ms.close();
			System.gc();
			
			System.out.println("\nBye.");
			System.exit(RESULT_SUCCESS);
		} 
		catch (VorbisFileReader.Error e)
		{
			e.printStackTrace();
			System.exit(RESULT_VORBIS_ERROR);
		} 
		catch (MixedSounds.Error e)
		{
			e.printStackTrace();
			System.exit(RESULT_MIXED_ERROR);
		}
	}	
	
	public static void main0(String[] args)
	{
		try
		{
			if (args.length == 0)
			{
				System.err.println("No input file");
				System.exit(RESULT_NO_INPUT_FILE);
				return;
			}

			System.out.println("Input file: " + args[0]);

			VorbisFileReader vfr = new VorbisFileReader(args[0], 64);

			System.out.println("Bitstream has " + vfr.getChannels() + " channels, " + vfr.getRate() + "Hz, quality is " + (int)(Math.round((float)vfr.getBitsPerSecond() / 1000)) + "Kbps (on average)");
			System.out.println("Encoded by " + vfr.getVendor());

			String[] comments = vfr.getComments(); 
			if (comments.length > 0) System.out.println("Comments:");
			for (int i = 0; i < comments.length; i++)
			{
				System.out.println("  " + comments[i]);
			}
			
			PortAudioPlayer pap = new PortAudioPlayer(vfr.getChannels(), vfr.getRate(), 1024);

			pap.setSoundSource(vfr);
			pap.play();
			
			System.out.println();
			while (vfr.getState() != VorbisFileReader.State.sAfterEnd)
			{
				try
				{
					Thread.sleep(10);
					
					int min = (int)(vfr.getPlayhead()) / 60;
					int sec = (int)(vfr.getPlayhead()) % 60;
					int sp10 = (int)(vfr.getPlayhead() * 100) % 100;
					
					int lmin = (int)(vfr.getLength()) / 60;
					int lsec = (int)(vfr.getLength()) % 60;
					int lsp10 = (int)(vfr.getLength() * 100) % 100;
					
					System.out.print("\rPlaying the file... [ " + twoDig(min) + ":" + twoDig(sec) + "." + twoDig(sp10) + " / " + twoDig(lmin) + ":" + twoDig(lsec) + "." + twoDig(lsp10) + " ]");
					
				} 
				catch (InterruptedException e)
				{
					e.printStackTrace();
				}
			}
			
			pap.close();
			vfr.close();
			System.gc();
			
			System.out.println("\nBye.");
			System.exit(RESULT_SUCCESS);
		} 
		catch (VorbisFileReader.Error e)
		{
			e.printStackTrace();
			System.exit(RESULT_VORBIS_ERROR);
		}
	}
	
	
	public static void main1(String[] args)
	{
		try
		{
			if (args.length == 0)
			{
				System.err.println("No input files");
				System.exit(RESULT_NO_INPUT_FILE);
				return;
			}

			MixedSounds ms = new MixedSounds();

			for (int argIndex = 0; argIndex < args.length; argIndex++)
			{
				
				System.out.println("Input file: " + args[argIndex]);
	
				VorbisFileReader vfr = new VorbisFileReader(args[argIndex], 64);
	
				System.out.println("Bitstream has " + vfr.getChannels() + " channels, " + vfr.getRate() + "Hz, quality is " + (int)(Math.round((float)vfr.getBitsPerSecond() / 1000)) + "Kbps (on average)");
				System.out.println("Encoded by " + vfr.getVendor());
	
				String[] comments = vfr.getComments(); 
				if (comments.length > 0) System.out.println("Comments:");
				for (int i = 0; i < comments.length; i++)
				{
					System.out.println("  " + comments[i]);
				}
				System.out.println();
				
				ms.addSound(vfr);
			}
			
			ms.rewind(-3);
			
			PortAudioPlayer pap = new PortAudioPlayer(ms.getChannels(), ms.getRate(), 1024);

			pap.setSoundSource(ms);
			pap.play();
			
			while (ms.getState() != VorbisFileReader.State.sAfterEnd)
			{
				try
				{
					Thread.sleep(10);
					
					double playhead = Math.abs(ms.getPlayhead());
					
					int min = (int)(playhead) / 60;
					int sec = (int)(playhead) % 60;
					int sp10 = (int)(playhead * 100) % 100;
					
					int lmin = (int)(ms.getLength()) / 60;
					int lsec = (int)(ms.getLength()) % 60;
					int lsp10 = (int)(ms.getLength() * 100) % 100;
					
					System.out.print("\rPlaying the file... [ " + (ms.getPlayhead() < 0 ? "-" : " ") + twoDig(min) + ":" + twoDig(sec) + "." + twoDig(sp10) + " / " + twoDig(lmin) + ":" + twoDig(lsec) + "." + twoDig(lsp10) + "  ]");
					
				} 
				catch (InterruptedException e)
				{
					e.printStackTrace();
				}
			}
			
			pap.close();
			ms.close();
			System.gc();
			
			System.out.println("\nBye.");
			System.exit(RESULT_SUCCESS);
		} 
		catch (VorbisFileReader.Error e)
		{
			e.printStackTrace();
			System.exit(RESULT_VORBIS_ERROR);
		} 
		catch (MixedSounds.Error e)
		{
			e.printStackTrace();
			System.exit(RESULT_MIXED_ERROR);
		}
	}
}