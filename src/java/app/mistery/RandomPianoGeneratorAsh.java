package app.mistery;

import vam.SoundSource.Error;
import app.Meter;
import app.Note;
import app.NoteSoundPool;
import app.Note.Key;

public class RandomPianoGeneratorAsh extends RandomPianoGenerator
{

	public RandomPianoGeneratorAsh(NoteSoundPool noteSoundPool,
			Meter meter, int notesPerBeat, int bufferSize) throws Error
	{
		super(noteSoundPool, new Note[] 
		{  
			new Note(Key.F, 3),
			new Note(Key.Ash, 3),
			new Note(Key.Ash, 4),
			new Note(Key.Csh, 4),
			new Note(Key.F, 4),
			new Note(Key.G, 4),
			//new Note(Key.Gsh, 4),
		}, meter, notesPerBeat, bufferSize);
		
	}

}
