#ifndef AreaPolygon_H
#define AreaPolygon_H

#include <vector>
#include "ofVec2f.h"
#include "ofGraphics.h"
#include "ofxAugmenta.h"



class AreaPolygon {
public:

	AreaPolygon(ofVec2f a_oFirstPoint);

	inline void setRadius(float a_fRadius){ m_fRadius = a_fRadius; };
	inline void setColorCircle(ofColor a_oCircleColor){ m_oPointsColor = a_oCircleColor; };
	inline void setColorLines(ofColor a_oLineColor){ m_oLinesColor = a_oLineColor; };
	inline void setLinesWidth(int a_iWidth){ m_iLinesWidth = a_iWidth; };
	inline bool isCompleted(){ return m_bIsFinished; };
	inline int getSize(){ return m_vVectorPoints.size(); };
	inline ofVec2f getPoint(int a_iIndice){ return m_vVectorPoints[a_iIndice]; };
	inline void hasBeenSelected(bool a_bSelected){ m_bSelected = a_bSelected; };

	void save();
	void load();

	void draw(int width, int height); 
	void drawPeopleInside(int width, int height);
	void setPeopleInside(vector<Augmenta::Person*> a_vPeople);
	void setPolygonCentroid();
	void complete();
	void addPoint(ofVec2f a_oPoint);
	bool removeLastPoint();
	bool isPointInPolygon(ofVec2f a_oPersonPosition);
	
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void move(float a_iX, float a_iY);


private:
	vector<ofVec2f> m_vVectorPoints;
	bool m_bIsFinished;
	int m_iPeopleInside;
	bool m_bSelected;
	ofVec2f m_oCentroid;

	ofColor m_oPointsColor;
	ofColor m_oLinesColor;
	ofColor m_oCompletedColor;
	ofColor m_oSelectedColor;
	float m_fRadius;
	int m_iLinesWidth;

};
#endif // AreaPolygon_H
