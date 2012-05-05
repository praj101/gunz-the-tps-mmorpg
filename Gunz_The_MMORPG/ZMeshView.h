#ifndef ZMESHVIEW_H
#define ZMESHVIEW_H

#include "CCWidget.h"
#include "RMesh.h"
#include "RVisualMeshMgr.h"

#include "CCButton.h"

using namespace RealSpace2;


class RTVisualMesh {
public:
	RTVisualMesh() {
		m_pVisualMesh = new RVisualMesh;
		bInit = false;
	}

	~RTVisualMesh() {
		if(m_pVisualMesh){
			delete m_pVisualMesh;
			m_pVisualMesh = NULL;
		}
	}

	void Create() {
		bInit = true;
	}

	bool bInit;
	RVisualMesh* GetVMesh(bool b=true);
	RVisualMesh* m_pVisualMesh;
};

class ZMeshView : public CCButton{
protected:
	RTVisualMesh	m_pTVisualMesh;
	bool			m_bLook;
	float			m_fDist;
	float			m_fMinDist, m_fMaxDist;
	float			m_fCRot;
	bool			m_bEnableRotate;
	bool			m_bEnableZoom;
	rvector	m_Eye, m_At, m_Up;	// Camera
public:
	
protected:
	virtual bool OnEvent(CCEvent* pEvent, CCListener* pListener);
	void DrawTestScene(void);
	virtual void OnDraw(CCDrawContext* pDC);

	void RotateLeft(float add_degree=1.0f)			{ m_fCRot += add_degree; }
	void RotateRight(float add_degree=1.0f)			{ m_fCRot -= add_degree; }

	void RotateVertical(float add_degree=1.0f);

	void ZoomIn(float add_distance = 1.0f);
	void ZoomOut(float add_distance = 1.0f); 
public:
	ZMeshView(const char* szName=NULL, CCWidget* pParent=NULL, CCListener* pListener=NULL);
	virtual ~ZMeshView(void);

//	void SetMesh(RVisualMesh* pVisualMesh);
	void SetLight(rvector LPos);

	void SetEnableRotateZoom(bool bEnableRotate, bool bEnableZoom);

	RVisualMesh* GetVisualMesh() { 
		return m_pTVisualMesh.GetVMesh(); 
	}
	void SetLook(bool bLook) { m_bLook = bLook; }

	virtual void OnInvalidate() {};
	virtual void OnRestore() {};
	static list<ZMeshView*> msMeshViewList;
};


#endif