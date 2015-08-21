#include "polygon.h"

polygon::polygon(ofVec2f a_oFirstPoint){
	m_oPointsColor = ofColor::lightBlue;
	m_oLinesColor = ofColor::white;
	m_iLinesWidth = 2;
	m_fRadius = 20;
	addPoint(a_oFirstPoint);
}

//--------------------------------------------------------------
void polygon::addPoint(ofVec2f a_oPoint){
	m_vVectorPoints.push_back(a_oPoint);
}

//--------------------------------------------------------------
void polygon::draw(){
	ofPushStyle();


	if (m_bIsFinished){
		
		ofSetColor(m_oLinesColor);
		ofSetLineWidth(m_iLinesWidth);
		for (int i = 0; i < m_vVectorPoints.size() - 1; ++i){
			for (int j = 1; j < m_vVectorPoints.size(); ++j){
				ofLine(m_vVectorPoints[i], m_vVectorPoints[j]);
				++i;
			}
		}

	}
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
void polygon::removeLastPoint(){
	if (m_vVectorPoints.size()>1){
		m_vVectorPoints.pop_back();
	}
}