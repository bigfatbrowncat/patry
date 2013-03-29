package app;

import java.util.ArrayList;

import app.Note.Tone;
import vam.MixedSounds;
import vam.MovedSound;
import vam.SoundSource;
import vam.PortAudioPlayer;
import vam.VorbisFileReader;

public class Application
{
	public static final int RESULT_SUCCESS				= 0;
	public static final int RESULT_NO_INPUT_FILE		= 1;
	public static final int RESULT_SOUNDSOURCE_ERROR			= 2;

	private static String twoDig(int value)
	{
		if (value >= 10) 
			return String.valueOf(value);
		else 
			return "0" + String.valueOf(value);
	}
	
	private static String timeToString(double time)
	{
		double atime = Math.abs(time);
		int min = (int)(atime) / 60;
		int sec = (int)(atime) % 60;
		int sp10 = (int)(atime * 100) % 100;
		
		return (time < 0 ? "-" : " ") + twoDig(min) + ":" + twoDig(sec) + "." + twoDig(sp10);
	}
	
	private static final int INPUT_BUFFER = 256;
	
	public static void main(String[] args)
	{
		System.load("C:\\Users\\imizus\\Projects\\VariMusic\\patry\\bin\\avian-embed.exe");
		
		Meter m = new Meter(9, 4, 120);
		
		try
		{
			NoteSoundPool noteSoundPool = new NoteSoundPool();
			noteSoundPool.addSoundForNote(new Note(Tone.A, 3), new VorbisFileReader("H:\\New\\arp1\\A3.ogg", INPUT_BUFFER));
			noteSoundPool.addSoundForNote(new Note(Tone.A, 4), new VorbisFileReader("H:\\New\\arp1\\A4.ogg", INPUT_BUFFER));
			noteSoundPool.addSoundForNote(new Note(Tone.B, 4), new VorbisFileReader("H:\\New\\arp1\\B4.ogg", INPUT_BUFFER));
			noteSoundPool.addSoundForNote(new Note(Tone.E, 4), new VorbisFileReader("H:\\New\\arp1\\E4.ogg", INPUT_BUFFER));
			noteSoundPool.addSoundForNote(new Note(Tone.G, 4), new VorbisFileReader("H:\\New\\arp1\\G4.ogg", INPUT_BUFFER));
			noteSoundPool.addSoundForNote(new Note(Tone.C, 5), new VorbisFileReader("H:\\New\\arp1\\C5.ogg", INPUT_BUFFER));
			
			RandomNotesShuffler randomNotesShuffler = new RandomNotesShuffler();
			randomNotesShuffler.setNoteSoundPool(noteSoundPool);
			
			randomNotesShuffler.addNotes(new Note[] 
			{  
					new Note(Tone.A, 3),
					new Note(Tone.A, 4),
					new Note(Tone.B, 4),
					new Note(Tone.E, 4),
					new Note(Tone.G, 4),
					new Note(Tone.C, 5)
			});
			
			randomNotesShuffler.setBeatsBetweenNotes(1);
			randomNotesShuffler.setMeter(m);
			
			SoundSource resultSource = randomNotesShuffler.mix(16);
			
			resultSource.rewind(-1);
			
			PortAudioPlayer pap = new PortAudioPlayer(resultSource.getChannels(), resultSource.getRate(), 1024);

			pap.setSoundSource(resultSource);
			
			pap.play();
			
			while (resultSource.getPlayhead() < resultSource.getEndTime())
			{
				try
				{
					Thread.sleep(10);
					
					System.out.print("\rPlaying the file... [ " + timeToString(resultSource.getStartTime()) + " / " + timeToString(resultSource.getPlayhead()) + " / " + timeToString(resultSource.getEndTime()) + "  ]");
					
				} 
				catch (InterruptedException e)
				{
					e.printStackTrace();
				}
			}
			
			pap.close();
			System.gc();
			
			System.out.println("\nBye.");
			System.exit(RESULT_SUCCESS);
		} 
		catch (SoundSource.Error e)
		{
			e.printStackTrace();
			System.exit(RESULT_SOUNDSOURCE_ERROR);
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
			while (vfr.getPlayhead() < vfr.getEndTime())
			{
				try
				{
					Thread.sleep(10);
					
					int min = (int)(vfr.getPlayhead()) / 60;
					int sec = (int)(vfr.getPlayhead()) % 60;
					int sp10 = (int)(vfr.getPlayhead() * 100) % 100;
					
					int lmin = (int)(vfr.getEndTime()) / 60;
					int lsec = (int)(vfr.getEndTime()) % 60;
					int lsp10 = (int)(vfr.getEndTime() * 100) % 100;
					
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
			System.exit(RESULT_SOUNDSOURCE_ERROR);
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
			
			while (ms.getPlayhead() < ms.getEndTime())
			{
				try
				{
					Thread.sleep(10);
					
					double playhead = Math.abs(ms.getPlayhead());
					
					int min = (int)(playhead) / 60;
					int sec = (int)(playhead) % 60;
					int sp10 = (int)(playhead * 100) % 100;
					
					int lmin = (int)(ms.getEndTime()) / 60;
					int lsec = (int)(ms.getEndTime()) % 60;
					int lsp10 = (int)(ms.getEndTime() * 100) % 100;
					
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
		catch (SoundSource.Error e)
		{
			e.printStackTrace();
			System.exit(RESULT_SOUNDSOURCE_ERROR);
		} 
	}
}