#pragma once
 
#define LOOK_IN_CONSTRUCTOR()	m_pCustomLook = NULL;


// Start
#define DECLARE_LOOK(CLASS_LOOK)							\
	private:												\
		static CLASS_LOOK	m_DefaultLook;					\
		static CLASS_LOOK*	m_pStaticLook;					\
		CLASS_LOOK*			m_pCustomLook;					\
	protected:												\
		virtual void OnDraw(CCDrawContext* pDC);			\
	public:													\
		static void ChangeLook(CLASS_LOOK* pLook);			\
		void ChangeCustomLook(CLASS_LOOK* pLook);			\
		CLASS_LOOK* GetLook(){								\
			if(m_pCustomLook!=NULL) return m_pCustomLook;	\
			return m_pStaticLook;							\
		}													
// End

// Start
#define DECLARE_LOOK_CLIENT()								\
	public:													\
		virtual sRect GetClientRect(void){					\
		if(GetLook()==NULL)									\
			return CCWidget::GetClientRect();				\
		return GetLook()->GetClientRect(this,				\
			CCWidget::GetClientRect());						\
	}
// End

// Start
#define IMPLEMENT_LOOK(CLASS, CLASS_LOOK)						\
	CLASS_LOOK CLASS::m_DefaultLook;							\
	CLASS_LOOK* CLASS::m_pStaticLook = &CLASS::m_DefaultLook;	\
	void CLASS::OnDraw(CCDrawContext* pDC){						\
		if(GetLook()!=NULL) GetLook()->OnDraw(this, pDC);		\
	}															\
	void CLASS::ChangeLook(CLASS_LOOK* pLook){					\
		if(pLook==NULL){										\
			m_pStaticLook = &m_DefaultLook;						\
			return;												\
		}														\
		m_pStaticLook = pLook;									\
	}															\
	void CLASS::ChangeCustomLook(CLASS_LOOK* pLook){			\
		m_pCustomLook = pLook;									\
	}
// End

