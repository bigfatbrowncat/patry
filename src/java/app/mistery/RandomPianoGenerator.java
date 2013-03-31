package app.mistery;

import vam.MovedSound;
import vam.SoundSource;
import app.Meter;
import app.Note;
import app.NoteSoundPool;
import app.Note.Key;

public class RandomPianoGenerator
{
	private int bufferSize;
	private Meter meter;
	private int notesPerBeat;
	private RandomNotesShuffler randomNotesShufflerA;
	
	public RandomPianoGenerator(NoteSoundPool noteSoundPool, Note[] notes, Meter meter, int notesPerBeat, int bufferSize) throws SoundSource.Error
	{
		this.bufferSize = bufferSize;
		this.meter = meter;
		this.notesPerBeat = notesPerBeat;
		
		randomNotesShufflerA = new RandomNotesShuffler(bufferSize);
		randomNotesShufflerA.setNoteSoundPool(noteSoundPool);
		
		randomNotesShufflerA.addNotes(notes);
		
		randomNotesShufflerA.setBeatsBetweenNotes(1.0 / notesPerBeat);
		randomNotesShufflerA.setMeter(meter);
	}
	
	public SoundSource mix(double beats_delay, double bars) throws SoundSource.Error
	{
		MovedSound ms = new MovedSound(bufferSize);
		ms.setDelay(meter.beatsToTimeSpan(beats_delay));
		ms.setSound(randomNotesShufflerA.mix((int)(bars * meter.getNumerator() * notesPerBeat)));
		return ms;
	}
}
