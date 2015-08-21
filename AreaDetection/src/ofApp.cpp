#include "ofApp.h"

#define APP_NAME "AreaDetection"

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
    setupPreferences();
    setupGUI();
    setupOSC();
    
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
}

//--------------------------------------------------------------
void ofApp::init(){
    
    //--------------------------------------------
    // Change default values here.
    //--------------------------------------------
    
    // App default values (preferences.xml)
    m_bHideInterface = false;
    m_bLogToFile = false;
    m_iFboWidth = ofGetWidth();
    m_iFboHeight = ofGetHeight();
    m_iOscReceiverPort = 12000;
    m_iOscSenderPort = 12000;
    m_sOscSenderHost = "127.0.0.1";
    m_sReceiverOscDisplay = "Listening to OSC on port " + ofToString(m_iOscReceiverPort) + "\n";
    
    // GUI default value (settings.xml)
    m_fPointRadius = 20;
	m_iLinesWidthSlider = 2;
	m_bRedondanteMode = true;
	m_bIsCreating = false;
	m_iNumberOfAreaPolygons = 0;
	m_iRadiusClosePolyZone = 30;
    m_vMyVec = ofVec3f(1,2,3);
    
    //--------------------------------------------

}

//--------------------------------------------------------------
void ofApp::reset(){
    
    init();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Do some OSC stuff ...
   // receiveOSC();
   // sendOSC();
    

	//Update GUI
	m_iNumberOfAreaPolygons = m_vAreaPolygonsVector.size();
	for (size_t i = 0; i < m_vAreaPolygonsVector.size(); i++){
		m_vAreaPolygonsVector[i].setRadius(m_fPointRadius);
		m_vAreaPolygonsVector[i].setLinesWidth(m_iLinesWidthSlider);
	}
}

//--------------------------------------------------------------
void ofApp::drawAreaPolygons(){
	m_fbo.begin();
	ofClear(ofColor::black); // Clear FBO content to black
	ofEnableDepthTest();

	for (int i = 0; i < m_vAreaPolygonsVector.size(); ++i){
		m_vAreaPolygonsVector[i].draw();
	}

	ofDisableDepthTest();
	m_fbo.end();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
 //   drawVisuals(); // Draw visuals (in a FBO)
	
	drawAreaPolygons();

	sendVisuals(); // Remove this line if you don't need it !
    
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
    m_syphonServer.publishTexture(&m_fbo.getTextureReference());
    #endif
}

//--------------------------------------------------------------
// Only draw the content that will be sent outside the app here
/*
void ofApp::drawVisuals(){
    
    m_fbo.begin(); // Render scene in the FBO
    
    ofClear(ofColor::black); // Clear FBO content to black
    
    // Draw whatever you want here !
    
    // Remember to disable it when you don't need it anymore, to prevent problems in other drawing functions !
    ofEnableDepthTest();
    
    //ofCamera myCam = ofCamera();    // A camera is needed to draw 3D stuff. Here, we use an ofEasyCam, which allows us to navigate in 3D with mouse.
    m_myCam.begin();                  // Start rendering to camera
    
    // /!\ REMEMBER : every time you make a Push(something), you MUST do a Pop(something) somewhere after.
    ofPushMatrix();                 // Store your current transformation matrix.
    ofPushStyle();                  // Store your current style parameters (color, fill...)
    
    ofTranslate(m_,0,0);          // After this point, all content is translated in space to position x:m_iMyInt, y:0, z:0.
    ofRotate(ofGetFrameNum() % 360, m_vMyVec->x, m_vMyVec->y, m_vMyVec->z);
   
    ofFill();
    ofDrawBox(120);
    // Draw outline of the cube
    ofNoFill();
    ofSetColor(ofColor::black);
    ofDrawBox(120);
    
    ofPopStyle();                   // Go back to the previous style (the one defined before pushStyle). For example, the color is not set to salmon anymore
    ofPopMatrix();                  // Go back to the previous transform matrix (the one defined before pushMatrix). For example, the translation and rotation defined above aren't taken into account anymore.
    
    m_myCam.end();                    // Stop rendering to camera
    
    ofDisableDepthTest();
    
    m_fbo.end(); // End of rendering inside FBO
}*/

//--------------------------------------------------------------
// Draw the interface of your app : visuals, GUI, debug content...
void ofApp::drawInterface(){
    
    ofBackground(ofColor::darkGray); // Clear screen
    
    // Show the FBO in the app window with right ratio
    float fFboRatio = (float) m_iFboWidth / (float) m_iFboHeight;
    if(fFboRatio > 1){
        m_fbo.draw(0, 0, ofGetWidth(), ofGetWidth() / fFboRatio);
    } else {
        m_fbo.draw(0, 0, ofGetHeight() * fFboRatio, ofGetHeight());
    }
    
    // Update the UI element containing framerate
    m_sFramerate = ofToString(ofGetFrameRate());
	m_sNumberOfAreaPolygons = ofToString(m_iNumberOfAreaPolygons);
    m_gui.draw();
}

