#include "AreaPolygon.h"
#include "ofUtils.h"


#define MOVE_STEP 0.001
#define LINES_WIDTH 3
#define CIRCLE_RADIUS 5


AreaPolygon::AreaPolygon(ofVec2f a_oFirstPoint, vector<Augmenta::Person*> a_vPeople,int a_iIndice){
	m_oPointsColor = ofColor(246,128,248);
	m_oLinesColor = ofColor(237,232,229);
	m_oCompletedColor = ofColor(255, 166, 70);
	m_oLinesCompletedColor = ofColor(243,80,64);
	m_oNotEmptyColor = ofColor(243,80,64);
	m_iLinesWidth = LINES_WIDTH;
	m_fRadius = CIRCLE_RADIUS;
	m_bIsFinished = false;
	setPeopleInside(a_vPeople);
	m_iOldPeopleInside=m_iPeopleInside;
	m_bSelected = false;
	m_vInOsc.push_back("/area" + ofToString(a_iIndice) + "/personEntered");
	m_vOutOsc.push_back("/area" + ofToString(a_iIndice) + "/personWillLeave");

	addPoint(a_oFirstPoint);
}

//--------------------------------------------------------------
string AreaPolygon::getInOscAll(){
	string tempString = m_vInOsc[0];
	for (int i = 1; i < m_vInOsc.size(); ++i){
		tempString = tempString + " " + m_vInOsc[i];
	}
	return tempString;
}

//--------------------------------------------------------------
string AreaPolygon::getOutOscAll(){
	string tempString = m_vOutOsc[0];
	for (int i = 1; i < m_vOutOsc.size(); ++i){
		tempString = tempString + " " + m_vOutOsc[i];
	}
	return tempString;
}

//--------------------------------------------------------------
bool AreaPolygon::doesStringContainNumber(string a_sString){
	for (int i = 0; i < 10; i++){
		if (a_sString.find(ofToString(i)) != string::npos){
			return true;
		}
	}
	return false;;
}

//--------------------------------------------------------------
void AreaPolygon::loadOscMessage(string m_aInOsc, string m_aOutOsc){
	m_vInOsc = ofSplitString(m_aInOsc, " ");
	for (int i = 1; i < m_vInOsc.size(); i++){
		if (m_vInOsc[i].find(".") != string::npos){
			//is a float
			m_oOscMessageIn.addFloatArg(ofToFloat(m_vInOsc[i]));
		}
		else if (doesStringContainNumber(m_vInOsc[i])){
			// is a int 
			m_oOscMessageIn.addIntArg(ofToInt(m_vInOsc[i]));
		}
		else{
			// is a string 
			m_oOscMessageIn.addStringArg(m_vInOsc[i]);
		}
	}

	m_vOutOsc = ofSplitString(m_aOutOsc, " ");
	for (int i = 1; i < m_vOutOsc.size(); i++){
		if (m_vOutOsc[i].find(".") != string::npos){
			//is a float
			m_oOscMessageOut.addFloatArg(ofToFloat(m_vOutOsc[i]));
		}
		else if (doesStringContainNumber(m_vOutOsc[i])){
			// is a int 
			m_oOscMessageOut.addIntArg(ofToInt(m_vOutOsc[i]));
		}
		else{
			// is a string 
			m_oOscMessageOut.addStringArg(m_vOutOsc[i]);
		}
	}

	//m_sInOsc = m_aInOsc;
	//m_sOutOsc = m_aOutOsc;
}

//--------------------------------------------------------------
void AreaPolygon::addPoint(ofVec2f a_oPoint){
	m_vVectorPoints.push_back(ofVec2f(a_oPoint.x,a_oPoint.y));
}

//--------------------------------------------------------------
void AreaPolygon::drawPeopleInside(int width, int height){
	if (m_bIsFinished){
		ofDrawBitmapString("In :" + m_vInOsc[0], ofVec2f(m_oCentroid.x * width, m_oCentroid.y * height - 20));
		ofDrawBitmapString("Out :" + m_vOutOsc[0], ofVec2f(m_oCentroid.x * width, m_oCentroid.y * height - 10));
		ofDrawBitmapString("People :" + ofToString(m_iPeopleInside), ofVec2f(m_oCentroid.x * width, m_oCentroid.y * height));
	}
}

