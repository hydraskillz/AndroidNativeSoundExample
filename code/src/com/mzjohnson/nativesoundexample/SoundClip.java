package com.mzjohnson.nativesoundexample;

import android.media.SoundPool;

public class SoundClip
{
	private final SoundPool mSoundPool;
	private final int mSoundId;
	
	public SoundClip( SoundPool pool, int id )
	{
		mSoundPool = pool;
		mSoundId = id;
	}
	
	public void destroy()
	{
		mSoundPool.unload( mSoundId );
	}
	
	public int play( float volume, float pitch, boolean loop )
	{
		// streamId will be 0 if the sound fails to play
		int streamId = mSoundPool.play( mSoundId, volume, volume, 1, loop ? -1 : 0, pitch );
		return streamId;
	}
}
