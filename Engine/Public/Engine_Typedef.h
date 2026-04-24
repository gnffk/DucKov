#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{

	typedef		bool						_bool;
	typedef		char						_char;
	typedef		unsigned char				_uchar;
	typedef		wchar_t						_tchar;
	typedef		long						_long;
	typedef		unsigned long				_ulong;

	typedef		float						_float;
	typedef		double						_double;
	typedef		string						_string;
	typedef		wstring						_wstring;

	/* 저장용 데이터 */
	typedef		XMFLOAT2					_float2;
	typedef		XMFLOAT3					_float3;
	typedef		XMFLOAT4					_float4;
	typedef		XMFLOAT4X4					_float4x4;

	/* 연산용 데이터 */
	typedef		XMVECTOR					_vector;
	typedef		FXMVECTOR					_fvector;
	typedef		GXMVECTOR					_gvector;
	typedef		HXMVECTOR					_hvector;
	typedef		CXMVECTOR					_cvector;

	typedef		XMMATRIX					_matrix;
	typedef		FXMMATRIX					_fmatrix;
	typedef		CXMMATRIX					_cmatrix;

}
#endif // Engine_Typedef_h__
