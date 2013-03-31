package app.mistery;

import vam.SoundSource.Error;
import app.Meter;
import app.Note;
import app.NoteSoundPool;
import app.Note.Key;

public class RandomPianoGeneratorA extends RandomPianoGenerator
{

	public RandomPianoGeneratorA(NoteSoundPool noteSoundPool,
			Meter meter, int notesPerBeat, int bufferSize) throws Error
	{
		super(noteSoundPool, new Note[] 
		{  
			new Note(Key.A, 3),
			new Note(Key.A, 4),
			new Note(Key.B, 4),
			new Note(Key.E, 4),
			new Note(Key.G, 4),
			new Note(Key.C, 5)
		}, meter, notesPerBeat, bufferSize);
		
	}

}
