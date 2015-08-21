# OfxTemplate

OfxTemplate is our set of simple internal codebases for Openframeworks containing all the libs and examples code to get started in most THÉORIZ cases. It does send OSC, receive OSC, and draw and send some visuals to Syphon/Spout.

MySketch

- A test application to check if OSC, Syphon and Spout work correctly on a machine
- A simple codebase for a new project, with coding guidelines and instructions to follow

MyStateMachineSketch

- A structured codebase including a stateMachine to be able to achieve multiple "MySketch" in one application

## How to start your new project with ofxTemplate

- Download a zip of this repository (Don't clone it)

- Choose your template and copy its directory in `$(OPENFRAMEWORKS_ROOT_FOLDER)/apps/your-project-folder`

- You should have this `$(OPENFRAMEWORKS_ROOT_FOLDER)/apps/your-project-folder/mySketch` or `$(OPENFRAMEWORKS_ROOT_FOLDER)/apps/your-project-folder/myStateMachineSketch`

- Rename your project
	- Rename `mySketch` folder to your app name.

	- Xcode
		- In your app folder, launch the `mySketch.xcodeproj` file (don't rename it !).
		- In the project navigator (left panel of the interface), click on mySketch, wait 2s, and then click again to rename it to your app name. A pop-up may ask you if you want to rename the projects content items. Select all items and click rename. Then Click on disable for the snapshot window.
		- In the toolbar, click on "mySketch Debug" (just next to the "play" button in the top-left corner) and click on "Manage Schemes".
		- In the pop-up window, there is the different schemes. Replace "mySketch" by your app name.
		- Restart Xcode to be sure all changes are taken into account.
		- If you already have an icon, replace the two `icon.icns` file in the repository. To create an icon, use `Icon composer` with a 512x512 png.
	- Visual Studio
		- Open the solution file `mySketch.sln`
		- In Visual Studio, in the solution explorer (the right panel), right click on the solution, select Rename and enter your app name. If the Solution Explorer panel is closed, go to the View menu to open it, or press Ctrl+Alt+L.
		- Just under the solution, right click on the "mySketch" project, select Rename and enter your app name.
		- Save your changes with Save All entry under File menu.
		- Close Visual Studio.
		- Open the `mySketch.vcxproj` with a text editor (notepad, sublime text...), and use Find function to replace all "mySketch" occurences by your app name.
		- Save changes and close the file.
		- Rename `mySketch.vcxproj` file by your app name.
		- Rename `mySketch.vcxproj.filters` file by your app name.
		- Open the `.sln` file with a text editor (notepad, sublime text...), and use Find function to replace all "mySketch" occurences by your app name.
		- Save changes and close the file.
		- Delete the `mySketch.sdf` file (a new file will be created when you will restart Visual Studio)
		- You can now reopen your solution in Visual Studio. 
		- In the Solution Explorer, if the openFrameworksLib project appear in bold, right click on your app project and select "Set as StartUp Project".

- In `src/ofApp.cpp`, replace the APP_NAME macro value by your app name.
- Carefully read and update every `README.md` chapter with the correct data and remove this chapter once you are done.
- Now coding can start ! Every instructions and explanations are in the code :)

## Use

### MySketch

#### First Group

My Int : An awesome slider where you can change ... an int !

#### Second Group

My Bool : The best feature ! Transform your cube into a hulkish pale color !

#### Third Group

My Vector : This is madness, you can change x y z coordinates of a vector, making your cube moving !

#### Mouse clics

Use left clic and right clic to move and zoom on your cube, this does affect the 3D camera, not the real size of the cube.

### MyStateMachineSketch

#### First sketch?

##### First Group

TODO !

##### Second Group

#### Second sketch?

##### First Group

TODO !

##### Second Group

## Dev env

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
- [ofxStateMachine](https://github.com/Lyptik/ofxStateMachine) (For myStateMachineSketch only)

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
