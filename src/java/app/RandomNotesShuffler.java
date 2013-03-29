package app;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.HashSet;
import java.util.Random;

import vam.MixedSounds;
import vam.MovedSound;
import vam.SoundSource;

public class RandomNotesShuffler
{
	private long randomSeed;

	private Meter meter;
	private double beatsBetweenNotes;
	private NoteSoundPool noteSoundPool;
	private HashSet<Note> notes;
	
	public RandomNotesShuffler()
	{
		randomSeed = (new Date()).getTime();
		notes = new HashSet<Note>();
	}
	
	public void setMeter(Meter value) { this.meter = meter; }
	public void setBeatsBetweenNotes(double value) { this.beatsBetweenNotes = value; }
	public void setNoteSoundPool(NoteSoundPool value) { this.noteSoundPool = value; }
	
	public void addNotes(Note[] notes)
	{
		ArrayList<Note> notesList = new ArrayList<>();
		for (int i = 0; i < notes.length; i++)
		{
			notesList.add(notes[i]);
		}
		
		this.notes.addAll(notesList);
	}

	public void addNotes(Collection<? extends Note> notes)
	{
		this.notes.addAll(notes);
	}
	
	public SoundSource mix(int number) throws MovedSound.Error, MixedSounds.Error
	{
		Random rnd = new Random(randomSeed);
		
		MixedSounds res = new MixedSounds();
		
		double time = 0;
		Note prevNote = null;

		if (notes.size() == 0)
		{
			return null;
		}

		Note[] notesArray = notes.toArray(new Note[] {});
		
		for (int i = 0; i < number; i++)
		{
			MovedSound ms = new MovedSound();
			
			// Generating the next note
			Note nextNote = null;
			while (nextNote == prevNote)
			{
				int nextIndex = rnd.nextInt() % notesArray.length;
				System.out.print(nextIndex + " ");
				nextNote = notesArray[nextIndex];
			}
			
			ms.setSound(noteSoundPool.getSoundForNote(nextNote));
			ms.setDelay(time);
			res.addSound(ms);
			
			prevNote = nextNote;
			time += beatsBetweenNotes * 0.5;	// TODO Use Meter to calculate time
		}
		
		return res;
	}
}
