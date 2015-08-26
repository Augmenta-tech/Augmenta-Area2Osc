# augmenta-area2osc

augmenta-area2osc allows you to easly create polygons which will refer to detection zone. Osc messages will be sent for every person entering or leaving the zone.

## Use

#### First Group

My Int : An awesome slider where you can change ... an int !

#### Second Group

My Bool : The best feature ! Transform your cube into a hulkish pale color !

#### Third Group

My Vector : This is madness, you can change x y z coordinates of a vector, making your cube moving !

#### Mouse clics

Use left clic and right clic to move and zoom on your cube, this does affect the 3D camera, not the real size of the cube.

### System

Windows 8.1

OSX 10.10

### Libs

OpenFrameworks 0.8.4 

#### Addons

Already in OpenFrameworks

- ofxGui
- ofxOsc
- ofxXmlSettings

External Addons :

- [ofxSyphon](https://github.com/astellato/ofxSyphon) (Mac OSX >= v10.6)
- [ofxSpout2](https://github.com/Kj1/ofxSpout2) (Windows with NVIDIA DirectX/Opengl interop extension)
- [ofxAugmenta](https://github.com/Theoriz/ofxAugmenta)

## Setup

### OSX & Linux

Run ./installAddons.sh in scripts/ folder to clone all addons needed. (Already present addons won't be reinstalled)

### Windows

Please refer to the external addons section above and use the links to get all the addons.

## Dev guidelines

### Workflow

We are using git with the master/experimental paradigm

All work in progress change should be pushed to experimental, and then merged to master once thorough testing is done.

[About this workflow](https://www.atlassian.com/en/git/workflows#!workflow-feature-branch)

[A propos de ce workflow](https://www.atlassian.com/fr/git/workflows#!workflow-feature-branch)

### Git practices

Use prefix in your git commit comment

- FIX : for a fix
- minor : for a change that is not impacting much stuff
- Feature : for a major feature

### Coding guidelines

Here is a typical file called videopipe.h showing all the guidelines.

	#ifndef VIDEOPIPE_H
	#define VIDEOPIPE_H

	class VideoPipe
	{

	public:
		videoPipe();
		virtual ~videoPipe();

		// Method
		void run();
		int setup();
		int saveImage(); // Short comment
		int recordVideo();

		// Setter
		inline void setRecording(bool a_bIsRecording) {m_bIsRecording = a_bIsRecording;}

		// Getter
		inline IPalgorithms* getIPalgo() {return &m_oIpAlgo;}
		
		// Members
		int m_iScaleFactor;
		bool m_bIsRecording;
	};

	#endif // VIDEOPIPE_H

### Adding an addon

Update the following files

- `README.md` : add the name of the addon and a link to its repository to make it easy for users to install your project
- `addons.make` : add the name of the addon
- `scripts/installAddons.sh` : Update and append the following code to the script. (replace all ofxRay occurences by your addon)

	```
	# Install ofxRay
	ADDONS=ofxRay
	cd $SCRIPT_PATH/../../../addons/ &&
	if [ ! -d $ADDONS ]; then
	        git clone https://github.com/elliotwoods/ofxRay.git &&
	        cd $ADDONS &&
	        git checkout master
	fi
	```

### Adding an asset

All the assets must be placed in the data folder next to the binary.

#### OSX

On OSX, the assets are loaded form the `Contents/Resources/` folder inside the .app bundle, instead of `data/ folder next to the .app file.
Assets must still be in the data folder, but should be copied at each compilation inside the .app bundle.

Setup a build copy phase

- In Xcode, go to the targets settings pane.
- In `Build Phases` tab, open the second `Copy Files` foldable menu (`Destination` parameter should be set to `Resources`).
- Add assets here with `+` button or by dragging & dropping them.

## Performance

60fps on a 2014 macmini in Debug and Release for both app

Note : On Windows, when not having a NVIDIA board using DirectX/Opengl interop extension, performances might be very poor when using Spout.
