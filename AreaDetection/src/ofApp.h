#ifndef OF_APP_H
#define OF_APP_H

#include "AreaPolygon.h"
#include "AugmentaScene.h"

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
    
    void sendOSC();
    
    // Only draw final output visuals - no GUI, no debug visuals, only content that can be seen by spectator
    void drawVisuals();
	void drawAreaPolygons();
	void drawAugmentaPeople();
	void fboSizeHaveChanged(int a_iNewWidth, int a_iNewHeight);

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
	ofPoint transformMouseCoord(int x, int y);

    
    // Fbo used to render visuals
    ofFbo m_fbo;
	ofFbo m_oFboSent;
    int m_iFboWidth;
    int m_iFboHeight;
    string m_sSendFboResolution;
    string m_sScreenResolution;

	bool isInsideAPolygon(ofVec2f a_oPoint);
    void deleteLastPolygon();
	void deleteAllPolygon();

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
	vector<Augmenta::Person*> m_oPeople;
	string m_sAugmentaOscDiplay;
	Augmenta::Scene* m_oActualScene;

	//Parameters AreaEditor
	int m_iNextFreeId;
	bool m_bEditMode;
	bool m_bSelectMode;
	int m_iNumberOfAreaPolygons;
	int m_iIndicePolygonSelected;
	vector<AreaPolygon> m_vAreaPolygonsVector;
	ofVec2f m_oOldMousePosition;
	int m_iRadiusClosePolyZone;
	float m_fPointRadius;
	int m_iLinesWidthSlider;
	int m_iWidthRender;
	int m_iHeightRender;


    // Gui panel
    ofxPanel m_gui;

    // Gui content ----------------
	ofxLabel m_sNumberOfAreaPolygons;
    ofxButton m_bResetSettings;     // Boolean indicating if variables must be reset to their default values
    
	// Parameters group to organize your parameters
	ofParameterGroup m_guiFirstGroup, m_guiSecondGroup, m_guiThirdGroup;

    // Gui parameters inside m_guiFirstGroup
    ofxToggle m_bRedondanteMode;
	ofxIntSlider m_iAntiBounce;
	ofxFloatSlider m_fZoomCoef;

	// Gui parameters inside m_SecondGroup
	ofxToggle m_oToggleClearAll;
	ofxToggle m_oToggleDeleteLastPoly;

	// Gui parameters inside m_ThirdGroup
	ofxToggle m_bSendFbo;
    ofxLabel m_sOscOutGUI;
    ofxLabel m_sOscInGUI;

    // --------- End of Gui content	
	
    #ifdef WIN32
    ofxSpout2 m_spoutSender;
    #elif MAC_OS_X_VERSION_10_6
    ofxSyphonServer m_syphonServer;
    #endif
		
};

#endif // OF_APP_H
