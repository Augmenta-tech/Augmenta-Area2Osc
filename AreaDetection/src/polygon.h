#include <vector>
#include "ofVec2f.h"
#include "ofGraphics.h"



class polygon {

public:

	polygon(ofVec2f a_oFirstPoint);

	inline void setRadius(float a_fRadius){ m_fRadius = a_fRadius; };
	inline void setColorCircle(ofColor a_oCircleColor){ m_oPointsColor = a_oCircleColor; };
	inline void setColorLines(ofColor a_oLineColor){ m_oLinesColor = a_oLineColor; };
	inline void setLinesWidth(int a_iWidth){ m_iLinesWidth = a_iWidth; };
	inline ofVec2f getFirstpoint(){ return m_vVectorPoints[0]; };
	inline void complete(){ m_bIsFinished = true; };

	void save();
	void load();

	void draw();
	
	void addPoint(ofVec2f a_oPoint);
	void removeLastPoint();

private:
	vector<ofVec2f> m_vVectorPoints;
	bool m_bIsFinished;

	ofColor m_oPointsColor;
	ofColor m_oLinesColor;
	float m_fRadius;
	int m_iLinesWidth;

};