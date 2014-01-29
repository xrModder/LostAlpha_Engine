#include "stdafx.h"

#ifdef USE_OLD_MESHMENDER
#include "nvMeshMender.h"
void CBuild::xrPhase_TangentBasis()
{
	// ************************************* Correct single-degenerates
	/*
	float	teps			= .5f / 4096.f;		// half pixel from 4096 texture (0.0001220703125)
	for (u32 f=0; f<g_faces.size(); f++)
	{
		Progress	(float(f)/float(g_faces.size()));
		Face*		F		= g_faces[f];
		Fvector2&	tc0		= F->tc.front().uv[0];
		Fvector2&	tc1		= F->tc.front().uv[1];
		Fvector2&	tc2		= F->tc.front().uv[2];
		float		e01		= tc0.distance_to(tc1);	
		float		e12		= tc1.distance_to(tc2);
		float		e20		= tc2.distance_to(tc0);
		if (e01<teps)
		{
			Fvector2	d0,d1,d,r;
			d0.sub		(tc0,tc2);	d0.norm	();
			d1.sub		(tc1,tc2);	d1.norm	();
			d.averageA	(d0,d1);	d.norm	();
			r.cross		(d);		r.mul	(teps);	// right
			tc1.add		(r);		tc0.sub	(r);	// correction
		} else if (e12<teps) {
			Fvector2	d1,d2,d,r;
			d0.sub		(tc0,tc2);	d0.norm	();
			d1.sub		(tc1,tc2);	d1.norm	();
			d.averageA	(d0,d1);	d.norm	();
			r.cross		(d);		r.mul	(teps);	// right
			tc1.add		(r);		tc0.sub	(r);	// correction
		}
	}
	*/

	// ************************************* Declare inputs
	Status						("Declarator...");
	u32 v_count_reserve			= iFloor(float(g_vertices.size())*1.33f);
	u32 i_count_reserve			= 3*g_faces.size();

	xr_vector<NVMeshMender::VertexAttribute> input,output;
	input.push_back	(NVMeshMender::VertexAttribute());	// pos
	input.push_back	(NVMeshMender::VertexAttribute());	// norm
	input.push_back	(NVMeshMender::VertexAttribute());	// tex0
	input.push_back	(NVMeshMender::VertexAttribute());	// *** faces

	input[0].Name_= "position";	xr_vector<float>&	v_position	= input[0].floatVector_;	v_position.reserve	(v_count_reserve);
	input[1].Name_= "normal";	xr_vector<float>&	v_normal	= input[1].floatVector_;	v_normal.reserve	(v_count_reserve);
	input[2].Name_= "tex0";		xr_vector<float>&	v_tc		= input[2].floatVector_;	v_tc.reserve		(v_count_reserve);
	input[3].Name_= "indices";	xr_vector<int>&		v_indices	= input[3].intVector_;		v_indices.reserve	(i_count_reserve);

	output.push_back(NVMeshMender::VertexAttribute());	// tex0
	output.push_back(NVMeshMender::VertexAttribute());	// tangent
	output.push_back(NVMeshMender::VertexAttribute());	// binormal
	output.push_back(NVMeshMender::VertexAttribute());	// *** faces
	output.push_back(NVMeshMender::VertexAttribute());	// position, needed for mender

	output[0].Name_= "tex0";	
	output[1].Name_= "tangent";	
	output[2].Name_= "binormal";
	output[3].Name_= "indices";	
	output[4].Name_= "position";

	// ************************************* Build vectors + expand TC if nessesary
	Status						("Building inputs...");
	std::sort					(g_vertices.begin(),g_vertices.end());
	xr_vector<xr_vector<u32> >	remap;
	remap.resize				(g_vertices.size());
	for (u32 f=0; f<g_faces.size(); f++)
	{
		Progress	(float(f)/float(g_faces.size()));
		Face*		F	= g_faces[f];

		for (u32 v=0; v<3; v++)
		{
			Vertex*		V	= F->v[v];	
			Fvector2	Ftc = F->tc.front().uv[v];
			u32 ID			= u32(std::lower_bound(g_vertices.begin(),g_vertices.end(),V)-g_vertices.begin());
			xr_vector<u32>& m	= remap[ID];

			// Search
			BOOL bFound		= FALSE;
			for (u32 it=0; it<m.size(); it++)
			{
				u32		m_id		= m[it];
				float*	tc			= &*v_tc.begin()+m_id*3;
				if (!fsimilar(tc[0],Ftc.x))	continue;
				if (!fsimilar(tc[1],Ftc.y))	continue;

				bFound				= TRUE;
				v_indices.push_back	(m_id);
				break;
			}

			// Register new if not found
			if (!bFound)
			{
				u32				m_id			= v_position.size()/3;
				v_position.push_back(V->P.x);	v_position.push_back(V->P.y);	v_position.push_back(V->P.z);
				v_normal.push_back(V->N.x);		v_normal.push_back(V->N.y);		v_normal.push_back(V->N.z);
				v_tc.push_back(Ftc.x);			v_tc.push_back(Ftc.y);			v_tc.push_back(0);
				v_indices.push_back(m_id);
				remap[ID].push_back(m_id);
			}
		}
	}
	remap.clear	();
	u32			v_was	= g_vertices.size();
	u32			v_become= v_position.size()/3;
	clMsg		("duplication: was[%d] / become[%d] - %2.1f%%",v_was,v_become,100.f*float(v_become-v_was)/float(v_was));

	// ************************************* Perform mungle
	Status			("Calculating basis...");
	NVMeshMender	mender	;
	if (!mender.Munge		(
		input,										// input attributes
		output,										// outputs attributes
		deg2rad(61.f),								// tangent space smooth angle
		0,											// no texture matrix applied to my texture coordinates
		NVMeshMender::FixTangents,					// fix degenerate bases & texture mirroring
		NVMeshMender::DontFixCylindricalTexGen,		// handle cylindrically mapped textures via vertex duplication
		NVMeshMender::DontWeightNormalsByFaceSize	// weigh vertex normals by the triangle's size
		))
	{
		Debug.fatal	(DEBUG_INFO,"NVMeshMender failed (%s)",mender.GetLastError().c_str());
	}

	// ************************************* Bind declarators
	// bind
	xr_vector<float>&	o_tc		= output[0].floatVector_;	R_ASSERT(output[0].Name_=="tex0");
	xr_vector<float>&	o_tangent	= output[1].floatVector_;	R_ASSERT(output[1].Name_=="tangent");
	xr_vector<float>&	o_binormal	= output[2].floatVector_;	R_ASSERT(output[2].Name_=="binormal");
	xr_vector<int>&		o_indices	= output[3].intVector_;		R_ASSERT(output[3].Name_=="indices");

	// verify
	R_ASSERT		(3*g_faces.size()	== o_indices.size());
	R_ASSERT		(o_tc.size()		== o_tangent.size());
	R_ASSERT		(o_tangent.size()	== o_binormal.size());
	R_ASSERT		(o_tc.size()		>= v_tc.size());

	// ************************************* Retreive data
	Status						("Retreiving basis...");
	for (u32 f=0; f<g_faces.size(); f++)
	{
		Face*	F				= g_faces[f];
		for (u32 v=0; v<3; v++)
		{
			u32	id							=	o_indices	[f*3+v];	// vertex index
			R_ASSERT						(id*3 < o_tc.size());
			F->tc.front().uv[v].set			(o_tc		[id*3+0],	o_tc		[id*3+1]);
			F->basis_tangent[v].set			(o_tangent	[id*3+0],	o_tangent	[id*3+1],	o_tangent	[id*3+2]);
			F->basis_binormal[v].set		(o_binormal	[id*3+0],	o_binormal	[id*3+1],	o_binormal	[id*3+2]);
		}
	}
}