//--------------------------------------------------------------
// Minimalist interface with a black background and some information
void ofApp::drawHiddenInterface(){
    
    ofBackground(ofColor::black);
    
    ofPushStyle();
    ofSetColor(ofColor::white);
    
    ofDrawBitmapString("FPS: " +
                       ofToString(ofGetFrameRate()) + "\n" +
                       m_sReceiverOscDisplay +
                       "Sending OSC to " + m_sOscSenderHost + ":" + ofToString(m_iOscSenderPort) + "\n\n" +
                       "---------------------------------------\n"
                       "\n[h] to unhide interface\n" \
                       "[ctrl+s] / [cmd+s] to save settings\n" \
                       "[ctrl+l] / [cmd+l] to load last saved settings\n" \
                       "[left click] to move the 3D camera\n" \
                       "[right click] to zoom with 3D camera\n\n" \
                       "---------------------------------------\n" \
                       "\nTo optimize performance : \n\n" \
                       "  - Stay in this hidden interface mode\n" \
                       "  - Minimize this window\n" \
                       "\nNote : Your settings are saved when the app quits and are loaded at startup. (autosave feature)\n" \
                       "       To load the last saved settings, use [ctrl+l] / [cmd+l] or click on the GUI icon."
                       ,20,20);
    
    ofPopStyle();
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
            
        // Other keys
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
                saveSettings();
                ofLogNotice("keyPressed", "Settings have been successfully saved ");
            }
            break;
            
        case 'l':
        case 'L':
            // CTRL+L or CMD+L
            if(m_iModifierKey == OF_KEY_CONTROL || m_iModifierKey == OF_KEY_COMMAND){
                // Load settings
                loadSettings();
                ofLogNotice("keyPressed", "Settings have been successfully loaded ");
            }
            break;
            
        #ifdef WIN32
            
        case 19:
            // CTRL+S or CMD+S
            saveSettings();
            ofLogNotice("keyPressed", "Settings have been successfully saved ");
            break;
            
        case 12:
            // CTRL+L or CMD+L
            loadSettings();
            ofLogNotice("keyPressed", "Settings have been successfully loaded ");
            break;
            
        #endif

		case 'r':
		case 'R':
			//Delete all AreaPolygons
			ofLogNotice("keyPressed", "All AreaPolygons are now deleted ");
			m_vAreaPolygonsVector.clear();
			m_bIsCreating = false;
			break;

		case 'Z':
		case 'z':
			//Delete Last AreaPolygon
			ofLogNotice("keyPressed", "Last AreaPolygon is now deleted ");
			m_vAreaPolygonsVector.pop_back();
			m_bIsCreating = false;
			break;

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
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

	if(button == 0){
		//One AreaPolygon is not finish
		if (m_bIsCreating){
			//Is closing the poly
			if (m_vAreaPolygonsVector[m_iNumberOfAreaPolygons - 1].getFirstpoint().distance(ofVec2f(x,y))<m_iRadiusClosePolyZone){
				m_vAreaPolygonsVector[m_iNumberOfAreaPolygons - 1].complete();
				m_bIsCreating = false;
			}
			//Create another point
			else{
				m_vAreaPolygonsVector[m_iNumberOfAreaPolygons - 1].addPoint(ofVec2f(x, y));
			}
		}

		//Every AreaPolygons are completed
		else{

			m_vAreaPolygonsVector.push_back(AreaPolygon(ofVec2f(x, y)));
			m_bIsCreating = true;

		}
	}
	if (button == 2){
		//One AreaPolygon is not finish
		if (m_bIsCreating){
			if (m_vAreaPolygonsVector[m_vAreaPolygonsVector.size() - 1].removeLastPoint()){}
			else{
				m_vAreaPolygonsVector.pop_back();
				m_bIsCreating = false;
			}
		}

		//Every AreaPolygons are completed
		else{

		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//_______________________________________________________________
//_____________________________SETUP_____________________________
//_______________________________________________________________

//--------------------------------------------------------------
void ofApp::setupPreferences(){
    
    /*
     
     Loading or creating preferences.xml file containing
     all settings which are not part of the GUI
     
     */
    
    ofxXmlSettings preferences;
    // If a preferences.xml file exists, load it
    if(ofFile::doesFileExist("preferences.xml")){
        preferences.load("preferences.xml");
        preferences.pushTag("Settings");
        m_bHideInterface = preferences.getValue("HideInterface", m_bHideInterface);
        m_bLogToFile = preferences.getValue("LogToFile", m_bLogToFile);
        m_iFboWidth = preferences.getValue("FboWidth", m_iFboWidth);
        m_iFboHeight = preferences.getValue("FboHeight", m_iFboHeight);
        preferences.popTag();
    }
    // Else, create a new preferences.xml file with the default values
    else{
        preferences.addTag("Settings");
        preferences.pushTag("Settings");
        preferences.addValue("HideInterface", m_bHideInterface);
        preferences.addValue("LogToFile", m_bLogToFile);
        preferences.addValue("FboWidth", m_iFboWidth);
        preferences.addValue("FboHeight", m_iFboHeight);
        preferences.popTag();
        preferences.saveFile("preferences.xml");
    }
    
    // Setup log output here because m_bLogToFile variable has just been loaded above from preferences.xml
    if(m_bLogToFile){
        ofLogToFile("../../../" +
                    ofGetTimestampString("%Y-%m-%d_%H-%M-%S") + "_" + APP_NAME + ".log");
    }
}

//--------------------------------------------------------------
void ofApp::setupGUI(){
    
    // Add listeners before setting up so the initial values are correct
    // Listeners can be used to call a function when a UI element has changed.
    // Don't forget to call ofRemoveListener before deleting any instance that is listening to an event, to prevent crashes. Here, we will call it in exit() method.
    m_bResetSettings.addListener(this, &ofApp::reset);
    
    // Setup GUI panel
    m_gui.setup();
    m_gui.setName("Preferences");
    
    // Add content to GUI panel
    m_gui.add(m_sFramerate.setup("FPS", m_sFramerate));
	m_gui.add(m_sNumberOfAreaPolygons.setup("Number of AreaPolygons", m_sNumberOfAreaPolygons));
    m_gui.add(m_bResetSettings.setup("Reset Settings", m_bResetSettings));
  
	// guiFirstGroup parameters ---------------------------
    string sFirstGroupName = "Design";
    m_guiFirstGroup.setName(sFirstGroupName);     // Name the group of parameters (important if you want to apply color to your GUI)
	m_guiFirstGroup.add((m_fPointRadius.setup("Circles Radius", m_fPointRadius, 10, 50))->getParameter());    // Setup parameter : args = name, default, min, max ; default argument value has been set in init(), so take the variable itself as argument.
	m_guiFirstGroup.add((m_iLinesWidthSlider.setup("lines Width", m_iLinesWidthSlider, 1, 10))->getParameter());    // Setup parameter : args = name, default, min, max ; default argument value has been set in init(), so take the variable itself as argument.
	m_gui.add(m_guiFirstGroup);     // When all parameters of the group are set up, add the group to the gui panel.
 
	// guiSecondGroup parameters ---------------------------
    string sSecondGroupName = "Parameters";
    m_guiSecondGroup.setName(sSecondGroupName);
	m_guiSecondGroup.add((m_bRedondanteMode.setup("Enable redondante mode", m_bRedondanteMode))->getParameter());
	m_guiSecondGroup.add((m_iRadiusClosePolyZone.setup("Closing radius", m_iRadiusClosePolyZone, 20, 50))->getParameter());    // Setup parameter : args = name, default, min, max ; default argument value has been set in init(), so take the variable itself as argument.
    m_gui.add(m_guiSecondGroup);
   
	// guiThirdGroup parameters ---------------------------
    string sThirdGroupName = "Third Group";
    m_guiThirdGroup.setName(sThirdGroupName);
    m_guiThirdGroup.add((m_vMyVec.setup("My Vector", m_vMyVec, ofVec3f(0,0,0), ofVec3f(3,3,3)))->getParameter());
    m_gui.add(m_guiThirdGroup);

    // You can add colors to your GUI groups to identify them easily
    // Example of beautiful colors you can use : salmon, orange, darkSeaGreen, teal, cornflowerBlue...
    m_gui.getGroup(sFirstGroupName).setHeaderBackgroundColor(ofColor::salmon);    // Parameter group must be get with its name defined in setupGUI()
    m_gui.getGroup(sFirstGroupName).setBorderColor(ofColor::salmon);
    m_gui.getGroup(sSecondGroupName).setHeaderBackgroundColor(ofColor::orange);
    m_gui.getGroup(sSecondGroupName).setBorderColor(ofColor::orange);
    m_gui.getGroup(sThirdGroupName).setHeaderBackgroundColor(ofColor::cornflowerBlue);
    m_gui.getGroup(sThirdGroupName).setBorderColor(ofColor::cornflowerBlue);
    
    // Load autosave settings
    if(ofFile::doesFileExist("autosave.xml")){
        m_gui.loadFromFile("autosave.xml");
    }
}

//--------------------------------------------------------------
void ofApp::setupOSC(){
    
    ofxXmlSettings settings;
    bool bNeedToSaveSettings = false;
    
    // If a file settings exists, load values saved ; else, take default values
    if(ofFile::doesFileExist("preferences.xml")){
        settings.load("preferences.xml");
        if(settings.tagExists("OSC")){
            settings.pushTag("OSC");
            m_iOscReceiverPort = settings.getValue("ReceiverPort", m_iOscReceiverPort);
            m_iOscSenderPort = settings.getValue("SenderPort", m_iOscSenderPort);
            m_sOscSenderHost = settings.getValue("SenderHost", m_sOscSenderHost);
            settings.popTag();
        } else{
            bNeedToSaveSettings = true;
        }
    } else{
        bNeedToSaveSettings = true;
    }
    
    if(bNeedToSaveSettings){
        settings.addTag("OSC");
        settings.pushTag("OSC");
        settings.setValue("ReceiverPort", m_iOscReceiverPort);
        settings.setValue("SenderPort", m_iOscSenderPort);
        settings.setValue("SenderHost", m_sOscSenderHost);
        settings.popTag();
        settings.saveFile("preferences.xml");
    }

    m_sReceiverOscDisplay = "Listening to OSC on port " + ofToString(m_iOscReceiverPort) + "\n";
    
    try{
        m_oscReceiver.setup(m_iOscReceiverPort);
    } catch (std::exception&e){
        ofLogWarning("setupOSC") << "Error : " << ofToString(e.what());
        m_sReceiverOscDisplay = "\n/!\\ ERROR : Could not bind to OSC port " + ofToString(m_iOscReceiverPort) + " !\n\n";
    }
    
    m_oscSender.setup(m_sOscSenderHost, m_iOscSenderPort);
}

//--------------------------------------------------------------
void ofApp::saveSettings(){
    
    // Save GUI parameters
    m_gui.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::loadSettings(){
    
    // Load saved settings
    if(ofFile::doesFileExist("settings.xml")){
        m_gui.loadFromFile("settings.xml");
    }
}

//_______________________________________________________________
//_____________________________OSC_______________________________
//_______________________________________________________________

//--------------------------------------------------------------
void ofApp::receiveOSC(){
    
    // Get OSC data
    while(m_oscReceiver.hasWaitingMessages()){
        
        // Get the next message
        ofxOscMessage m;
        m_oscReceiver.getNextMessage(&m);
        
        // Parse your messages here
        
        // First message example
        if(m.getAddress() == "/my/first/address" || m.getAddress() == "/my/first/address/"){    // can be useful to check both, in case you don't know exactly what is sent by your sender
            int iMyInt = m.getArgAsInt32(0);
            float fMyFloat = m.getArgAsFloat(1);
        }
        // Second message example
        else if(m.getAddress() == "/my/second/address" || m.getAddress() == "/my/second/address/"){
            string sMyString = m.getArgAsString(0);
        } else{
            
            // Unrecognized message: display a log output
            string sMsg;
            sMsg = m.getAddress();
            sMsg += " ";
            for(int i = 0; i < m.getNumArgs(); i++){
                // get the argument type
                sMsg += m.getArgTypeName(i);
                sMsg += ":";
                // display the argument - make sure we get the right type
                if(m.getArgType(i) == OFXOSC_TYPE_INT32){
                    sMsg += ofToString(m.getArgAsInt32(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
                    sMsg += ofToString(m.getArgAsFloat(i));
                }
                else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
                    sMsg += m.getArgAsString(i);
                }
                else{
                    sMsg += "unknown";
                }
                sMsg += " ";
            }
            ofLogNotice("receiveOSC"," Unknown OSC message : " + sMsg);
        }
    }
}

//--------------------------------------------------------------
void ofApp::sendOSC(){
    
    // Create a new message
    ofxOscMessage m;
    
    // Create a first dataset
    m.setAddress("/my/first/address");
    m.addFloatArg(m_vMyVec->x);
    m_oscSender.sendMessage(m); // Send it
    m.clear(); // Clear message to be able to reuse it
    
    // Create a second dataset
    m.setAddress("/my/second/address");
    m.addStringArg("I am a string flying through OSC !");
    m_oscSender.sendMessage(m); // Send it
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

	m_gui.saveToFile("autosave.xml");

	// Remove listener because instance of our gui button will be deleted
	m_bResetSettings.removeListener(this, &ofApp::reset);
}
