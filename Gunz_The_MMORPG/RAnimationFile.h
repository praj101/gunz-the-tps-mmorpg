#pragma once

#include "RAnimationNode.h"
_NAMESPACE_REALSPACE2_BEGIN

class RAnimationFile : public RBaseObject
{
public:
	RAnimationFile();
	~RAnimationFile();

	bool LoadAni(char* filename);

	RAnimationNode* GetNode(char* name);

	void AddRef();
	void DecRef();

public:

	RAnimationNode**	m_ani_node;
	int					m_ani_node_cnt;
	AnimationType		m_ani_type;
	AnimationFileType	m_ani_file_type;

	RAnimationNode*		m_pBipRootNode;

	int		m_max_frame;
	int		m_iRefCount;
};

_NAMESPACE_REALSPACE2_END