//--------------------------------------------------------------
void AreaPolygon::drawPeopleMovement(int width, int height){
	if (m_iPeopleMovement != 0 && m_bIsFinished){
		if (m_iPeopleMovement > 0){
			ofLogVerbose(ofToString(m_iPeopleMovement) + " people just entered");
		}
		else{
			ofLogVerbose(ofToString(abs(m_iPeopleMovement)) + " people just left");
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::draw(int width,int height){
	ofPushStyle();
	int brightness = 150;

	//The shape is finished
	if (m_bIsFinished){
		//draw completed poly
		ofSetColor(m_oLinesColor);
		for (int i = 0; i < m_vVectorPoints.size() - 1; ++i){
			for (int j = 1; j < m_vVectorPoints.size(); ++j){
				ofLine(m_vVectorPoints[i].x * width, m_vVectorPoints[i].y * height, m_vVectorPoints[j].x * width, m_vVectorPoints[j].y * height);
				++i;
			}
		}
		ofLine(m_vVectorPoints[0].x * width, m_vVectorPoints[0].y * height, m_vVectorPoints[m_vVectorPoints.size() - 1].x * width, m_vVectorPoints[m_vVectorPoints.size() - 1].y * height);
			
		
		 if (m_iPeopleInside > 0){
			 if (m_bSelected){
				 ofColor tempColor = m_oNotEmptyColor;
				 tempColor.setBrightness(brightness);
				 ofSetColor(tempColor);
			 }
			 else{
				 ofSetColor(m_oNotEmptyColor);
			 }
		}
		else{
			if (m_bSelected){
				ofColor tempColor = m_oCompletedColor;
				tempColor.setBrightness(brightness);
				ofSetColor(tempColor);
			}
			else{
				ofSetColor(m_oCompletedColor);
			}
		}
		ofSetPolyMode(OF_POLY_WINDING_NONZERO);
		ofBeginShape();
		for (size_t i = 0; i < m_vVectorPoints.size(); i++){
			ofVertex(m_vVectorPoints[i].x*width, m_vVectorPoints[i].y*height);
		}
		ofEndShape();	
	}
	//The shape is not finished
	else{
		ofSetColor(m_oPointsColor);
		for (int i = 0; i < m_vVectorPoints.size(); ++i){
			ofCircle(m_vVectorPoints[i].x * width, m_vVectorPoints[i].y * height, m_fRadius);
		}
		ofSetColor(m_oLinesColor);
		ofSetLineWidth(m_iLinesWidth);
		for (int i = 0; i < m_vVectorPoints.size() - 1; ++i){
			for (int j = 1; j < m_vVectorPoints.size(); ++j){
				ofLine(m_vVectorPoints[i].x * width, m_vVectorPoints[i].y * height, m_vVectorPoints[j].x * width, m_vVectorPoints[j].y * height);
				++i;
			}
		}
	}
	ofPopStyle();
	drawPeopleInside(width,height);
}

//--------------------------------------------------------------
void AreaPolygon::setPeopleInside(vector<Augmenta::Person*> people){
	ofPoint centroid;
	m_iPeopleInside = 0;
	for (int i = 0; i < people.size(); i++){
		if (isPointInPolygon(people[i]->centroid)){
			m_iPeopleInside++;
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::complete(){
	m_bIsFinished = true; 
	setPolygonCentroid();
}

//--------------------------------------------------------------
void AreaPolygon::setPolygonCentroid(){
	ofVec2f centroid(0, 0);
	for (int i = 0; i < m_vVectorPoints.size(); i++){
		centroid += m_vVectorPoints[i];
	}
	centroid /= m_vVectorPoints.size();
	m_oCentroid = centroid;
}

//--------------------------------------------------------------
bool AreaPolygon::isPointInPolygon(ofVec2f a_oPersonPosition) {
	bool  result = false;
	int   i, j = m_vVectorPoints.size() - 1;
	

	for (i = 0; i<m_vVectorPoints.size(); i++) {
		if ((m_vVectorPoints[i].y< a_oPersonPosition.y && m_vVectorPoints[j].y >= a_oPersonPosition.y
			|| m_vVectorPoints[j].y< a_oPersonPosition.y && m_vVectorPoints[i].y >= a_oPersonPosition.y)
			&& (m_vVectorPoints[i].x <= a_oPersonPosition.x || m_vVectorPoints[j].x <= a_oPersonPosition.x)) {
			if (m_vVectorPoints[i].x + (a_oPersonPosition.y - m_vVectorPoints[i].y) / (m_vVectorPoints[j].y - m_vVectorPoints[i].y)*(m_vVectorPoints[j].x - m_vVectorPoints[i].x)<a_oPersonPosition.x) {
				result = !result;
			}
		}
		j = i;
	}

	return result;
}

//--------------------------------------------------------------
bool AreaPolygon::removeLastPoint(){
	if (m_vVectorPoints.size()>1){
		m_vVectorPoints.pop_back();
		return true;
	}
	else {
		return false;
	}
}

//--------------------------------------------------------------
void AreaPolygon::moveDown(){
	bool isMovePossible = true;
	for (int i = 0; i < m_vVectorPoints.size(); ++i){
		if (m_vVectorPoints[i].y + MOVE_STEP > 1.0){
			isMovePossible = false;
		}
	}
	if (isMovePossible){
		for (int i = 0; i < m_vVectorPoints.size(); ++i){
			m_vVectorPoints[i].y = m_vVectorPoints[i].y + MOVE_STEP;
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::moveLeft(){
	bool isMovePossible= true;
	for (int i = 0; i < m_vVectorPoints.size(); ++i){
		if (m_vVectorPoints[i].x - MOVE_STEP < 0){
			isMovePossible = false;
		}
	}
	if (isMovePossible){
		for (int i = 0; i < m_vVectorPoints.size(); ++i){
			m_vVectorPoints[i].x = m_vVectorPoints[i].x - MOVE_STEP;
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::moveRight(){
	bool isMovePossible = true;
	for (int i = 0; i < m_vVectorPoints.size(); ++i){
		if (m_vVectorPoints[i].x + MOVE_STEP > 1.0){
			isMovePossible = false;
		}
	}
	if (isMovePossible){
		for (int i = 0; i < m_vVectorPoints.size(); ++i){
			m_vVectorPoints[i].x = m_vVectorPoints[i].x + MOVE_STEP;
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::moveUp(){
	bool isMovePossible = true;
	for (int i = 0; i < m_vVectorPoints.size(); ++i){
		if (m_vVectorPoints[i].y - MOVE_STEP < 0){
			isMovePossible = false;
		}
	}
	if (isMovePossible){
		for (int i = 0; i < m_vVectorPoints.size(); ++i){
			m_vVectorPoints[i].y = m_vVectorPoints[i].y - MOVE_STEP;
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::move(float a_iX, float a_iY){
	bool isXMovePossible = true;
	bool isYMovePossible = true;
	
	for (int i = 0; i < m_vVectorPoints.size(); ++i){
		if ((m_vVectorPoints[i].x - a_iX) < 0 || (m_vVectorPoints[i].x - a_iX) > 1){
			isXMovePossible = false;
		}
		if ((m_vVectorPoints[i].y - a_iY) < 0 || (m_vVectorPoints[i].y - a_iY) > 1){
			isYMovePossible = false;
		}
	}
	if (isXMovePossible){
		for (int i = 0; i < m_vVectorPoints.size(); ++i){
			m_vVectorPoints[i].x = m_vVectorPoints[i].x - a_iX;
		}
	}
	if (isYMovePossible){
		for (int i = 0; i < m_vVectorPoints.size(); ++i){
			m_vVectorPoints[i].y = m_vVectorPoints[i].y - a_iY;
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::update(vector<Augmenta::Person*> a_vPeople){

setPeopleInside(a_vPeople);
m_iPeopleMovement= m_iPeopleInside - m_iOldPeopleInside;
m_iOldPeopleInside = m_iPeopleInside;

}