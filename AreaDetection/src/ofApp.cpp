#include "ofApp.h"

#define APP_NAME "Augmenta area2OSC"


//_______________________________________________________________
//_____________________________SETUP_____________________________
//_______________________________________________________________

//--------------------------------------------------------------
void ofApp::setup(){

	// If we are building a debug binary, all the outputs will be shown
#ifdef DEBUG
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetWindowTitle(APP_NAME"Debug");
#else
	ofSetLogLevel(OF_LOG_NOTICE);
	ofSetWindowTitle(APP_NAME);
#endif

	// Limit framerate to 60fps
	ofSetFrameRate(60);

	// Init function is used to set default variables that can be changed.
	// For example, GUI variables or preferences.xml variables.
	init();

	// Important : call those function AFTER init,
	// because init() will define all default values
	m_iNextFreeId = 0;
    load("autosave.xml");
    setupGUI();
    setupOSC();
    
    //Augmenta
    AugmentaReceiver.connect(m_iOscReceiverPorts[0]);
    m_oPeople = AugmentaReceiver.getPeople();
    m_oActualScene = AugmentaReceiver.getScene();

	if ((m_oActualScene->width != 0 && m_oActualScene->height != 0) && m_bAutoSize){
		m_iFboWidth = m_oActualScene->width;
		m_iFboHeight = m_oActualScene->height;
	}

    /*
     Visuals will be drawn in a FBO for several reasons :
     - We need a texture reference to send our visuals with Syphon or Spout outside the app
     - You can draw visuals larger than your app or screen resolution
     - FBOs are easy to draw wherever you want in screen space to build your UI
     
     */
    
    // Allocating a FBO of the size defined in preferences.xml (default : window size), can be any size
	m_fbo.allocate(m_iFboWidth,m_iFboHeight,GL_RGBA);

    #if MAC_OS_X_VERSION_10_6
    // Setup Syphon output
    m_syphonServer.setName(APP_NAME);
    #endif
    
    m_iCurrentPointMoved = -1; // No point moving
    
    // Hardcode window size
    ofSetWindowShape(800,600);
}

//--------------------------------------------------------------
void ofApp::init(){
   
	//--------------------------------------------
	//In case of reset
	if (m_vAreaPolygonsVector.size() > 0){
		if (!m_vAreaPolygonsVector[m_vAreaPolygonsVector.size() - 1].isCompleted()){
			m_vAreaPolygonsVector.pop_back();
		}
	}
	for (int i = 0; i < m_vAreaPolygonsVector.size(); ++i){
		m_vAreaPolygonsVector[i].hasBeenSelected(false);
	}

    //--------------------------------------------
    // Change default values here.
    //--------------------------------------------

    // App default values (preferences.xml)
    m_sSettingsPath = "settings.xml";
    m_bHideInterface = false;
    m_bLogToFile = false;
    m_iFboWidth = 1024;
    m_iFboHeight = 768;
    m_iXMLFboWidth = m_iFboWidth;
    m_iXMLFboHeight = m_iFboHeight;
    m_iOscReceiverPorts.push_back(12000);
    m_iOscSenderPorts.push_back(7000);
    m_sOscSenderHosts.push_back("127.0.0.1");
	m_iIndicePolygonSelected = -1;
    m_fPointRadius = 20;
	m_iLinesWidthSlider = 2;
	m_bRedondanteMode = true;
	m_oToggleClearAll = false;
	m_oToggleDeleteLastPoly = false;
	m_bEditMode = false;
	m_bSelectMode = false;
	m_iNumberOfAreaPolygons = m_vAreaPolygonsVector.size();
	m_iRadiusClosePolyZone = 15;
	m_oOldMousePosition = ofVec2f(0,0);
	m_iAntiBounce = 100;
	m_fZoomCoef = 1.0;
	m_bSendFbo = false;
    m_bAutoSize = false;
	m_sScreenResolution = ofToString(m_iWidthRender) +" x "+ ofToString(m_iHeightRender);
	m_sSendFboResolution = ofToString(m_iFboWidth) + " x " + ofToString(m_iFboHeight);
    m_pFboOffset = *new ofPoint(0,0);
	
}

