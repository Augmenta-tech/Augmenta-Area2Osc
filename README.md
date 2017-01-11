# augmenta-area2osc

augmenta-area2osc allows you to easly create polygons which will refer to detection zone. Osc messages will be sent for every person entering or leaving the zone.

## Use

#### Polygons Group

Delete all polygons : Push button which delete all the polygons

Delete last polygon : Push button which delete the last polygon created or the one you're currently creating

#### OSC Group

Send all event : If the button is ticked all person coming in and person coming out event will be sent.If the button is not ticked only the first person coming in will send and the last person coming out.

Anti bounce : Slider which represent the time in milliseconds, that will be waited once a person leaved a polygon to remove it definetly, (In the goal to avoid flickering when the person is on the edge of a polygon).

Plus one to dollar : If a $ symbol in an osc message string, it will automatically be replaced by the number of people in the corresponding polygon. If this box is checked, it will add "1" to the number of people so that 0 becomes 1, 1 becomes 2, etc. Useful for Resolume for example.

Mute Osc : Mute the osc messages going out.

#### Mouse clics

Left click : to create a new point or polygon.

Left click : inside a polygon to select it /outside a polygon to deselect it.

Left click : on a selected polygon point to move it.

Right click : to delete the last point created.

Mousewheel click + drag : to move the view.

Mousewheel up/down : to zoom in/out of the view. 

#### Keyboard

Ctrl+s / Cmd+s : To save settings and the currents polygons.

Ctrl+l / Cmd+l : To load last saved settings and polygons.

Ctrl+z : To delete the last polygon created or the current polygon.

R / r : To delete all the polygons you have created.

Del / BackSpace : To delete the selected polygon.

(-) : To zoom in (only for a better appreciation on the screen).

(+) : To zoom out (only for a better appreciation on the screen).

Arrows : move a polygon when selected / move the view when not.

#### Note 

Your settings are saved when the app quits and are loaded at startup. (autosave feature).

The dimensions of the application window will be the same as the ones sent by Augmenta.

You can change the osc messages of a polygon in the preferences.xml file.

Any $ symbol in an osc message will be replaced by the number of people in the corresponding polygon.

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