#else // testing
#include "MeshMenderLayerOrdinaryStatic.h"
static u32 find_same_vertex( const xr_vector<u32> &m, const Fvector2	&Ftc, const xr_vector< MeshMender::Vertex > &theVerts )
{
	// Search
	for ( u32 it=0; it<m.size(); it++ )
	{
		u32	m_id	= m[it];
		float tc[2]		= { theVerts[ m_id ].s, theVerts[ m_id ].t }; 
		if ( !fsimilar(tc[0],Ftc.x ) )	
			continue;
		if ( !fsimilar( tc[1], Ftc.y ) )	
			continue;
		return m_id;
	}
	return u32(-1);
}

static u32 add_vertex(	 const	Vertex						&V,
				 const	Fvector2					&Ftc,
				 xr_vector< MeshMender::Vertex >	&theVerts
			   )
{
	MeshMender::Vertex new_vertex;
	set_vertex( new_vertex, V, Ftc );
	theVerts.push_back( new_vertex );
	return theVerts.size() - 1;
}

static void	add_face(	const Face& F, 
					xr_vector< MeshMender::Vertex >& theVerts,
					xr_vector< unsigned int >& theIndices,
					xr_vector<xr_vector<u32> >	&remap )
{
	for (u32 v=0; v<3; v++)
	{
		const Vertex	*V	= F.v[v];	
		u32				ID	= u32(std::lower_bound(g_vertices.begin(),g_vertices.end(),V)-g_vertices.begin());
		xr_vector<u32>	&m	= remap[ID];
		Fvector2		Ftc = F.tc.front().uv[v];

		u32 vertex_index	= find_same_vertex( m, Ftc, theVerts );
		
		// Register new if not found
		if ( vertex_index == u32(-1) )
		{
			vertex_index =	add_vertex( *V, Ftc, theVerts );
			remap[ID].push_back( vertex_index );
		}
		
		theIndices	.push_back		(vertex_index);
	}
}

