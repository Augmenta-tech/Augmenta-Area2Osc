#include "AreaPolygon.h"
#include "ofUtils.h"
#include <algorithm>
#include <locale>


AreaPolygon::AreaPolygon(ofVec2f a_oFirstPoint, vector<Augmenta::Person*> a_vPeople, int a_iIndice, int m_iAntiBounce){
	m_oPointsColor = ofColor(246,128,248,200);
	m_oLinesColor = ofColor(237,232,229,200);
	m_oCompletedColor = ofColor(255, 166, 70,200);
	m_oLinesCompletedColor = ofColor(243,80,64,200);
	m_oNotEmptyColor = ofColor(243,80,64,200);
	m_iLinesWidth = 3;
	m_fRadius = 5;
	m_bIsFinished = false;
	setPeopleInside(a_vPeople, m_iAntiBounce);
	m_iOldPeopleInside=m_iPeopleInside;
	m_bSelected = false;
	m_fMoveIncremente = 0.001;
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
bool AreaPolygon::doesStringContainOnlyNumber(string a_sString){
		std::locale loc;
	for (std::string::iterator it = a_sString.begin(); it != a_sString.end(); ++it)
	{
		if (!(!std::isalpha(*it, loc) && std::isalnum(*it, loc))){
			return false;
		}
	}
	return true;
}

//--------------------------------------------------------------
bool AreaPolygon::containOnlyAlpha(string a_sString){
	std::locale loc;
	for (std::string::iterator it = a_sString.begin(); it != a_sString.end(); ++it)
	{
		if (!std::isalpha(*it, loc)){
			return false;
		}	
	}
return true;
}

//--------------------------------------------------------------
bool AreaPolygon::doesStringContainOnlyNumberAndOnePoint(string a_sString){
	std::locale loc;
	for (std::string::iterator it = a_sString.begin(); it != a_sString.end(); ++it)
	{
		if (!((!std::isalpha(*it, loc) && std::isalnum(*it,loc)) || *it == '.')){
			return false;
		}
	}
	if (std::count(a_sString.begin(), a_sString.end(), '.') != 1){
		return false;
	}
	return true;
}

//--------------------------------------------------------------
void AreaPolygon::loadOscMessage(string m_aInOsc, string m_aOutOsc){
	m_vInOsc = ofSplitString(m_aInOsc, " ");
	for (int i = 1; i < m_vInOsc.size(); i++){
		if (containOnlyAlpha(m_vInOsc[i])){
			// is a string 
			m_oOscMessageIn.addStringArg(m_vInOsc[i]);
		}
		else if (doesStringContainOnlyNumber(m_vInOsc[i])){
			//is a int 
			m_oOscMessageIn.addIntArg(ofToInt(m_vInOsc[i]));
			}
		else if (doesStringContainOnlyNumberAndOnePoint(m_vInOsc[i])){
			//is a float
			m_oOscMessageIn.addFloatArg(ofToFloat(m_vInOsc[i]));
		}
		else{
			//Unknow type send as string
			ofLogError("m_oOscMessageIn loaded an unknow variable as string");
			m_oOscMessageIn.addStringArg(m_vInOsc[i]);
		}
	}

	m_vOutOsc = ofSplitString(m_aOutOsc, " ");
	for (int i = 1; i < m_vOutOsc.size(); i++){
		if (containOnlyAlpha(m_vOutOsc[i])){
			// is a string 
			m_oOscMessageOut.addStringArg(m_vOutOsc[i]);
		}
		else if (doesStringContainOnlyNumber(m_vOutOsc[i])){
			//is a int 
			m_oOscMessageOut.addIntArg(ofToInt(m_vOutOsc[i]));
		}
		else if (doesStringContainOnlyNumberAndOnePoint(m_vOutOsc[i])){
			//is a float
			m_oOscMessageOut.addFloatArg(ofToFloat(m_vOutOsc[i]));
		}
		else{
			//Unknow type send as string
			ofLogError("m_oOscMessageOut loaded an unknow variable as string");
			m_oOscMessageOut.addStringArg(m_vOutOsc[i]);
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::addPoint(ofVec2f a_oPoint){
	m_vVectorPoints.push_back(ofVec2f(a_oPoint.x,a_oPoint.y));
}

//--------------------------------------------------------------
void AreaPolygon::drawPeopleInside(int width, int height){
	if (m_bIsFinished){
		ofDrawBitmapString("[in] " + m_vInOsc[0], ofVec2f(m_oCentroid.x * width, m_oCentroid.y * height - 20));
		ofDrawBitmapString("[out] " + m_vOutOsc[0], ofVec2f(m_oCentroid.x * width, m_oCentroid.y * height - 10));
		ofDrawBitmapString("people : " + ofToString(m_iPeopleInside), ofVec2f(m_oCentroid.x * width, m_oCentroid.y * height));
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
				ofDrawLine(m_vVectorPoints[i].x * width, m_vVectorPoints[i].y * height, m_vVectorPoints[j].x * width, m_vVectorPoints[j].y * height);
				++i;
			}
		}
		ofDrawLine(m_vVectorPoints[0].x * width, m_vVectorPoints[0].y * height, m_vVectorPoints[m_vVectorPoints.size() - 1].x * width, m_vVectorPoints[m_vVectorPoints.size() - 1].y * height);
			
		
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
			ofDrawCircle(m_vVectorPoints[i].x * width, m_vVectorPoints[i].y * height, m_fRadius);
		}
		ofSetColor(m_oLinesColor);
		ofSetLineWidth(m_iLinesWidth);
		for (int i = 0; i < m_vVectorPoints.size() - 1; ++i){
			for (int j = 1; j < m_vVectorPoints.size(); ++j){
				ofDrawLine(m_vVectorPoints[i].x * width, m_vVectorPoints[i].y * height, m_vVectorPoints[j].x * width, m_vVectorPoints[j].y * height);
				++i;
			}
		}
	}
	ofPopStyle();
	drawPeopleInside(width,height);
}

//--------------------------------------------------------------
void AreaPolygon::setPeopleInside(vector<Augmenta::Person*> people, int a_iBounceIntervalTime){
	ofPoint centroid;
	unsigned long long currentTime = ofGetElapsedTimeMillis();
	vector<int> peopleInsideRightNow;
	m_iPeopleInside = 0;

	//Getting id of the persons whose inside
	for (int i = 0; i < people.size(); i++){
		if (isPointInPolygon(people[i]->centroid)){
			peopleInsideRightNow.push_back(people[i]->pid);
		}
	}

	//Add WaitingToLeave
	for (int j = 0; j < m_vIdPeopleInside.size(); j++){
		if (std::find(peopleInsideRightNow.begin(), peopleInsideRightNow.end(), m_vIdPeopleInside[j]) == peopleInsideRightNow.end()){
			m_vWaitingToLeave.push_back(LeavingPerson(currentTime, m_vIdPeopleInside[j]));
		}
	}

	//Suppression of doublons in waitingToleave
	for (int j = 0; j < peopleInsideRightNow.size(); ++j){
		for (int i = 0; i < m_vWaitingToLeave.size(); ++i){
			if (m_vWaitingToLeave[i].getId() == peopleInsideRightNow[j]){
				m_vWaitingToLeave.erase(m_vWaitingToLeave.begin() + i);
				i--;
			}
		}
	}

	//Update of the people in waiting to leave list
	for (int i = 0; i < m_vWaitingToLeave.size(); ++i){
		if ((currentTime - m_vWaitingToLeave[i].getLeavingTime()) > a_iBounceIntervalTime){
			m_vWaitingToLeave.erase(m_vWaitingToLeave.begin() + i);
		}
	}

	//m_iPeopleInside = personns inside right now + people on the waiting to leave list 
	m_iPeopleInside = m_vWaitingToLeave.size() + peopleInsideRightNow.size();

	m_vIdPeopleInside.clear();
	m_vIdPeopleInside.swap(peopleInsideRightNow);

	/*	
	std::cout << " people inside = " << m_iPeopleInside << std::endl;
	std::cout << "m_vIdPeopleInside size = " << m_vIdPeopleInside.size() << std::endl;
	std::cout << "m_vWaitingToLeave size = " << m_vWaitingToLeave.size() << std::endl;
	*/
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
		if (((m_vVectorPoints[i].y< a_oPersonPosition.y && m_vVectorPoints[j].y >= a_oPersonPosition.y)
			|| (m_vVectorPoints[j].y< a_oPersonPosition.y && m_vVectorPoints[i].y >= a_oPersonPosition.y))
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
		if (m_vVectorPoints[i].y + m_fMoveIncremente > 1.0){
			isMovePossible = false;
		}
	}
	if (isMovePossible){
		for (int i = 0; i < m_vVectorPoints.size(); ++i){
			m_vVectorPoints[i].y = m_vVectorPoints[i].y + m_fMoveIncremente;
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::moveLeft(){
	bool isMovePossible= true;
	for (int i = 0; i < m_vVectorPoints.size(); ++i){
		if (m_vVectorPoints[i].x - m_fMoveIncremente < 0){
			isMovePossible = false;
		}
	}
	if (isMovePossible){
		for (int i = 0; i < m_vVectorPoints.size(); ++i){
			m_vVectorPoints[i].x = m_vVectorPoints[i].x - m_fMoveIncremente;
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::moveRight(){
	bool isMovePossible = true;
	for (int i = 0; i < m_vVectorPoints.size(); ++i){
		if (m_vVectorPoints[i].x + m_fMoveIncremente > 1.0){
			isMovePossible = false;
		}
	}
	if (isMovePossible){
		for (int i = 0; i < m_vVectorPoints.size(); ++i){
			m_vVectorPoints[i].x = m_vVectorPoints[i].x + m_fMoveIncremente;
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::moveUp(){
	bool isMovePossible = true;
	for (int i = 0; i < m_vVectorPoints.size(); ++i){
		if (m_vVectorPoints[i].y - m_fMoveIncremente < 0){
			isMovePossible = false;
		}
	}
	if (isMovePossible){
		for (int i = 0; i < m_vVectorPoints.size(); ++i){
			m_vVectorPoints[i].y = m_vVectorPoints[i].y - m_fMoveIncremente;
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
void AreaPolygon::update(vector<Augmenta::Person*> a_vPeople, int a_iBounceIntervalTime){

    setPeopleInside(a_vPeople, a_iBounceIntervalTime);
    m_iPeopleMovement= m_iPeopleInside - m_iOldPeopleInside;
    m_iOldPeopleInside = m_iPeopleInside;

}