//--------------------------------------------------------------
void ofApp::setupGUI(){

	// Add listeners before setting up so the initial values are correct
	// Listeners can be used to call a function when a UI element has changed.
	// Don't forget to call ofRemoveListener before deleting any instance that is listening to an event, to prevent crashes. Here, we will call it in exit() method.
	m_bResetSettings.addListener(this, &ofApp::reset);
    m_bResetFboView.addListener(this, &ofApp::resetFboView);

	// Setup GUI panel
	m_gui.setup();
	m_gui.setName("GUI");

	// Add content to GUI panel
	//m_gui.add(m_sScreenResolution.setup("Window res ", m_sScreenResolution));
	//m_gui.add(m_sSendFboResolution.setup("Fbo res ", m_sSendFboResolution));
	m_gui.add((m_fZoomCoef.setup("Zoom", 1, 0.3, 2)));
    m_gui.add(m_bResetFboView.setup("Reset View",false));
	m_gui.add(m_bResetSettings.setup("Reset Settings", m_bResetSettings));
    
    // Add save/load listener
    ofAddListener(m_gui.savePressedE,this, &ofApp::onSavePressed);
    ofAddListener(m_gui.loadPressedE,this, &ofApp::onLoadPressed);

	// guiFirstGroup parameters ---------------------------
	string sFirstGroupName = "Polygons";
    m_gui.add(m_sNumberOfAreaPolygons.setup("Number of polygons", m_sNumberOfAreaPolygons));
	m_guiFirstGroup.setName(sFirstGroupName);     // Name the group of parameters (important if you want to apply color to your GUI)
	m_guiFirstGroup.add((m_oToggleClearAll.setup("Delete all polygons", m_oToggleClearAll))->getParameter());
	m_guiFirstGroup.add((m_oToggleDeleteLastPoly.setup("Delete last polygon", m_oToggleDeleteLastPoly))->getParameter());
	m_gui.add(m_guiFirstGroup);     // When all parameters of the group are set up, add the group to the gui panel.

	// guiSecondGroup parameters ---------------------------
	string sSecondGroupName = "OSC";
	m_guiSecondGroup.setName(sSecondGroupName);
	m_guiSecondGroup.add((m_bRedondanteMode.setup("Send all event", m_bRedondanteMode))->getParameter());
	m_guiSecondGroup.add(m_iAntiBounce.setup("Anti bounce ms",100,1,400)->getParameter());
	m_gui.add(m_guiSecondGroup);

	#ifdef WIN32
	string sThirdGroupName = "SPOUT";
	#elif __APPLE__
	string sThirdGroupName = "SYPHON";
	#else
	string sThirdGroupName = "FBO";
	#endif
	

	m_guiThirdGroup.setName(sThirdGroupName);
	m_guiThirdGroup.add((m_bSendFbo.setup("on/off", m_bSendFbo))->getParameter());
	
	m_gui.add(m_guiThirdGroup);

    // 4th group
    string sFourthGroupName = "Augmenta";
    m_guiFourthGroup.setName(sFourthGroupName);
    m_guiFourthGroup.add((m_bAutoSize.setup("Auto resize", m_bAutoSize))->getParameter());
    m_gui.add(m_guiFourthGroup);
    
	// You can add colors to your GUI groups to identify them easily
	// Example of beautiful colors you can use : salmon, orange, darkSeaGreen, teal, cornflowerBlue...
	m_gui.getGroup(sFirstGroupName).setHeaderBackgroundColor(ofColor::salmon);    // Parameter group must be get with its name defined in setupGUI()
	m_gui.getGroup(sFirstGroupName).setBorderColor(ofColor::salmon);
	m_gui.getGroup(sSecondGroupName).setHeaderBackgroundColor(ofColor::orange);
	m_gui.getGroup(sSecondGroupName).setBorderColor(ofColor::orange);
	m_gui.getGroup(sThirdGroupName).setHeaderBackgroundColor(ofColor::cornflowerBlue);
	m_gui.getGroup(sThirdGroupName).setBorderColor(ofColor::cornflowerBlue);
    m_gui.getGroup(sFourthGroupName).setHeaderBackgroundColor(ofColor::darkOrchid);
    m_gui.getGroup(sFourthGroupName).setBorderColor(ofColor::darkOrchid);

	// Load autosave settings
	if (ofFile::doesFileExist("autosave.xml")){
        load("autosave.xml");
	}
}

//--------------------------------------------------------------
void ofApp::setupOSC(){
    
    m_oscReceivers.clear();
    m_oscSenders.clear();
    
    // Senders
    for (int i=0 ; i < m_iOscSenderPorts.size() ; i++){
        ofxOscSender sender;
        sender.setup(m_sOscSenderHosts[i],  m_iOscSenderPorts[i]);
        m_oscSenders.push_back(sender);
        ofLogNotice() << "Sender added : " <<  m_sOscSenderHosts.back() << ":" << m_iOscSenderPorts.back();
    }
    
    // Receivers
    /* Don't add receivers for this app : augmenta is handling the port
    for (int i=0 ; i < m_iOscReceiverPorts.size() ; i++){
        ofxOscReceiver receiver;
        receiver.setup(m_iOscReceiverPorts[i]);
        m_oscReceivers.push_back(receiver);
        ofLogNotice() << "Receiver added : " << m_iOscReceiverPorts.back();
    }
     */
}

//--------------------------------------------------------------
void ofApp::reset(){
    init();
}

//_______________________________________________________________
//_____________________________UPDATE____________________________
//_______________________________________________________________

//--------------------------------------------------------------
void ofApp::update(){

	m_sScreenResolution = ofToString(ofGetWindowWidth()) + " x " + ofToString(ofGetWindowHeight());
	m_sSendFboResolution = ofToString(m_iFboWidth) + " x " + ofToString(m_iFboHeight);

	if (m_oToggleDeleteLastPoly){
		deleteLastPolygon();
		m_oToggleDeleteLastPoly = false;
	}
	if (m_oToggleClearAll){
		deleteAllPolygon();
		m_oToggleClearAll = false;
	}

	//Update Augmenta
	m_oPeople = AugmentaReceiver.getPeople();
	m_oActualScene = AugmentaReceiver.getScene();

	//Update GUI
	m_iNumberOfAreaPolygons = m_vAreaPolygonsVector.size();

	//Update Colision
	for (size_t i = 0; i < m_iNumberOfAreaPolygons; i++){
		if (m_vAreaPolygonsVector[i].isCompleted()){
			m_vAreaPolygonsVector[i].update(m_oPeople, m_iAntiBounce);
		}
	}
	
	//Osc
	sendOSC();
}

//--------------------------------------------------------------
void ofApp::fboSizeHaveChanged(int a_iNewWidth, int a_iNewHeight){
	if (a_iNewWidth != 0 && a_iNewHeight != 0){
		if (m_iFboWidth != a_iNewWidth || m_iFboHeight != a_iNewHeight){
			m_iFboWidth = a_iNewWidth;
			m_iFboHeight = a_iNewHeight;
			m_fbo.allocate(m_iFboWidth, m_iFboHeight, GL_RGBA);
		}
	}
}

