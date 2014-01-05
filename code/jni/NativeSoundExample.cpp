/*
 * Author      : Matthew Johnson
 * Date        : 5/Jan/2014
 * Description :
 *   A bare bones example for playing sound using native activity.
 */
#include <jni.h>
#include <android_native_app_glue.h>

// Global handle to the JavaVM
JavaVM* javaVM;

// Our game engine
struct Engine
{
    struct android_app* app;

    int animating;
    class SoundClip* sfx;
};
Engine theEngine;

// This class wraps SoundClip.java and calls its methods
class SoundClip
{
public:
	SoundClip( JNIEnv* env, jclass soundClipClass, jobject soundClipObj )
		:  mSoundClipClass( soundClipClass )
		, mSoundClipObj( soundClipObj )
	{
		// Cache the method id for later use
		// the method signature in Java int play( float, float, boolean )
		// which maps to (FFZ)I
		// see http://docs.oracle.com/javase/1.5.0/docs/guide/jni/spec/types.html
		mMethod_Play = env->GetMethodID( mSoundClipClass, "play", "(FFZ)I" );
		// void destroy()
		mMethod_Destroy = env->GetMethodID( mSoundClipClass, "destroy", "()V" );
	}

	~SoundClip()
	{
		// Set up Java environment
		JNIEnv* env;
		javaVM->AttachCurrentThread( &env, 0 );

		// Call our void method
		env->CallVoidMethod( mSoundClipObj, mMethod_Destroy );

		// Delete global references to the Java class
		env->DeleteGlobalRef( (jobject) mSoundClipClass );
		env->DeleteGlobalRef( mSoundClipObj );

		// Detach from the VM
		javaVM->DetachCurrentThread();
	}

	void Play( float volume=1.0f, float pitch=1.0f, bool loop=false )
	{
		// Set up Java environment
		JNIEnv* env;
		javaVM->AttachCurrentThread( &env, 0 );

		// Call our int method
		env->CallIntMethod( mSoundClipObj, mMethod_Play, volume, pitch, loop );

		// Detach from the VM
		javaVM->DetachCurrentThread();
	}

private:
	jclass mSoundClipClass;
	jobject mSoundClipObj;
	jmethodID mMethod_Play;
	jmethodID mMethod_Destroy;
};

// This function calls the newSoundClip( String ) method in our activity class.
// It then creates a native SoundClip class from the Java SoundClip class.
SoundClip* CreateNewSoundClip( const char* filename )
{
	JNIEnv *env;
	SoundClip* clip = NULL;

	javaVM->AttachCurrentThread( &env, NULL );

	// Get the activity Java class
	jclass activityClass = env->GetObjectClass( theEngine.app->activity->clazz );
	jobject activityObj = theEngine.app->activity->clazz;

	// Get the method SoundClip newSoundClip( String )
	jmethodID method = env->GetMethodID( activityClass, "newSoundClip", "(Ljava/lang/String;)Lcom/mzjohnson/nativesoundexample/SoundClip;" );
	// Convert the const char* into a jstring
	jstring jfilename = env->NewStringUTF( filename );
	// Call the method
	jobject soundClipObj = env->CallObjectMethod( activityObj, method, jfilename );

	// Create global a reference to the class
	jclass cls = env->GetObjectClass( soundClipObj );
	jclass soundClipClsRef = (jclass) env->NewGlobalRef( cls );
	jobject soundClipObjRef = env->NewGlobalRef( soundClipObj );

	// Create a new SoundClip
	clip = new SoundClip( env, soundClipClsRef, soundClipObjRef );

	// Detach from VM
	javaVM->DetachCurrentThread();

	return clip;
}

// Event handlers
static void handle_cmd(struct android_app* app, int32_t cmd);
static int32_t handle_input(struct android_app* app, AInputEvent* event);

// A basic entry point taken from the sample code (minus the renderer)
void android_main( struct android_app* app )
{
	// Make sure glue isn't stripped.
	app_dummy();

	memset (&theEngine, 0, sizeof(Engine) );
	app->userData = &theEngine;
	app->onAppCmd = handle_cmd;
	app->onInputEvent = handle_input;
	theEngine.app = app;

	javaVM = app->activity->vm;

	// Create a new SoundClip
	// The sound file is relative to the assets folder
	theEngine.sfx = CreateNewSoundClip( "super_mario_jump.wav" );

	while (1)
	{
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((ident=ALooper_pollAll(theEngine.animating ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
		{
			// Process this event.
			if (source != NULL)
			{
				source->process(app, source);
			}
			// Check if we are exiting.
			if (app->destroyRequested != 0)
			{
				return;
	        }
	    }
	}
}

void handle_cmd(struct android_app* app, int32_t cmd) {
    switch (cmd)
    {
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if ( app->window != NULL)
            {
            	theEngine.animating = 1;
            }
            break;
        case APP_CMD_GAINED_FOCUS:
        	theEngine.animating = 1;
            break;
        case APP_CMD_LOST_FOCUS:
        	theEngine.animating = 0;
            break;
    }
}

int32_t handle_input(struct android_app* app, AInputEvent* event)
{
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
    {
    	const int action = AMotionEvent_getAction( event );
		switch ( action & AMOTION_EVENT_ACTION_MASK )
		{
			case AMOTION_EVENT_ACTION_DOWN:
			{
				// Play our sound on touch
				if ( theEngine.sfx )
				{
					theEngine.sfx->Play();
				}
				return 1;
			}
		}
    }
    return 0;
}
