#ifndef OF_APP_H
#define OF_APP_H

#include "AreaPolygon.h"

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"
#include "ofxAugmenta.h"



#ifdef WIN32
#include "ofxSpout2.h"
#elif MAC_OS_X_VERSION_10_6
#include "ofxSyphon.h"
#endif

class ofApp : public ofBaseApp {

public:
    
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	    
    void saveSettings();
    void loadSettings();
	void savePreferences();
	void loadPreferences();

    
private:
    
    void init();
    void reset();
    
	void setupOSC();
    void setupGUI();
    
    void receiveOSC();
    void sendOSC();
    
    // Only draw final output visuals - no GUI, no debug visuals, only content that can be seen by spectator
    void drawVisuals();
	void drawAreaPolygons();
	void drawAugmentaPeople();
    // Draw the interface of your app : visuals, GUI, debug content...
    void drawInterface();
    // Minimalist interface with black screen and some information
    void drawHiddenInterface();
    // Send FBO to Spout/Syphon
    void sendVisuals();
    
    // Boolean indicating if logs must be saved to a file. If set to true, will not see log messages on the console, only in the file
    bool m_bLogToFile;
    // Boolean indicating if visuals must be drawn or not in app
    bool m_bHideInterface;
    // Keyboard modifier key currently pressed
    int m_iModifierKey;
    
    // Fbo used to render visuals
    ofFbo m_fbo;
    int m_iFboWidth;
    int m_iFboHeight;

    
    // OSC
    ofxOscReceiver m_oscReceiver;
    ofxOscSender m_oscSender;
    string m_sOscPortDisplayMessage;   
    int m_iOscReceiverPort;
    int m_iOscSenderPort;
    string m_sOscSenderHost;
    string m_sReceiverOscDisplay;

	//Augmenta
	Augmenta::Receiver AugmentaReceiver;
	vector<Augmenta::Person*> people;

	//Parameters AreaEditor
	bool m_bEditMode;
	bool m_bSelectMode;
	int m_iNumberOfAreaPolygons;
	int m_iIndicePolygonSelected;
	vector<AreaPolygon> m_vAreaPolygonsVector;

    // Gui panel
    ofxPanel m_gui;
    // Gui content ----------------
    ofxLabel m_sFramerate;
	ofxLabel m_sNumberOfAreaPolygons;
    ofxButton m_bResetSettings;     // Boolean indicating if variables must be reset to their default values
    // Parameters group to organize your parameters
    ofParameterGroup m_guiFirstGroup, m_guiSecondGroup, m_guiThirdGroup;
    
	// Gui parameters inside m_guiFirstGroup
    ofxFloatSlider m_fPointRadius;
	ofxIntSlider m_iLinesWidthSlider;

    // Gui parameters inside m_guiSecondGroup
    ofxToggle m_bRedondanteMode;
	ofxIntSlider m_iRadiusClosePolyZone;

	// Gui parameters inside m_ThirdGroup
    ofxVec3Slider m_vMyVec;
    // --------- End of Gui content	
    
    #ifdef WIN32
    ofxSpout2 m_spoutSender;
    #elif MAC_OS_X_VERSION_10_6
    ofxSyphonServer m_syphonServer;
    #endif
		
};

#endif // OF_APP_H
