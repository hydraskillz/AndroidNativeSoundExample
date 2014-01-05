package com.mzjohnson.nativesoundexample;

import java.io.IOException;

import android.app.NativeActivity;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Bundle;
import android.util.Log;

public class MainActivity extends NativeActivity
{	
	private static final String TAG = "NativeSound";
	private SoundPool mSoundPool;
	private AssetManager mAssetManager;
	
	@Override
	protected void onCreate( Bundle savedInstanceState )
	{
		super.onCreate( savedInstanceState );
		
		// Get the asset manager
		mAssetManager = getAssets();
		
		// Create a new SoundPool
		mSoundPool = new SoundPool( 16, AudioManager.STREAM_MUSIC, 100 );
		setVolumeControlStream( AudioManager.STREAM_MUSIC );
	}
	
	// Load our sound clip
	public SoundClip newSoundClip( String filename )
	{
		SoundClip clip = null;
		try
		{
			AssetFileDescriptor descriptor = mAssetManager.openFd( filename );
			clip = new SoundClip( mSoundPool, mSoundPool.load( descriptor, 1 ) );
			descriptor.close();
		}
		catch ( IOException ex )
		{
			Log.e( TAG, "Failed to open Sound Asset " + filename );
		}
		return clip;
	}
}
