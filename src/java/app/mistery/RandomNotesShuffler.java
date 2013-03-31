package app.mistery;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Date;
import java.util.HashSet;
import java.util.List;
import java.util.Random;

import app.Meter;
import app.Note;
import app.NoteSoundPool;

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
	private int bufferSize;
	
	public RandomNotesShuffler(int bufferSize)
	{
		this.bufferSize = bufferSize;
		randomSeed = 5;//System.nanoTime();
		notes = new HashSet<Note>();
	}
	
	public void setMeter(Meter value) { meter = value; }
	public void setBeatsBetweenNotes(double value) { beatsBetweenNotes = value; }
	public void setNoteSoundPool(NoteSoundPool value) { noteSoundPool = value; }
	
	public void addNotes(Note[] notes)
	{
		ArrayList<Note> notesList = new ArrayList<Note>();
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
		
		MixedSounds res = new MixedSounds(bufferSize);
		
		double time = 0;

		if (notes.size() == 0)
		{
			return null;
		}

		Note[] notesArray = notes.toArray(new Note[] {});
		Arrays.sort(notesArray);
		
		List<Integer> indices = new ArrayList<Integer>();
		for (int i = 0; i < number; i++)
		{
			MovedSound ms = new MovedSound(bufferSize);
			Note nextNote = null;
			
			// Generating the next note
			boolean ok;
			int nextIndex;
			do
			{
				ok = true;
				
				
				if (i == 0)
				{
					// The first note is the lowest one
					nextIndex = 0;
				}
				else if (i == 1)
				{
					// That should be 1 or 2
					nextIndex = rnd.nextInt(2) + 1;
				}
				else if (i == number - 1) 
				{
					// The last note is in the middle
					nextIndex = 3;
				}
				else if (i == number - 2)
				{
					// That should be 1 or 2 (greater than 0 and lower than the previous)
					nextIndex = rnd.nextInt(2) + 1;
				}
				else
				{
					// A note in the middle should be between 1 and length. The lowest isn't allowed there
					nextIndex = rnd.nextInt(notesArray.length - 1) + 1;
				}

				
				if (indices.size() >= 1 && nextIndex == indices.get(indices.size() - 1))
				{
					// The current note shouldn't be equal to the previous one
					ok = false;
				}
				
				if (indices.size() >= 2 && nextIndex == indices.get(indices.size() - 2))
				{
					if (rnd.nextInt(20) > 1)
					{
						// If the next note is equal to the one that is 2 notes before,
						// the result is not ok with 6/7 chance
						ok = false;
					}
				}
			
			}
			while (!ok);
			indices.add(nextIndex);
			
			nextNote = notesArray[indices.get(indices.size() - 1)];

			ms.setSound(noteSoundPool.getSoundForNote(nextNote));
			ms.setDelay(time);
			res.addSound(ms);
			
			time += meter.beatsToTimeSpan(beatsBetweenNotes);
		}
		
		return res;
	}
}
