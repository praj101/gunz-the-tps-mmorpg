#include "stdafx.h"

#include "RCharCloth.h"
#include "RSphere.h"
#include "RCylinder.h"
#include "CCDebug.h"
#include "Realspace2.h"
#include "CCProfiler.h"
#include "RShaderMgr.h"
#include "DirectX/D3D9Types.h"
#include "RVisualMesh.h"

#define NUM_SPHERE_BET_BONE		3
#define NUM_MAX_COAT_VERTEX		6000

RVertex gVertices[NUM_MAX_COAT_VERTEX];
LPDIRECT3DVERTEXBUFFER9 gpClothVertexBuffer;
int gRef = 0;
#define COLLISION_DISTANCE 1800
#define VALET_DISTANCE			2200
#define CAL_LENGTH_DISTANCE	2500

namespace
{
	bool bHarewareBuffer = true;
}

bool IsIntersect( rvector& o, rvector& d, rvector& dir, rvector& c, float r, rvector& normal, rvector* intersect )
{		
	rvector ddir = c-d;
	float d_sq = D3DXVec3LengthSq(&ddir);
	float r_sq = r * r;
	if( d_sq > r_sq ) return false;	// 최종 목적지가 원의 바깥쪽이면 상관없음

	rvector ldir = c-o;
	float s = D3DXVec3Dot(&ldir, &dir);
	float l_sq = D3DXVec3LengthSq( &ldir );

	float m_sq = l_sq - s*s;
	if( m_sq > r_sq ) 
		return false; // 이건 충돌이 아님... 이런 경우가 왜 생길까?

	float q = sqrt( r_sq - m_sq );

	float t;
	if( D3DXVec3Dot( &normal, &dir) < 0 )  t = s-q;
	else t = s + q;
	*intersect = o + dir*t;

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 생성자
//////////////////////////////////////////////////////////////////////////
RCharCloth::RCharCloth()
{
	mForceField = rvector( 0,0,0 );
	mInitParticle = true;
	mUpdateStatus	= ALL;
	m_pVertexBuffer	= 0;
	mTime			= 0;
	m_iCntC			= 0;
	mpInitNormal	= 0;
	++gRef;	
}


//////////////////////////////////////////////////////////////////////////
// 소멸자
//////////////////////////////////////////////////////////////////////////
RCharCloth::~RCharCloth()
{
//	SAFE_RELEASE(mIndexBuffer);
	SAFE_DELETE_ARRAY( mpInitNormal );
	--gRef;
	if( gRef == 0 ) {
		cclog("Cloth VertexBuffer is Released\n" );
		SAFE_RELEASE( gpClothVertexBuffer );
	}

//	if(mpMeshNode)
//		mpMeshNode->m_bClothMeshNodeSkip = true;
}


//////////////////////////////////////////////////////////////////////////
// Initialize for Gunz Coat
//////////////////////////////////////////////////////////////////////////
bool RCharCloth::initialize( )
{
	int i;

	for( i = 0; i< NUM_CLOTH_BONE; i++)
		mBips[i]	= NULL;

	for( i = 0 ; i < mpMesh->m_data_num; ++i )
	{
		RMeshNode* pmesh = mpMesh->m_data[i];
		if( strcmp( pmesh->GetName(), "Bip01" ) == 0 )
		{
			mLocalMat = pmesh->m_mat_result;
			D3DXMatrixInverse( &mLocalInv, 0, &mLocalMat );
			mBips[BIPS01]	= pmesh;
		}
		else if( strcmp( pmesh->GetName(), "Bip01 L Thigh" ) == 0 )
		{
			mBips[LTHIGH]	= pmesh;
		}
		else if( strcmp( pmesh->GetName(), "Bip01 R Thigh" ) == 0 )
		{
			mBips[RTHIGH]	= pmesh;
		}
		else if( strcmp( pmesh->GetName(), "Bip01 L Calf" ) == 0 )
		{
			mBips[LCALF]	= pmesh;
		}
		else if( strcmp( pmesh->GetName(), "Bip01 R Calf" ) == 0 )
		{
			mBips[RCALF]	= pmesh;
		}
		else if( strcmp( pmesh->GetName(), "Bip01 L Foot" ) == 0 )
		{
			mBips[LFOOT]	= pmesh;
		}
		else if( strcmp( pmesh->GetName(), "Bip01 R Foot" ) == 0 )
		{
			mBips[RFOOT]	= pmesh;
		}
	}

	for( i = 0; i< NUM_CLOTH_BONE; i++)
	{
		if(mBips[i]	== NULL)
		{
			cclog("cloth bone %d missing\n",i);
			RSetError(RERROR_CLOTH_BONE_MISSING);
			return false;
		}
	}

	// cloth에 피직데이터가 없다는 것은...
	if (mpMeshNode->m_physique == NULL || mpMeshNode->m_physique_num == 0)
	{
		cclog("cloth physique missing\n");
		RSetError(RERROR_CLOTH_PHYSIQUE_MISSING);
		return false;
	}

	RSetError(ROK);

	for( i = 0 ; i < m_iCntP; ++i )
	{
		if( mpMeshNode->m_point_color_list[i].x != 0 )
		{
			m_pHolds[i]	= CLOTH_HOLD;
			m_pWeights[i]	= 0;
			continue;
		}
		if( mpMeshNode->m_point_color_list[i].y != 0 )
		{
			m_pHolds[i]	|= CLOTH_COLLISION;
			m_pWeights[i]	= 0;
		}
		if( mpMeshNode->m_point_color_list[i].z > 0 )
		{
			m_pHolds[i]	|= CLOTH_FORCE;
			m_pWeights[i]	= mpMeshNode->m_point_color_list[i].z*mpMeshNode->m_point_color_list[i].z;
		}
	}

#define SPHERE_RADIUS		10.0f
	{
		mSphere[0].setSphere( SPHERE_RADIUS);		
		mSphere[1].setSphere( SPHERE_RADIUS);		
		mSphere[2].setSphere( SPHERE_RADIUS);		
		mSphere[3].setSphere( SPHERE_RADIUS);		
		mSphere[4].setSphere( SPHERE_RADIUS);		
		mSphere[5].setSphere( SPHERE_RADIUS);		
	}

	int	 j, index, indexTemp[3];
	rplane planeTemp;
	rvector Point[3];

	for( i = 0 ; i < mpMeshNode->m_face_num; ++i )
	{
		for( j = 0 ; j < 3; ++j )
		{
			index		= mpMeshNode->m_face_list[i].m_point_index[j];
			Point[j]			= mpMeshNode->m_point_list[index];
			indexTemp[j]	= index;
		}
		rvector n;
		D3DXVec3Cross( &n, &(Point[2] - Point[0]), &(Point[2] - Point[1] ));	

		for( j = 0 ; j < 3; ++j )
		{
			index	= indexTemp[j];
			mpInitNormal[index] += n;
		}
	}

	for( i = 0 ; i < m_iCntP; ++i )
		D3DXVec3Normalize( &mpInitNormal[i], &mpInitNormal[i] );

	m_AccelationRatio	= 1.0f;
	m_fTimeStep			= 0.06 ;
	m_iCntIter				= 1;	

	memset( m_pForce, 0, sizeof(rvector) * m_iCntP );

	int* iTemp = new int[mpMeshNode->m_face_num*3];
	for( i = 0 ; i < mpMeshNode->m_face_num; ++i )
		for( j = 0 ; j < 3; ++j )
			iTemp[3*i+j] = mpMeshNode->m_face_list[i].m_point_index[j];

	delete iTemp;
	return true;
}


//////////////////////////////////////////////////////////////////////////
//	accumulate force
//////////////////////////////////////////////////////////////////////////
void RCharCloth::accumulateForces(bool bGame)
{
	if( !bGame )
	{
		for( int i = 0 ; i < m_iCntP; ++i )
		{
			float gravity = -90;
			float forward, side;
			forward	= max(min( mForceField.x , -30 ), -200 );
			side = max(min( mForceField.z,10), -10 ); 


			if( CLOTH_FORCE & m_pHolds[i] )
			{
				m_pForce[i].z	= side + mpInitNormal[i].x * 80 ;
				m_pForce[i].x	= forward -mpInitNormal[i].z * 30;
				m_pForce[i].y	= gravity;
			}
		}
	}
	else
	{
		//  천이 위로 말려 올라가는 것 방지
		float gravity = max(min( mForceField.y, -30 ), -90 );

		//	천이 앞으로 말리는 것과 양옆으로 빠지는 것 방지
		float forward, side;
		forward	= max(min( mForceField.x , -30 ), -200 );
		side = max(min( mForceField.z,10), -10 ); 

		if( mUpdateStatus & CHARACTER_DIE_STATE )
		{
			for( int i = 0 ; i < m_iCntP; ++i		)
			{
				if( CLOTH_FORCE & m_pHolds[i] )
				{
					m_pForce[i].z = mpInitNormal[i].x * 10; 
					m_pForce[i].x = -mpInitNormal[i].z * 10;
					m_pForce[i].y = gravity;
				}
			}
			return;
		}

		for( int i = 0 ; i < m_iCntP; ++i)
		{
			if( CLOTH_FORCE & m_pHolds[i] )
			{
				m_pForce[i].z	= side + mpInitNormal[i].x * 90 ;
				m_pForce[i].x	= forward -mpInitNormal[i].z * 50;
				m_pForce[i].y	= gravity;
			}
		}
	}	
}

//////////////////////////////////////////////////////////////////////////
//	valet
//////////////////////////////////////////////////////////////////////////
void RCharCloth::valet()
{
	if(  mUpdateStatus & NOT_VALET)
	{
		return;
	}
	rvector* swapTemp;
	for( int i = 0 ; i < m_iCntP; ++i )
	{
		if( CLOTH_VALET_ONLY == m_pHolds[i] )
		{
			m_pOldX[i] = m_pX[i] + m_AccelationRatio* ( m_pX[i] - m_pOldX[i] ); 
		}
		else if ( CLOTH_FORCE & m_pHolds[i] )
		{
			m_pOldX[i] = m_pX[i] + m_AccelationRatio* ( m_pX[i] - m_pOldX[i] ) + m_pForce[i] * m_fTimeStep * m_fTimeStep;
		}
	}
	swapTemp = m_pX;
	m_pX = m_pOldX;
	m_pOldX = swapTemp;
}


//////////////////////////////////////////////////////////////////////////
//	satisfy constraints
//////////////////////////////////////////////////////////////////////////
void RCharCloth::satisfyConstraints()
{
	sConstraint c;
	rvector x1, x2;
	rvector delta;
	float deltaLegth;
	float diff;
	int i, j, k;
	rvector intersection;

	c.refA = 0;
	c.refB = 0;
	c.restLength = 0.f;

	for( i = 0 ; i < m_iCntIter; ++i )
	{

		if(( mUpdateStatus & NOT_COLLISION ) == 0 )
		{

			for( j = 0 ; j < m_iCntP; ++j )							// particle index - j
			{
				for( k = 0 ; k < 6; ++k )													// sphere index - k
				{
					if( CLOTH_COLLISION & m_pHolds[j] )
					{
						rvector dir = m_pX[j] - m_pOldX[j];
						D3DXVec3Normalize(&dir, &dir);
						if(IsIntersect(m_pOldX[j], m_pX[j], dir ,mSphere[k].mCentre, mSphere[k].mRadius, m_pNormal[j], &intersection ))
						{
							m_pX[j] = intersection;
							break;
						}
					}
				}
			}
		}	

		float w1, w2;
		for( j = 0 ; j < m_iCntC; ++j )
		{
			c = m_pConst[j];
			if( c.refA <0 || c.refA>=m_iCntP )
			{
				cclog("RCharCloth_Error:: constraints reference Particle is Out of Range - ref : %d, n_particles : %d, mesh_node : %s, mesh : %s \n", c.refA, j, mpMeshNode->m_Name, mpMesh->GetFileName());
				continue;
			}
			if( c.refB < 0 || c.refB >= m_iCntP )
			{
				cclog("RCharCloth_Error:: constraints reference Particle is Out of Range - ref : %d, n_particles : %d, mesh_node : %s, mesh : %s \n", c.refA, j, mpMeshNode->m_Name, mpMesh->GetFileName());
				continue;
			}
			x1 = m_pX[c.refA];
			x2 = m_pX[c.refB];
			w1 = m_pWeights[c.refA];
			w2 = m_pWeights[c.refB];

			if( w1 == 0 && w2 == 0 )
				continue;
			delta = x2 - x1;

			deltaLegth = D3DXVec3Length( &delta );
			if( deltaLegth == 0 )
				diff = 0;
			else
				diff = (float)((deltaLegth - c.restLength)/(deltaLegth*(w1+w2)));

			m_pX[c.refA] += delta*w1*diff;
			m_pX[c.refB] -= delta*w2*diff;
		}


	}
}


//////////////////////////////////////////////////////////////////////////
//	update
//////////////////////////////////////////////////////////////////////////
void RCharCloth::update( bool bGame,rmatrix* pWorldMat_, float fDist_ )
{
	
	updateCO();
	updatePosition( pWorldMat_ );

	DWORD currTime = timeGetTime();
	if( currTime - mTime < 10 )
	{
		return;
	}
	mTime	= currTime;

	if( mUpdateStatus == CHARACTER_DIE_STATE )
	{
		mUpdateStatus |= NOT_COLLISION;
	}
	else
	{
		mUpdateStatus	= ALL;
		if( fDist_ >  COLLISION_DISTANCE )
			mUpdateStatus	|= NOT_COLLISION;
		if( fDist_ > VALET_DISTANCE )
			mUpdateStatus |= NOT_VALET;
		if( fDist_ > CAL_LENGTH_DISTANCE )
			mUpdateStatus	|= NOT_CAL_LENGTH;
	}

	accumulateForces(bGame);
	valet();
	satisfyConstraints();
}


//////////////////////////////////////////////////////////////////////////
//	updatePosition
//////////////////////////////////////////////////////////////////////////
void RCharCloth::updatePosition( rmatrix* pWorldMat_ )
{
	int nRefBone;
	int parentId;
	float weight;
	rmatrix mat;
	rvector rVec;
	rvector localVec;

	for( int i =0 ; i < m_iCntP; ++i )
	{
		if( CLOTH_HOLD & m_pHolds[i] || mInitParticle )
		{
			rVec = rvector( 0,0,0 );
			nRefBone = mpMeshNode->m_physique[i].m_num;
			//todok RPhysiqueInfo::m_num 으로 루프를 돌면 안전장치가 전혀 없다
			// 실제로 로딩한 후 ConnectMatrix()나 ConnectPhysique()를 돌릴때는 m_num > MAX_PHYSIQUE_KEY 이면
			// MAX_PHYSIQUE_KEY 까지만 돌고 있다. 여기서는 그냥 m_num으로 도니까 메모리 침범이 일어난다. 
			// 익스포트할때 경고메시지를 보여주도록 수정해야 할 것이다
			for( int j = 0 ; j < nRefBone; ++j )
			{
				parentId = mpMeshNode->m_physique[i].m_parent_id[j];
				weight = mpMeshNode->m_physique[i].m_weight[j];
				mat = mpMesh->m_data[ parentId ]->m_mat_result;
				localVec = mpMeshNode->m_physique[i].m_offset[j];
				rVec += localVec * mat * weight;
			}
			m_pX[i]		= rVec * mLocalInv;
			m_pOldX[i]	= m_pX[i];
		}
	}

	mWorldMat = *pWorldMat_;
	D3DXMatrixInverse( &mWorldInv, 0, &mWorldMat);
	mInitParticle = false;
}

//////////////////////////////////////////////////////////////////////////
//	create
//////////////////////////////////////////////////////////////////////////
bool RCharCloth::create( RMesh* pMesh_, RMeshNode* pMeshNode_ )
{
	mpMesh = pMesh_;
	mpMeshNode = pMeshNode_;

	int i;
	rvector vecDistance;

	// vertex copy
	m_iCntP = mpMeshNode->m_point_num;
	m_iCntC = mpMeshNode->m_face_num * 3 ;

	m_pX			= new rvector[m_iCntP];
	m_pOldX			= new rvector[m_iCntP];
	m_pForce		= new rvector[m_iCntP];
	m_pHolds		= new int    [m_iCntP];
	m_pWeights		= new float  [m_iCntP];
	m_pNormal		= new rvector[m_iCntP];
	mpInitNormal	= new rvector[m_iCntP];

	m_pConst		= new sConstraint[m_iCntC];

	memset( m_pX		, 0, sizeof(rvector) * m_iCntP );
	memset( m_pOldX		, 0, sizeof(rvector) * m_iCntP );
	memset( m_pForce	, 0, sizeof(rvector) * m_iCntP );
	memset( m_pHolds	, 0, sizeof(int)	 * m_iCntP );
	memset( m_pWeights	, 0, sizeof(float)   * m_iCntP );
	memset( m_pNormal	, 0, sizeof(rvector) * m_iCntP );
	memset( mpInitNormal, 0, sizeof(rvector) * m_iCntP );

	memset( m_pConst, 0, sizeof(sConstraint)*m_iCntC);

	//unsigned short* TempIndexBuf = new unsigned short[mpMeshNode->m_face_num*3];

	for( i = 0 ; i < mpMeshNode->m_face_num; ++i )
	{
		for( int j = 0 ; j < 3; ++j )
		{
			m_pConst[ i*3 + j ].refA = mpMeshNode->m_face_list[i].m_point_index[j];
			if( j + 1 >= 3 )
			{
				m_pConst[ i*3 + j ].refB = mpMeshNode->m_face_list[i].m_point_index[0];
			}
			else
			{
				m_pConst[ i*3 + j ].refB = mpMeshNode->m_face_list[i].m_point_index[j+1];
			}
			vecDistance = mpMeshNode->m_point_list[m_pConst[ i*3 + j ].refA] - mpMeshNode->m_point_list[m_pConst[ i*3 + j ].refB];
			m_pConst[ i*3 + j ].restLength = D3DXVec3Length(&vecDistance);

		}
	}

	UpdateNormal();

	// Render Vertex setup
	m_iNumVertices = 3 * mpMeshNode->m_face_num;
	//gVertices	= new RVertex[ m_iNumVertices ];

	// Vertex Buffer Setup)

	if (!initialize())
		return false;

	mInitParticle	= true;

	OnRestore();

	return true;
}


//////////////////////////////////////////////////////////////////////////
//	update Collision Object
//	이 함수는 반드시 RMesh의 RenderFrame함수가 불려진 이후에 불려져야 한다
//	RMesh 의 Render함수에서 RenderSub함수 불러준 후에 이 함수를 호출한다
//////////////////////////////////////////////////////////////////////////
void RCharCloth::updateCO()
{
	mLocalMat = mBips[BIPS01]->m_mat_result;
	D3DXMatrixInverse( &mLocalInv, 0, &mLocalMat );

	// Fastest Version.. NO MORE THAN THIS!!
	{
#define GetVector( x, y ) rvector( x._41 * y._11 + x._42 * y._21 + x._43 * y._31 + x._44 * y._41, \
	x._41 * y._12 + x._42 * y._22 + x._43 * y._32 + x._44 * y._42,\
	x._41 * y._13 + x._42 * y._23 + x._43 * y._33 + x._44 * y._43 )

		rvector vec, vec_1;

		vec	= GetVector(mBips[LTHIGH]->m_mat_result, mLocalInv );
		vec.y -= 7.f;
		vec.x -= 3.f;
		mSphere[0].setSphere( vec );

		vec	= GetVector(mBips[RTHIGH]->m_mat_result, mLocalInv);
		vec.y -= 7.f;
		vec.x -= 3.f;
		mSphere[1].setSphere( vec );

		vec	= GetVector(mBips[LTHIGH]->m_mat_result, mLocalInv );
		vec_1	= GetVector(mBips[LCALF]->m_mat_result, mLocalInv);
		mSphere[2].setSphere( (vec*0.4 + vec_1*0.6) );

		vec	= GetVector(mBips[RTHIGH]->m_mat_result, mLocalInv );
		vec_1	= GetVector(mBips[RCALF]->m_mat_result, mLocalInv);
		mSphere[3].setSphere( (vec*0.4 + vec_1*0.6));

		vec = GetVector(mBips[LCALF]->m_mat_result, mLocalInv);
		mSphere[4].setSphere(vec);

		vec = GetVector(mBips[RCALF]->m_mat_result, mLocalInv);
		mSphere[5].setSphere(vec);
	}
}


