#include "AreaPolygon.h"
#include "ofUtils.h"


#define MOVE_STEP 0.001

AreaPolygon::AreaPolygon(ofVec2f a_oFirstPoint, vector<Augmenta::Person*> a_vPeople, int a_iIndiceInPolygonsVector){
	m_oPointsColor = ofColor::lightBlue;
	m_oLinesColor = ofColor::white;
	m_oCompletedColor = ofColor::paleVioletRed;
	m_oSelectedColor = ofColor::red;
	m_oNotEmptyColor = ofColor::forestGreen;
	m_iLinesWidth = 2;
	m_fRadius = 20;
	m_bIsFinished = false;
	setPeopleInside(a_vPeople);
	m_iOldPeopleInside=m_iPeopleInside;
	m_bSelected = false;
	m_iIdInPolygonsVector = a_iIndiceInPolygonsVector;
	m_sInOsc = "/area" + ofToString(m_iIdInPolygonsVector) + "/personEntered";
	m_sOutOsc = "/area" + ofToString(m_iIdInPolygonsVector) + "/personWillLeave";

	addPoint(a_oFirstPoint);
}

//--------------------------------------------------------------
void AreaPolygon::loadOscMessage(string m_aInOsc, string m_aOutOsc){
	m_sInOsc = m_aInOsc;
	m_sOutOsc = m_aOutOsc;
}

//--------------------------------------------------------------
void AreaPolygon::addPoint(ofVec2f a_oPoint){
	m_vVectorPoints.push_back(ofVec2f(a_oPoint.x,a_oPoint.y));
}

//--------------------------------------------------------------
void AreaPolygon::drawPeopleInside(int width, int height){	
	ofDrawBitmapString("In Message = " + m_sInOsc, ofVec2f(m_oCentroid.x * width, m_oCentroid.y * height - 20));
	ofDrawBitmapString("Out Message = " + m_sOutOsc, ofVec2f(m_oCentroid.x * width, m_oCentroid.y * height - 10));
	ofDrawBitmapString("People inside = " + ofToString(m_iPeopleInside),ofVec2f(m_oCentroid.x * width, m_oCentroid.y * height));
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
	ofSetLineWidth(m_iLinesWidth);

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
				
		if (m_bSelected){
			ofSetColor(m_oSelectedColor);
		}
		else if (m_iPeopleInside > 0){
			ofSetColor(m_oNotEmptyColor);
		}
		else{
			ofSetColor(m_oCompletedColor);
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