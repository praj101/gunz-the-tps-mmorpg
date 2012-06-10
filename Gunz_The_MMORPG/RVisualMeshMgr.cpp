#include "stdafx.h"
#include "RMeshMgr.h"
#include "RVisualMeshMgr.h"
#include "CCDebug.h"
#include "RealSpace2.h"

#include "CCProfiler.h"
#include "RBspObject.h"

_USING_NAMESPACE_REALSPACE2

_NAMESPACE_REALSPACE2_BEGIN

//////////////////////////////////////////////////////////

RVisualMeshMgr::RVisualMeshMgr() {

	m_id_last = 0;
	m_node_table.reserve(MAX_VMESH_TABLE);//기본

	for(int i=0;i<MAX_VMESH_TABLE;i++)
		m_node_table[i] = NULL;

}

RVisualMeshMgr::~RVisualMeshMgr() {
	DelAll();
}

int RVisualMeshMgr::Add(RMesh* pMesh)
{
	if(!pMesh) {
		cclog("VisualMesh Create failure (pMesh==NULL) !!!\n");
		return -1;
	}

	RVisualMesh* node;
	node = new RVisualMesh;

	if (!node->Create(pMesh)) {
		cclog("VisualMesh Create failure !!!\n");
		return -1;
	}

//	m_node_table[m_id_last] = node;
	m_node_table.push_back(node);
	node->m_id = m_id_last;

	m_list.push_back(node);
	m_id_last++;
	return m_id_last-1;
}

int RVisualMeshMgr::Add(RVisualMesh* node)
{
	if(!node) {
		cclog("VisualMesh Create failure (pMesh==NULL) !!!\n");
		return -1;
	}

//	m_node_table[m_id_last] = node;
	m_node_table.push_back(node);
	node->m_id = m_id_last;

	m_list.push_back(node);
	m_id_last++;
	return m_id_last-1;
}

void RVisualMeshMgr::Del(int id) {

	if(m_list.empty()) return;

	r_vmesh_node node;

	for(node = m_list.begin(); node != m_list.end();) {
		if((*node)->m_id == id) {
			delete (*node);
			node = m_list.erase(node);
		}
		else ++node;
	}
}

void RVisualMeshMgr::Del(RVisualMesh* pVMesh) {

	if(m_list.empty()) return;

	r_vmesh_node node;

	for(node = m_list.begin(); node != m_list.end();) {
		if((*node) == pVMesh) {
			delete (*node);
			node = m_list.erase(node);
		}
		else ++node;
	}

}

void RVisualMeshMgr::DelAll() {

	if(m_list.empty()) return;

	r_vmesh_node node;

	for(node = m_list.begin(); node != m_list.end(); ) {
		delete (*node);
		node = m_list.erase(node);// ++node
	}

	m_node_table.clear();//버퍼는 남아 있다..재 사용하기 위해 delall 호출했다고 본다..

	m_id_last = 0;
}

void RVisualMeshMgr::Render() {

	if(m_list.empty()) return;

	r_vmesh_node node;
	for(node = m_list.begin(); node != m_list.end();  ++node) {
		(*node)->Render();
	}
}

void RVisualMeshMgr::Render(int id) {

	if(m_list.empty()) return;

	r_vmesh_node node;
	for(node = m_list.begin(); node != m_list.end();) {
		if((*node)->m_id == id) {
			(*node)->Render();
			return;
		}
		else ++node;
	}
}

void RVisualMeshMgr::RenderFast(int id) {

	if(id == -1) return;
	_ASSERT(m_node_table[id]);
	m_node_table[id]->Render();
}

void RVisualMeshMgr::Frame() {

	if(m_list.empty()) return;

	r_vmesh_node node;
	for(node = m_list.begin(); node != m_list.end(); ++node) {
		(*node)->Frame();
	}
}

void RVisualMeshMgr::Frame(int id) {

	if(m_list.empty()) return;

	r_vmesh_node node;
	for(node = m_list.begin(); node != m_list.end();) {
		if((*node)->m_id == id) {
			(*node)->Frame();
			return;
		}
		else ++node;
	}
}

RVisualMesh* RVisualMeshMgr::GetFast(int id) {
//	if(id == -1) return NULL;
	if(id < 0)			return NULL;
	if(id > m_id_last)	return NULL;
	return m_node_table[id];
}

_NAMESPACE_REALSPACE2_END
