#include "AreaPolygon.h"

#define MOVE_STEP 0.01

AreaPolygon::AreaPolygon(ofVec2f a_oFirstPoint){
	m_oPointsColor = ofColor::lightBlue;
	m_oLinesColor = ofColor::white;
	m_oCompletedColor = ofColor::paleVioletRed;
	m_oSelectedColor = ofColor::red;
	m_iLinesWidth = 2;
	m_fRadius = 20;
	m_bIsFinished = false;
	m_iPeopleInside = 0;
	m_bSelected = false;

	addPoint(a_oFirstPoint);
}

//--------------------------------------------------------------
void AreaPolygon::addPoint(ofVec2f a_oPoint){
	m_vVectorPoints.push_back(ofVec2f(a_oPoint.x,a_oPoint.y));
}

//--------------------------------------------------------------
void AreaPolygon::drawPeopleInside(int width, int height){	
	ofDrawBitmapString(ofToString(m_iPeopleInside),ofVec2f(m_oCentroid.x * width, m_oCentroid.y * height));
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
	for (int i = 0; i < m_vVectorPoints.size(); ++i){
		if (m_vVectorPoints[i].y + MOVE_STEP < 1.0){
			m_vVectorPoints[i].y = m_vVectorPoints[i].y + MOVE_STEP;
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::moveLeft(){
	for (int i = 0; i < m_vVectorPoints.size(); ++i){
		if (m_vVectorPoints[i].x - MOVE_STEP > 0){
			m_vVectorPoints[i].x = m_vVectorPoints[i].x - MOVE_STEP;
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::moveRight(){
	for (int i = 0; i < m_vVectorPoints.size(); ++i){
		if (m_vVectorPoints[i].x + MOVE_STEP < 1.0){
			m_vVectorPoints[i].x = m_vVectorPoints[i].x + MOVE_STEP;
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::moveUp(){
	for (int i = 0; i < m_vVectorPoints.size(); ++i){
		if (m_vVectorPoints[i].y - MOVE_STEP > 0){
			m_vVectorPoints[i].y = m_vVectorPoints[i].y - MOVE_STEP;
		}
	}
}

//--------------------------------------------------------------
void AreaPolygon::move(float a_iX, float a_iY){
	for (int i = 0; i < m_vVectorPoints.size(); ++i){
		if ((m_vVectorPoints[i].x - a_iX) > 0 && (m_vVectorPoints[i].x - a_iX) < 1){
			m_vVectorPoints[i].x = m_vVectorPoints[i].x - a_iX;
		}
		if ((m_vVectorPoints[i].y - a_iY) > 0 && (m_vVectorPoints[i].y - a_iY) < 1){
			m_vVectorPoints[i].y = m_vVectorPoints[i].y - a_iY;
		}
	}
}