static void	fill_mender_input( xr_vector< MeshMender::Vertex >& theVerts, xr_vector< unsigned int >& theIndices )
{
		// ************************************* Build vectors + expand TC if nessesary
	Status						("Building inputs...");
	std::sort					(g_vertices.begin(),g_vertices.end());
	xr_vector<xr_vector<u32> >	remap;
	remap.resize				(g_vertices.size());
	for (u32 f=0; f<g_faces.size(); f++)
	{
		Progress	(float(f)/float(g_faces.size()));
		Face*	F	= g_faces[f];
		add_face( *F, theVerts, theIndices, remap );
	}
	remap.clear	();
}

static void retrive_data_from_mender_otput( const	 xr_vector< MeshMender::Vertex >& theVerts, const xr_vector< unsigned int >& theIndices )

{
	// ************************************* Retreive data
	Status						("Retreiving basis...");
	for (u32 f=0; f<g_faces.size(); f++)
	{
		Face*	F						=	g_faces[f];
		u32	id0							=	theIndices	[f*3+0];	// vertex index
		u32	id1							=	theIndices	[f*3+1];	// vertex index
		u32	id2							=	theIndices	[f*3+2];	// vertex index
		R_ASSERT						( id0 < theVerts.size() );
		R_ASSERT						( id1 < theVerts.size() );
		R_ASSERT						( id2 < theVerts.size() );
		MeshMender::Vertex verts[3] =	{ theVerts[id0], theVerts[id1], theVerts[id2] };
		set_face( *F, verts );
	}
}
static		xr_vector< MeshMender::Vertex > mender_in_out_verts;
static		xr_vector< unsigned int >		mender_in_out_indices;
static		xr_vector< unsigned int >		mender_mapping_out_to_in_vert;

void CBuild::xrPhase_TangentBasis()
{
	// ************************************* Declare inputs
	Status						("Declarator...");
	u32 v_count_reserve			= iFloor(float(g_vertices.size())*1.33f);
	u32 i_count_reserve			= 3*g_faces.size();
	
	mender_in_out_verts				.clear( );
	mender_in_out_indices			.clear( );
	mender_mapping_out_to_in_vert	.clear( );


	mender_in_out_verts				.reserve( v_count_reserve );
	mender_in_out_indices			.reserve( i_count_reserve );
	mender_mapping_out_to_in_vert	.reserve( v_count_reserve );


	fill_mender_input( mender_in_out_verts, mender_in_out_indices );

	u32			v_was	= g_vertices.size();
	u32			v_become= mender_in_out_verts.size();
	clMsg		("duplication: was[%d] / become[%d] - %2.1f%%",v_was,v_become,100.f*float(v_become-v_was)/float(v_was));

	// ************************************* Perform mungle
	Status			("Calculating basis...");
	
	MeshMender	mender	;

	if ( !mender.Mend		(
		  mender_in_out_verts,
		  mender_in_out_indices,
		  mender_mapping_out_to_in_vert,
		  1,
		  0.5,
		  0.5,
		  0.0f,
		  MeshMender::DONT_CALCULATE_NORMALS,
		  MeshMender::RESPECT_SPLITS,
		  MeshMender::DONT_FIX_CYLINDRICAL
		)
	)
	{
		Debug.fatal	(DEBUG_INFO, "NVMeshMender failed " );
	//	Debug.fatal	(DEBUG_INFO,"NVMeshMender failed (%s)",mender.GetLastError().c_str());
	}

	// ************************************* Bind declarators
	// bind

	retrive_data_from_mender_otput( mender_in_out_verts, mender_in_out_indices );
	mender_in_out_verts				.clear( );
	mender_in_out_indices			.clear( );
	mender_mapping_out_to_in_vert	.clear( );
}
#endif