//////////////////////////////////////////////////////////////////////////
//	setForce
//////////////////////////////////////////////////////////////////////////
void RCharCloth::setForce( rvector& force_ )
{
	if( mInitParticle )
	{
		mForceField = rvector( 0.0f, 0.0f, 0.0f );
	}
	else
	{
		D3DXVECTOR4 vec;
		rmatrix mat = mWorldInv * mLocalInv ;
		mat._41 = mat._42 = mat._43 = 0;
		D3DXVec3Transform(&vec, &force_, &mat );
		mForceField = rvector( vec.x, vec.y, vec.z );
	}
}

_USING_NAMESPACE_REALSPACE2;

//////////////////////////////////////////////////////////////////////////
//	render
//////////////////////////////////////////////////////////////////////////
void RCharCloth::render()
{     
	//	bHarewareBuffer = true;
	int i;

	LPDIRECT3DDEVICE9 dev =	RGetDevice(); // Get Device Pointer

	UpdateNormal();

	RMtrlMgr* pMtrlMgr = &mpMeshNode->m_pParentMesh->m_mtrl_list_ex;
	RMtrl* pMtrl = pMtrlMgr->Get_s(mpMeshNode->m_mtrl_id,-1);
	int num_mtrl = pMtrl->m_sub_mtrl_num;

	int point_index;		// 현재 버텍스의 인덱스

	for( i = 0 ; i < mpMeshNode->m_face_num ; ++i )
	{
		for( int j = 0 ; j < 3; ++j )
		{
			point_index = mpMeshNode->m_face_list[i].m_point_index[j];
			gVertices[3*i+j].p	= m_pX[point_index];
			gVertices[3*i+j].tu	= mpMeshNode->m_face_list[i].m_point_tex[j].x;
			gVertices[3*i+j].tv	= mpMeshNode->m_face_list[i].m_point_tex[j].y;
			gVertices[3*i+j].n	= m_pNormal[point_index];
		}
	}	

	if( bHarewareBuffer && gpClothVertexBuffer)
	{
		//// Copy Begin
		void *Buffer;
		if( FAILED( gpClothVertexBuffer->Lock( 0, sizeof(RVertex) * mpMeshNode->m_face_num * 3, (VOID**)&Buffer, D3DLOCK_DISCARD )))
		{
			bHarewareBuffer = false;
			REL( gpClothVertexBuffer );

			cclog("Fail to lock of Vertex Buffer\n");
			goto e2SoftRender;
		}
		//memcpy( Buffer, gVertices, sizeof(RVertex) * m_iCntP );
		memcpy( Buffer, gVertices, sizeof(RVertex) * mpMeshNode->m_face_num * 3 );

		gpClothVertexBuffer->Unlock();
		// Copy End
	}
e2SoftRender:
	prerender();

	if(mpMesh->m_pVisualMesh)
		mpMesh->m_pVisualMesh->UpdateLight();

	rmatrix rtemp;
	dev->GetTransform( D3DTS_WORLD, &rtemp );
	dev->SetTransform( D3DTS_WORLD ,  &( mLocalMat * mWorldMat ) );

	mpMesh->SetCharacterMtrl_ON( pMtrl,mpMeshNode,1 ,mpMeshNode->GetTColor());

#ifdef USE_TOON_RENDER

	mpMeshNode->ToonRenderSettingOn(pMtrl);	

#endif

	if( bHarewareBuffer )
	{			
		dev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, mpMeshNode->m_face_num );
	}
	else
	{
		dev->DrawPrimitiveUP( D3DPT_TRIANGLELIST, mpMeshNode->m_face_num, gVertices, sizeof(RVertex));
	}