//--------------------------------------------------------------
bool ofApp::isInsideAPolygon(ofVec2f a_oPoint){
	//We don't count the last poly because it his in construction
	if (m_bEditMode){
		for (int i = 0; i < m_vAreaPolygonsVector.size() - 1; i++){
			if (m_vAreaPolygonsVector[i].isPointInPolygon(ofPoint(static_cast<float>(a_oPoint.x) / m_iFboWidth, static_cast<float>(a_oPoint.y) / m_iFboHeight))){
				return true;
			}
		}
		return false;
	}
	else{
		for (int i = 0; i < m_vAreaPolygonsVector.size(); i++){
			if (m_vAreaPolygonsVector[i].isPointInPolygon(ofPoint(static_cast<float>(a_oPoint.x) / m_iFboWidth, static_cast<float>(a_oPoint.y) / m_iFboHeight))){
				return true;
			}
		}
		return false;
	}
}

//_______________________________________________________________
//_____________________________DRAW______________________________
//_______________________________________________________________

//--------------------------------------------------------------
void ofApp::drawAreaPolygons(){
	for (int i = 0; i < m_vAreaPolygonsVector.size(); ++i){
		m_vAreaPolygonsVector[i].draw(m_iFboWidth , m_iFboHeight );
	}
}

//--------------------------------------------------------------
void ofApp::drawAugmentaPeople(){
	ofPoint centroid;
	ofPushStyle();
	ofSetColor(ofColor(93,224,133,200));
	for (int i = 0; i<m_oPeople.size(); i++){
		centroid = m_oPeople[i]->centroid;
		ofDrawCircle(centroid.x * m_iFboWidth , centroid.y * m_iFboHeight , 10);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if(m_bAutoSize){
        fboSizeHaveChanged(m_oActualScene->width, m_oActualScene->height);
    } else {
        fboSizeHaveChanged(m_iXMLFboWidth, m_iXMLFboHeight);
    }
    
	m_fbo.begin();
    
	ofClear(ofColor(20,20,20)); // Clear FBO content to black
	ofDisableDepthTest();

    drawAugmentaPeople();
	drawAreaPolygons();

    m_fbo.end();

	if (m_bSendFbo){
		sendVisuals();
	}

    // Draw interface
    if(m_bHideInterface){
        drawHiddenInterface();
    } else{
        drawInterface();
    }
    
}

//--------------------------------------------------------------
// Send the FBO to Syphon (OSX) or Spout (WIN32)
void ofApp::sendVisuals(){
    
    #ifdef WIN32
    // On Windows, use Spout
	m_spoutSender.sendTexture(m_fbo.getTextureReference(), APP_NAME);
    #elif MAC_OS_X_VERSION_10_6
    // On Mac OSX, use Syphon
    m_syphonServer.publishTexture(&m_fbo.getTexture());
    #endif
}

//--------------------------------------------------------------
// Draw the interface of your app : visuals, GUI, debug content...
void ofApp::drawInterface(){
    
	int min = 700;
	int max = 1000;

    ofBackground(ofColor::darkGray); // Clear screen

	m_iWidthRender = m_iFboWidth;
	m_iHeightRender = m_iFboHeight;


	float fFboRatio = (float)m_iFboWidth / (float)m_iFboHeight;

	// Portrait or square mode Height >= Width
	if (fFboRatio <= 1){
		if (m_iHeightRender > max){
			m_iHeightRender = max;
			m_iWidthRender = m_iHeightRender * fFboRatio;
		}
		if (m_iWidthRender < min){
			m_iWidthRender = min;
			m_iHeightRender = m_iWidthRender / fFboRatio;
		}
	}
	// Landscape mode Height < Width
	if (fFboRatio > 1){
		if (m_iWidthRender > max){
			m_iWidthRender = max;
			m_iHeightRender = m_iWidthRender / fFboRatio;
		}
		if (m_iHeightRender < min){
			m_iHeightRender = min;
			m_iWidthRender = m_iHeightRender * fFboRatio;
		}
	}
	
	m_fbo.draw(m_pFboOffset.x, m_pFboOffset.y, m_iWidthRender * m_fZoomCoef, m_iHeightRender * m_fZoomCoef);
	//ofSetWindowShape(m_iWidthRender * m_fZoomCoef, m_iHeightRender * m_fZoomCoef);
	
	// Update the UI
	m_sNumberOfAreaPolygons = ofToString(m_iNumberOfAreaPolygons);
	m_gui.draw();

}

//--------------------------------------------------------------
// Minimalist interface with a black background and some information
void ofApp::drawHiddenInterface(){
    
    ofBackground(ofColor::black);
    
    ofPushStyle();
    ofSetColor(ofColor::white);
    
    // Create string with senders
    string sendersString = "";
    for (int i = 0 ; i < m_sOscSenderHosts.size() ; i++){
        sendersString += m_sOscSenderHosts[i];
        sendersString += ":";
        sendersString += ofToString(m_iOscSenderPorts[i]);
        if (i < m_sOscSenderHosts.size()-1){
            sendersString += " / ";
        }
    }
    

	ofDrawBitmapString("FPS: " +
		ofToString(ofGetFrameRate()) + "\n\n"
        "Window res: " + m_sScreenResolution + "\n" +
        "FBO res: " + m_sSendFboResolution + "\n\n" +
        "Receiving OSC on port : " + ofToString(m_iOscReceiverPorts[0]) + "\n" +
		"Sending OSC to " + sendersString + "\n"
		+ "\n" +
		"---------------------------------------\n"
		"\n[h] to unhide interface\n" \
		"[ctrl+s] / [cmd+s] to save settings and the currents polygons\n" \
		"[ctrl+l] / [cmd+l] to load last saved settings and polygons\n" \
		"[ctrl+z] to delete the last polygon created or the current polygon\n" \
		"[r] / [R] to delete all the polygons you have created\n" \
		"[del] / [backSpace] to delete the selected polygon\n" \
		"[-] To zoom in(only for a better appreciation on the screen).\n"\
		"[+] To zoom out(only for a better appreciation on the screen).\n"\
		"[left click] to create a new point or polygon\n" \
		"[right click] to delete the last point created\n" \
		"[left click] inside a polygon to select it /outside a polygon to deselect it\n\n"

		"---------------------------------------\n" \
		"\nTo optimize performance : \n\n" \
		"  - Stay in this hidden interface mode\n" \
		"  - Minimize this window\n" \
		"\nNote : Your settings are saved when the app quits and are loaded at startup. (autosave feature)\n" \
		"The dimensions will be the same as the ones sent by Augmenta.\n" \
		"You can change the osc messages of a polygon in the preferences.xml file."\
        ,20,20);
    
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::deleteLastPolygon() {
    
    if (m_vAreaPolygonsVector.size() >= 1){
        if (m_bSelectMode){
            m_vAreaPolygonsVector[m_iIndicePolygonSelected].hasBeenSelected(false);
            m_iIndicePolygonSelected = -1;
            m_bSelectMode = false;
        }
        m_bEditMode = false;
        m_vAreaPolygonsVector.pop_back();
        ofLogVerbose("deleteLastPolygon", "Last AreaPolygon is now deleted ");
    }
}

//--------------------------------------------------------------
void ofApp::deleteAllPolygon(){
	if (m_bSelectMode){
		m_vAreaPolygonsVector[m_iIndicePolygonSelected].hasBeenSelected(false);
		m_iIndicePolygonSelected = -1;
		m_bSelectMode = false;
	}
	m_vAreaPolygonsVector.clear();
	m_bEditMode = false;
	m_iNextFreeId = 0;
	ofLogVerbose("deleteAllPolygon", "All AreaPolygons are now deleted ");
}

//--------------------------------------------------------------
void ofApp::resetFboView(){
    m_pFboOffset = ofPoint(0,0);
    m_fZoomCoef = 1.f;
}

//_______________________________________________________________
//_____________________________INPUT_____________________________
//_______________________________________________________________

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
 
    switch(key){
        // Modifier keys
        case OF_KEY_SHIFT:
            m_iModifierKey = OF_KEY_SHIFT;
            break;
        case OF_KEY_ALT:
            m_iModifierKey = OF_KEY_ALT;
            break;
        case OF_KEY_CONTROL:
            m_iModifierKey = OF_KEY_CONTROL;
            break;
        case OF_KEY_COMMAND:
            m_iModifierKey = OF_KEY_COMMAND;
            break;

		case '+':
			if (m_fZoomCoef + 0.2 < m_fZoomCoef.getMax()){
				m_fZoomCoef = m_fZoomCoef + 0.2;
			}
			else{
				m_fZoomCoef = m_fZoomCoef.getMax();
			}
			break;

		case '-':
			if (m_fZoomCoef - 0.2 > m_fZoomCoef.getMin()){
				m_fZoomCoef = m_fZoomCoef-0.2;
			}
			else{
				m_fZoomCoef = m_fZoomCoef.getMin();
			}
			break;

        case 'f':
        case 'F':
            ofToggleFullscreen();
            break;
            
        case 'h':
        case 'H':
            // Hide UI to save some performances (or for discretion)
            m_bHideInterface = !m_bHideInterface;
            break;
            
        case 's':
        case 'S':
            // CTRL+S or CMD+S
            if(m_iModifierKey == OF_KEY_CONTROL || m_iModifierKey == OF_KEY_COMMAND){
                // Save settings
                save(m_sSettingsPath);
				ofLogVerbose("keyPressed", "Settings have been successfully saved ");
            }
            break;
        
        case 'z':
        case 'Z':
            // CTRL+Z or CMD+Z
            if(m_iModifierKey == OF_KEY_CONTROL || m_iModifierKey == OF_KEY_COMMAND){
                deleteLastPolygon();
            }
            break;
            
        case 'l':
        case 'L':
            // CTRL+L or CMD+L
            if(m_iModifierKey == OF_KEY_CONTROL || m_iModifierKey == OF_KEY_COMMAND){
                // Load settings
                load(m_sSettingsPath);
				ofLogVerbose("keyPressed", "Settings have been successfully loaded ");
            }
            break;
            
        case 19:
            // CTRL+S or CMD+S
            save(m_sSettingsPath);
			ofLogVerbose("keyPressed", "Settings have been successfully saved ");
            break;
            
        case 12:
            // CTRL+L or CMD+L
            load(m_sSettingsPath);
			ofLogVerbose("keyPressed", "Settings have been successfully loaded ");
            break;

			//CTRL + Z
			//Delete Last AreaPolygon
		case 26:
            deleteLastPolygon();
			break;

		//Delete the selected poly
		case OF_KEY_DEL :			
		case OF_KEY_BACKSPACE :
			if (m_vAreaPolygonsVector.size() >= 1){
				if (m_bSelectMode && !m_bEditMode){
					m_vAreaPolygonsVector.erase(m_vAreaPolygonsVector.begin() + m_iIndicePolygonSelected);
					m_iIndicePolygonSelected = -1;
					m_bSelectMode = false;		
					ofLogVerbose("keyPressed", "The selected polygon is now deleted");
				}
			}
			break;
		
		//Delete all AreaPolygons
		case 'r':
		case 'R':			
			deleteAllPolygon();		
			break;

        //Move the selected polygon
		if (m_bSelectMode){
			case OF_KEY_LEFT:
				if (m_iIndicePolygonSelected != -1){
					m_vAreaPolygonsVector[m_iIndicePolygonSelected].moveLeft();
					m_vAreaPolygonsVector[m_iIndicePolygonSelected].setPolygonCentroid();
					ofLogVerbose("keyPressed", "go left !");
				}
				break;

			case OF_KEY_RIGHT:
				if (m_iIndicePolygonSelected != -1){
					m_vAreaPolygonsVector[m_iIndicePolygonSelected].moveRight();
					m_vAreaPolygonsVector[m_iIndicePolygonSelected].setPolygonCentroid();
					ofLogVerbose("keyPressed", "go right !");
				}
				break;

			case OF_KEY_UP:
				if (m_iIndicePolygonSelected != -1){
					m_vAreaPolygonsVector[m_iIndicePolygonSelected].moveUp();
					m_vAreaPolygonsVector[m_iIndicePolygonSelected].setPolygonCentroid();
					ofLogVerbose("keyPressed", "go up !");
				}
				break;

			case OF_KEY_DOWN:			
				if (m_iIndicePolygonSelected != -1){
					m_vAreaPolygonsVector[m_iIndicePolygonSelected].moveDown();
					m_vAreaPolygonsVector[m_iIndicePolygonSelected].setPolygonCentroid();
					ofLogVerbose("keyPressed", "go down !");
				}
				break;
		}
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    switch (key){
            
        // Release modifier key
        case OF_KEY_SHIFT:
        case OF_KEY_ALT:
        case OF_KEY_CONTROL:
        case OF_KEY_COMMAND:
            m_iModifierKey = -1;
            break;
           
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
    // Get transformed coords for mouse
	ofPoint temp = transformMouseCoord(x, y);
	int tx = temp.x;
	int ty = temp.y;
    // Get transformed coords for old pos
    temp = transformMouseCoord(m_oOldMousePosition.x, m_oOldMousePosition.y);
    int tox = temp.x;
    int toy = temp.y;
    
    // Screen space movement
    ofVec2f movement = ofVec2f(m_oOldMousePosition.x - x, m_oOldMousePosition.y - y);
    // Transformed movement
	ofVec2f tmovement = ofVec2f(tox - tx, toy - ty);

	if (button == 0){
		if (!m_bEditMode){
			if (m_bSelectMode){
                AreaPolygon* poly = &m_vAreaPolygonsVector[m_iIndicePolygonSelected];
                // First test if we have clicked a point
                ofVec2f dividedTemp = *new ofVec2f((float)tx/(float)m_iFboWidth, (float)ty/(float)m_iFboHeight);
                if (m_iCurrentPointMoved != -1){
                    poly->movePoint(m_iCurrentPointMoved, dividedTemp);
                } else {
                    // Else it's the full polygon
                    poly->move(static_cast<float>(tmovement.x) / m_iFboWidth, static_cast<float>(tmovement.y) / m_iFboHeight);
                    poly->setPolygonCentroid();
                }
			}
		}
    } else if (button == 1){
        m_pFboOffset -= movement;
    }
	
    m_oOldMousePosition = ofVec2f(x,y);
 
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){
    m_fZoomCoef = m_fZoomCoef + scrollY*0.05f;
    //Revert if we went too far
    if (m_fZoomCoef < 0.1f) m_fZoomCoef = m_fZoomCoef - scrollY*0.05f;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

    // Get transformed coords for mouse
    ofPoint temp = transformMouseCoord(x, y);
    int tx = temp.x;
    int ty = temp.y;
    // Get transformed coords for old pos
    temp = transformMouseCoord(m_oOldMousePosition.x, m_oOldMousePosition.y);
    int tox = temp.x;
    int toy = temp.y;
    m_oOldMousePosition = ofVec2f(x,y);
    
	bool isLastPoint = false;

	if (button == 2 && !m_bSelectMode){
		//One AreaPolygon is not finish
		if (m_bEditMode){
			if (m_vAreaPolygonsVector[m_vAreaPolygonsVector.size() - 1].removeLastPoint()){
				//if the value return by the remove last point value 
			}
			else{
				m_vAreaPolygonsVector.pop_back();
				m_bEditMode = false;
			}
		}
	}
	
	if (button == 0){
		//Creation of poly
		if (!m_bSelectMode && !isInsideAPolygon(ofVec2f(tx, ty))){
			//One AreaPolygon is not finish
			if (m_bEditMode){
				//Is closing the poly
				ofVec2f temp = m_vAreaPolygonsVector[m_iNumberOfAreaPolygons - 1].getPoint(0);
				temp.x = temp.x * m_iFboWidth;
				temp.y = temp.y * m_iFboHeight;

				if (temp.distance(ofVec2f(tx, ty)) < m_iRadiusClosePolyZone){
					m_vAreaPolygonsVector[m_iNumberOfAreaPolygons - 1].complete();
					m_bEditMode = false;
					if (m_vAreaPolygonsVector[m_iNumberOfAreaPolygons - 1].getSize() <= 2){
						m_vAreaPolygonsVector.pop_back();
					}
					isLastPoint = true;
				}
				//Create another point
				else{
					m_vAreaPolygonsVector[m_iNumberOfAreaPolygons - 1].addPoint(ofVec2f(static_cast<float>(tx) / m_iFboWidth, static_cast<float>(ty) / m_iFboHeight));
				}
			}

			//Every AreaPolygons are completed
			else{
				m_vAreaPolygonsVector.push_back(AreaPolygon(ofVec2f(static_cast<float>(tx) / m_iFboWidth, static_cast<float>(ty) / m_iFboHeight), m_oPeople, m_iNextFreeId,m_iAntiBounce));
				m_iNextFreeId++;
				m_bEditMode = true;
			}
		}

		//Selection
		if (!m_bEditMode && !isLastPoint){
            // Test polygon selection mode
			if(!m_bSelectMode){
				for (int i = 0; i < m_vAreaPolygonsVector.size(); i++){
					if (m_vAreaPolygonsVector[i].isPointInPolygon(ofPoint(static_cast<float>(tx) / m_iFboWidth, static_cast<float>(ty) / m_iFboHeight))){
						//We enter in selection mode
						m_iIndicePolygonSelected = i;
						m_vAreaPolygonsVector[i].hasBeenSelected(true);
						m_bSelectMode = true;
						break;//because we only want one selectd poly
					}
				}
                m_iCurrentPointMoved = -1;
			}
			else{
                bool isMouseInsideSelected = m_vAreaPolygonsVector[m_iIndicePolygonSelected].isPointInPolygon(ofPoint(static_cast<float>(tx) / m_iFboWidth, static_cast<float>(ty) / m_iFboHeight));
                int selectedPoint = m_vAreaPolygonsVector[m_iIndicePolygonSelected].pointClicked(ofVec2f(static_cast<float>(tx) / m_iFboWidth, static_cast<float>(ty) / m_iFboHeight));
                if (selectedPoint != -1){
                    m_iCurrentPointMoved = selectedPoint;
                } else {
                    m_iCurrentPointMoved = -1;
                }
                
                if(!isMouseInsideSelected && selectedPoint == -1 ){
					//We leave the selection mode
					m_vAreaPolygonsVector[m_iIndicePolygonSelected].hasBeenSelected(false);
					m_iIndicePolygonSelected = -1;
					m_bSelectMode = false;

					//We change the selected poly
					for (int i = 0; i < m_vAreaPolygonsVector.size(); i++){
						if (m_vAreaPolygonsVector[i].isPointInPolygon(ofPoint(static_cast<float>(tx) / m_iFboWidth, static_cast<float>(ty) / m_iFboHeight))){
							m_iIndicePolygonSelected = i;
							m_vAreaPolygonsVector[i].hasBeenSelected(true);
							m_bSelectMode = true;
							break;//because we only want one selectd poly
						}

					}
				}
			}
		}
	}
}



//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	
    // Get transformed coords for mouse
    ofPoint temp = transformMouseCoord(x, y);
    int tx = temp.x;
    int ty = temp.y;
    // Get transformed coords for old pos
    temp = transformMouseCoord(m_oOldMousePosition.x, m_oOldMousePosition.y);
    int tox = temp.x;
    int toy = temp.y;
    m_oOldMousePosition = ofVec2f(x,y);

    // Transformed movement
    ofVec2f tmovement = ofVec2f(tox - tx, toy - ty);

	if (button == 0){
		if (!m_bEditMode){
			if (m_bSelectMode){
				m_vAreaPolygonsVector[m_iIndicePolygonSelected].move(static_cast<float>(tmovement.x) / m_iFboWidth, static_cast<float>(tmovement.y) / m_iFboHeight);
				m_vAreaPolygonsVector[m_iIndicePolygonSelected].setPolygonCentroid();
			}
		}
	}
    
    m_iCurrentPointMoved = -1;
}

//--------------------------------------------------------------
ofPoint ofApp::transformMouseCoord(int x, int y){
    x -= m_pFboOffset.x;
    y -= m_pFboOffset.y;
	x = x * m_iFboWidth / (m_iWidthRender * m_fZoomCoef);
	y = y * m_iFboHeight / (m_iHeightRender * m_fZoomCoef);
	return ofPoint(x, y);
}

//_______________________________________________________________
//_____________________________SAVE & LOAD_______________________
//_______________________________________________________________

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo drag){
    // Save first
    save("autosave.xml");
    // Then load new pref file
    load(drag.files[0]);
    setupOSC();
}
//--------------------------------------------------------------
void ofApp::onLoadPressed(){
    ofLogNotice("Load button pressed");
    load(m_sSettingsPath);
    setupOSC();
}
//--------------------------------------------------------------
void ofApp::onSavePressed(){
    ofLogNotice("Save button pressed");
    save(m_sSettingsPath);
}

//--------------------------------------------------------------
void ofApp::save(string _sPath){
    ofLogNotice() << "Saving settings to "<< _sPath;
    
    // Clean existing file if needed
    if(ofFile::doesFileExist(_sPath)){
        ofFile::removeFile(_sPath);
        ofLogNotice("File correctly cleaned");
    }
    
    // ----------------------------------------------
    // First save GUI parameters
    // ----------------------------------------------
    m_gui.saveToFile(_sPath);
    ofLogNotice("GUI saved...");
	
    // ----------------------------------------------
    // Then save other settings
    // ----------------------------------------------

    // First reopen the file
    ofxXmlSettings settings;
    settings.load(_sPath);
    
    settings.addTag("Settings");
    settings.pushTag("Settings");
    settings.addValue("HideInterface", m_bHideInterface);
    settings.addValue("LogToFile", m_bLogToFile);
    
    if (m_iXMLFboWidth == 0) m_iXMLFboWidth = m_iFboWidth;
    if (m_iXMLFboHeight == 0) m_iXMLFboHeight = m_iFboHeight;
    settings.addValue("FboWidth", m_iXMLFboWidth);
    settings.addValue("FboHeight", m_iXMLFboHeight);
    settings.addValue("NextFreeId", m_iNextFreeId);
    settings.popTag();
    ofLogNotice() << "Other settings saved...";
    
    // ----------------------------------------------
    // Then save OSC settings
    // ----------------------------------------------
    settings.addTag("OSC");
    settings.pushTag("OSC");
    // Receivers
    for (int i = 0; i< m_oscReceivers.size(); i++){
        settings.addValue("Receiver", m_iOscReceiverPorts[i]);
    }
    // Senders
    for (int i = 0; i< m_oscSenders.size(); i++){
        //std::cout << "Save one sender" << std::endl;
        settings.addTag("Sender");
        settings.pushTag("Sender",i);
        settings.addValue("Ip", m_sOscSenderHosts[i]);
        settings.addValue("Port", m_iOscSenderPorts[i]);
        settings.popTag();
    }
    settings.popTag();
    ofLogNotice() << "OSC saved...";
    
    // ----------------------------------------------
    // Then save Polygons
    // ----------------------------------------------
    for (int i = 0; i < m_iNumberOfAreaPolygons; i++){
        if (m_vAreaPolygonsVector[i].isCompleted()){
            
            settings.addTag("AreaPolygon");
            settings.pushTag("AreaPolygon", i);
            
            for (int j = 0; j < m_vAreaPolygonsVector[i].getSize(); j++){
                settings.addTag("Point");
                settings.pushTag("Point", j);
                settings.addValue("x", m_vAreaPolygonsVector[i].getPoint(j).x);
                settings.addValue("y", m_vAreaPolygonsVector[i].getPoint(j).y);
                settings.popTag();
            }
            vector<ofxOscMessage> ins = m_vAreaPolygonsVector[i].getInOscMessages();
            vector<ofxOscMessage> outs = m_vAreaPolygonsVector[i].getOutOscMessages();
            settings.addTag("Osc");
            settings.pushTag("Osc");
            for (int j=0; j < ins.size(); j++){
                settings.addValue("In", m_vAreaPolygonsVector[i].messageToString(ins[j]));
            }
            for (int j=0; j < outs.size(); j++){
                settings.addValue("Out", m_vAreaPolygonsVector[i].messageToString(outs[j]));
            }
            settings.popTag();
            settings.popTag();
        }
    }
    settings.popTag();
    
    // ----------------------------------------------
    // Write File !
    // ----------------------------------------------
    
    // We have to do a little trick to combine the GUI XML with this one...
    // First copy the XML to text
    string more;
    settings.copyXmlToString(more);
    ofBuffer xml;
    xml.append("<root>\n");
    xml.append(more);
    xml.append("</root>");
    //Write
    ofBufferToFile(_sPath, xml);

    ofLogNotice("Saving finished");
    
}

//--------------------------------------------------------------
void ofApp::load(string _sPath){
    ofLogNotice() << "Trying to load "<< _sPath;
    
    // Check if file exists before opening it
    if(ofFile::doesFileExist(_sPath)){
        ofLogNotice() << "Loading settings";
        
        
        
        // ----------------------------------------------
        // First load GUI parameters
        // ----------------------------------------------
        
        // We must make a temp file with only the GUI parameters
        ofFile temp;
        temp.open(_sPath);
        ofBuffer fullBuff = temp.readToBuffer();
        stringstream ss;
        ss << fullBuff;
        string guiText = ofSplitString(ss.str(), "<GUI>")[1];
        guiText = ofSplitString(guiText,"</GUI>")[0];
        guiText = "<GUI>"+guiText+"</GUI>";
        ofBuffer tempBuff;
        tempBuff.append(guiText);
        ofBufferToFile("temp.xml", tempBuff);
        // Now we load it
        m_gui.loadFromFile("temp.xml");
        // And we delete the temp
        if(ofFile::doesFileExist("temp.xml")){
            ofFile::removeFile("temp.xml");
            ofLogNotice("Temp file correctly cleaned");
        }
        
        // Set a few things now it's loaded...
		if (m_bSelectMode){
			m_vAreaPolygonsVector[m_iIndicePolygonSelected].hasBeenSelected(false);
			m_iIndicePolygonSelected = -1;
			m_bSelectMode = false;
		}
		m_vAreaPolygonsVector.clear();
		m_bEditMode = false;
        
        // ----------------------------------------------
        // Then load other settings
        // ----------------------------------------------
        ofxXmlSettings settings;
        settings.load(_sPath);
        
        settings.pushTag("root");
        settings.pushTag("Settings");
        m_bHideInterface = settings.getValue("HideInterface", m_bHideInterface);
        m_bLogToFile = settings.getValue("LogToFile", m_bLogToFile);
        m_iXMLFboWidth = settings.getValue("FboWidth", m_iFboWidth);
        m_iXMLFboHeight = settings.getValue("FboHeight", m_iFboHeight);
        m_iFboWidth = m_iXMLFboWidth;
        m_iFboHeight = m_iXMLFboHeight;
        m_iNextFreeId = settings.getValue("NextFreeId", m_iNextFreeId);
        settings.popTag();
        ofLogNotice() << "Size fbo loaded : " << m_iFboWidth << "/" << m_iFboHeight;
        
        // ----------------------------------------------
        // Then load OSC settings
        // ----------------------------------------------
        
        if (settings.tagExists("OSC")){
            
            settings.pushTag("OSC");
            
            // Receivers
            int nbrReceivers = settings.getNumTags("Receiver");
            if (nbrReceivers != 0){
                m_iOscReceiverPorts.clear();
                for (int i=0; i < nbrReceivers ; i++){
                    m_iOscReceiverPorts.push_back(settings.getValue("Receiver", 12000));
                }
            }
            
            // Senders
            int nbrSenders = settings.getNumTags("Sender");
            if (nbrSenders != 0){
                m_sOscSenderHosts.clear();
                m_iOscSenderPorts.clear();
                for (int i=0; i < nbrSenders ; i++){
                    settings.pushTag("Sender", i);
                    m_iOscSenderPorts.push_back(settings.getValue("Port", 7000));
                    m_sOscSenderHosts.push_back(settings.getValue("Ip", "127.0.0.1"));
                    settings.popTag();
                }
            }
            
            settings.popTag();
        }
        
        // ----------------------------------------------
        // Then load Polygons
        // ----------------------------------------------
        ofVec2f p;
        int nbrPoints;
        int nbrPolygons;
        nbrPolygons = settings.getNumTags("AreaPolygon");
        ofLogNotice("loadPreferences") << "load of " << nbrPolygons << " polygons";
        
        for (int i = 0; i < nbrPolygons; ++i){
            settings.pushTag("AreaPolygon", i);
            
            nbrPoints = settings.getNumTags("Point");
            
            for (int j = 0; j < nbrPoints; j++){
                settings.pushTag("Point", j);
                
                p.x = settings.getValue("x", 0.0f);
                p.y = settings.getValue("y", 0.0f);
                
                if (j == 0){
                    m_vAreaPolygonsVector.push_back(AreaPolygon(ofVec2f(p.x, p.y), m_oPeople, 0, m_iAntiBounce));
                    
                }
                else{
                    m_vAreaPolygonsVector[i].addPoint(ofVec2f(p.x, p.y));
                }
                
                ofLogVerbose("loadPreferences") << "x = " << p.x;
                ofLogVerbose("loadPreferences") << "y = " << p.y;
                
                settings.popTag();
            }
            m_vAreaPolygonsVector[i].complete();
            
            settings.pushTag("Osc");
            
            // Handle multiple IN/OUT messages
            int nbrIn = settings.getNumTags("In");
            int nbrOut = settings.getNumTags("Out");
            vector<string> ins;
            vector<string> outs;
            
            for (int i=0 ; i < nbrIn ; i++){
                ins.push_back(settings.getValue("In",  "/area" + ofToString(m_iNextFreeId) + "/personEntered", i));
            }
            for (int i=0 ; i < nbrOut ; i++){
                outs.push_back(settings.getValue("Out", "/area" + ofToString(m_iNextFreeId) + "/personWillLeave", i));
            }
            // Load the strings into objects
            m_vAreaPolygonsVector[i].loadOscMessages(ins,outs);
            settings.popTag();
            
            settings.popTag();
        }
        settings.popTag();
        settings.popTag();
        
        // Perform a sanity check
        if (m_iNextFreeId < m_vAreaPolygonsVector.size()){
            ofLogWarning("Problem detected in the naming of the polygons.,.");
        }
        
    } else {
        ofLogNotice() << "Could not load settings : file '"<<_sPath<<"' doesn't exist";
    }
}


//_______________________________________________________________
//_____________________________OSC_______________________________
//_______________________________________________________________

//--------------------------------------------------------------
void ofApp::sendOSC(){
    
ofxOscMessage m;
    for (int i = 0; i < m_iNumberOfAreaPolygons; ++i){
        AreaPolygon ap = m_vAreaPolygonsVector[i];
        if (ap.isCompleted()){
            // IN
            if (ap.getPeopleMovement() > 0){
                // Get all in messages
                vector<ofxOscMessage> ins = ap.getInOscMessages();
                // Send each message
                for (int i=0; i<ins.size(); i++){
                    ofxOscMessage m = ins[i];
                    if (m_bRedondanteMode){
                        for (int j = 0; j < ap.getPeopleMovement(); j++){
                            for (int k=0; k<m_oscSenders.size(); k++){
                                m_oscSenders[k].sendMessage(m); // Send it
                                //std::cout << "Sending on sender num : "<< k<<" with port : "<< m_iOscSenderPorts[k] << std::endl;
                            }
                            ofLogVerbose("sendOSC") << m.getAddress();
                        }
                    } else {
                        for (int k=0; k<m_oscSenders.size(); k++){
                            m_oscSenders[k].sendMessage(m); // Send it
                        }
                        ofLogVerbose("sendOSC") << m.getAddress();
                    }
                    m.clear(); // Clear message to be able to reuse it
                }
            // OUT
            } else if (ap.getPeopleMovement() < 0){
                // Get all in messages
                vector<ofxOscMessage> outs = ap.getOutOscMessages();
                // Send each message
                for (int i=0; i<outs.size(); i++){
                    ofxOscMessage m = outs[i];
                    if (m_bRedondanteMode){
                        for (int j = 0; j > ap.getPeopleMovement(); j--){
                            for (int k=0; k<m_oscSenders.size(); k++){
                                m_oscSenders[k].sendMessage(m); // Send it
                            }
                            ofLogVerbose("sendOSC") << m.getAddress();
                        }
                    } else {
                        for (int k=0; k<m_oscSenders.size(); k++){
                            m_oscSenders[k].sendMessage(m); // Send it
                        }
                        ofLogVerbose("sendOSC") << m.getAddress();
                    }
                    m.clear(); // Clear message to be able to reuse it
                }
            }
        }
    }
}

//_______________________________________________________________
//_____________________________EXIT_______________________________
//_______________________________________________________________

//--------------------------------------------------------------
void ofApp::exit(){

#ifdef WIN32
	m_spoutSender.exit();
#endif

	/*
	Save parameters set in GUI before closing app
	Parameters are saved in another file that the one used in saveSettings() function
	to prevent saving wrong parameters if application quit unexpectedly.
	*/
    save("autosave.xml");
	// Remove listener because instance of our gui button will be deleted
	m_bResetSettings.removeListener(this, &ofApp::reset);
    m_bResetFboView.removeListener(this, &ofApp::resetFboView);
}
