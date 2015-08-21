#include "Polygon.h"

AreaPolygon::AreaPolygon(ofVec2f a_oFirstPoint){
	m_oPointsColor = ofColor::lightBlue;
	m_oLinesColor = ofColor::white;
	m_oLinesCompletedColor = ofColor::paleVioletRed;
	m_iLinesWidth = 2;
	m_fRadius = 20;
	m_bIsFinished = false;
	addPoint(a_oFirstPoint);
}

//--------------------------------------------------------------
void AreaPolygon::addPoint(ofVec2f a_oPoint){
	m_vVectorPoints.push_back(a_oPoint);
}

//--------------------------------------------------------------
void AreaPolygon::draw(){
	ofPushStyle();
	ofSetLineWidth(m_iLinesWidth);

	//The shape is finished
	if (m_bIsFinished){
		//draw completed poly
		ofSetColor(m_oLinesColor);
		for (int i = 0; i < m_vVectorPoints.size() - 1; ++i){
			for (int j = 1; j < m_vVectorPoints.size(); ++j){
				ofLine(m_vVectorPoints[i], m_vVectorPoints[j]);
				++i;
			}
		}
		ofLine(m_vVectorPoints[0], m_vVectorPoints[m_vVectorPoints.size() - 1]);
		ofSetColor(m_oLinesCompletedColor);
		ofSetPolyMode(OF_POLY_WINDING_NONZERO);
		ofBeginShape();
		for (size_t i = 0; i < m_vVectorPoints.size(); i++){
			ofVertex(m_vVectorPoints[i].x, m_vVectorPoints[i].y);
		}
		ofEndShape();	
	}
	//The shape is not finished
	else{
		ofSetColor(m_oPointsColor);
		for (int i = 0; i < m_vVectorPoints.size(); ++i){
			ofCircle(m_vVectorPoints[i].x, m_vVectorPoints[i].y, m_fRadius);
		}
		ofSetColor(m_oLinesColor);
		ofSetLineWidth(m_iLinesWidth);
		for (int i = 0; i < m_vVectorPoints.size() - 1; ++i){
			for (int j = 1; j < m_vVectorPoints.size(); ++j){
				ofLine(m_vVectorPoints[i],m_vVectorPoints[j]);
				++i;
			}
		}
	}

	ofPopStyle();
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