#ifdef USE_TOON_RENDER

//	if(Silhouette)
	{
		mpMeshNode->ToonRenderSilhouetteSettingOn();

		if( bHarewareBuffer )
			dev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, mpMeshNode->m_face_num );
		else
			dev->DrawPrimitiveUP( D3DPT_TRIANGLELIST, mpMeshNode->m_face_num, gVertices, sizeof(RVertex));

		mpMeshNode->ToonRenderSilhouetteSettingOff();
	}

	mpMeshNode->ToonRenderSettingOff();	

#endif

	mpMesh->SetCharacterMtrl_OFF( pMtrl, 1 );
	dev->SetTransform( D3DTS_WORLD, &rtemp );

	postrender();

}

//////////////////////////////////////////////////////////////////////////
//	UpdateNormal
//////////////////////////////////////////////////////////////////////////
void RCharCloth::UpdateNormal()
{
	int	i, j, index, indexTemp[3];
	rplane planeTemp;
	rvector Point[3];

	_ASSERT( m_pNormal );

	memset( m_pNormal, 0, sizeof(rvector)*m_iCntP );

	_ASSERT( mpMeshNode );

	for( i = 0 ; i < mpMeshNode->m_face_num; ++i )
	{
		for( j = 0 ; j < 3; ++j )
		{
			index		= mpMeshNode->m_face_list[i].m_point_index[j];

			_ASSERT( index < m_iCntP );
			_ASSERT( index >= 0 );

			Point[j]		= m_pX[index];
			indexTemp[j]	= index;
		}
		rvector n;
		D3DXVec3Cross( &n, &(Point[2] - Point[0]), &(Point[2] - Point[1] ));	

		for( j = 0 ; j < 3; ++j )
		{
			index	= indexTemp[j];

			_ASSERT( index < m_iCntP );
			_ASSERT( index >= 0 );

			m_pNormal[index] += n;
		}
	}

	for( i = 0 ; i < m_iCntP; ++i )
	{
		D3DXVec3Normalize( &m_pNormal[i], &m_pNormal[i] );
	}
}

void RCharCloth::prerender()
{
	LPDIRECT3DDEVICE9 dev =	RGetDevice(); // Get Device Pointer

	dev->SetFVF( RVertexType );
	dev->SetRenderState( D3DRS_LIGHTING, TRUE );
	dev->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	dev->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
	dev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);

	if( bHarewareBuffer )
		dev->SetStreamSource(0, gpClothVertexBuffer, 0,sizeof(RVertex) );
}

void RCharCloth::postrender()
{

}

void RCharCloth::renderforce()
{
	prerender();
	render();
	postrender();
}

void RCharCloth::OnInvalidate()
{
	REL(gpClothVertexBuffer);
}

void RCharCloth::OnRestore()
{
	if( gpClothVertexBuffer != 0 ) return;

	if( FAILED( RGetDevice()->CreateVertexBuffer( sizeof(RVertex) * NUM_MAX_COAT_VERTEX
		, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, RVertexType,  D3DPOOL_DEFAULT, &gpClothVertexBuffer ,NULL)))	
	{
		bHarewareBuffer = false;
		SAFE_RELEASE( gpClothVertexBuffer );
		cclog("Fail to Create Cloth Vertex buffer");
	}
	bHarewareBuffer = true;
}