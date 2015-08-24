#ifndef AreaPolygon_H
#define AreaPolygon_H

#include <vector>
#include "ofVec2f.h"
#include "ofGraphics.h"



class AreaPolygon {
public:

	AreaPolygon(ofVec2f a_oFirstPoint);

	inline void setRadius(float a_fRadius){ m_fRadius = a_fRadius; };
	inline void setColorCircle(ofColor a_oCircleColor){ m_oPointsColor = a_oCircleColor; };
	inline void setColorLines(ofColor a_oLineColor){ m_oLinesColor = a_oLineColor; };
	inline void setLinesWidth(int a_iWidth){ m_iLinesWidth = a_iWidth; };
	inline void complete(){ m_bIsFinished = true; };
	inline int getSize(){ return m_vVectorPoints.size(); };
	inline ofVec2f getPoint(int a_iIndice){ return m_vVectorPoints[a_iIndice]; };

	void save();
	void load();

	void draw(int width, int height);
	
	void addPoint(ofVec2f a_oPoint);
	bool removeLastPoint();

private:
	vector<ofVec2f> m_vVectorPoints;
	bool m_bIsFinished;

	ofColor m_oPointsColor;
	ofColor m_oLinesColor;
	ofColor m_oLinesCompletedColor;
	float m_fRadius;
	int m_iLinesWidth;

};
#endif // AreaPolygon_H
