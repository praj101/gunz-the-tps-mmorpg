#pragma once
#include <math.h>

class CCVector3
{
public:
	union {
		struct {
			float x, y, z;
		} ;
		float v[3];
	};

	CCVector3() { }
	CCVector3(float xx, float yy, float zz): x(xx), y(yy), z(zz) { }

	// operator
	CCVector3 operator-() const { return CCVector3(-x, -y, -z);   }
	CCVector3& operator=(const CCVector3& other)	{ x = other.x; y = other.y; z = other.z; return *this; }

	CCVector3 operator+(const CCVector3& other) const { return CCVector3(x + other.x, y + other.y, z + other.z);	}
	CCVector3& operator+=(const CCVector3& other)	{ x+=other.x; y+=other.y; z+=other.z; return *this; }

	CCVector3 operator-(const CCVector3& other) const { return CCVector3(x - other.x, y - other.y, z - other.z);	}
	CCVector3& operator-=(const CCVector3& other)	{ x-=other.x; y-=other.y; z-=other.z; return *this; }

	CCVector3 operator*(const CCVector3& other) const { return CCVector3(x * other.x, y * other.y, z * other.z);	}
	CCVector3& operator*=(const CCVector3& other)	{ x*=other.x; y*=other.y; z*=other.z; return *this; }
	CCVector3 operator*(const float v) const { return CCVector3(x * v, y * v, z * v);	}
	CCVector3& operator*=(const float v) { x*=v; y*=v; z*=v; return *this; }

	CCVector3 operator/(const CCVector3& other) const { return CCVector3(x / other.x, y / other.y, z / other.z);	}
	CCVector3& operator/=(const CCVector3& other)	{ x/=other.x; y/=other.y; z/=other.z; return *this; }
	CCVector3 operator/(const float v) const { float i=(float)1.0/v; return CCVector3(x * i, y * i, z * i);	}
	CCVector3& operator/=(const float v) { float i=(float)1.0/v; x*=i; y*=i; z*=i; return *this; }

	bool operator<=(const CCVector3&other) const { return x<=other.x && y<=other.y && z<=other.z;};
	bool operator>=(const CCVector3&other) const { return x>=other.x && y>=other.y && z>=other.z;};
	bool operator==(const CCVector3& other) const { return other.x==x && other.y==y && other.z==z; }
	bool operator!=(const CCVector3& other) const { return other.x!=x || other.y!=y || other.z!=z; }

	friend CCVector3 operator * ( float f, const class CCVector3& v) { return CCVector3( f*v.x , f*v.y , f*v.z ); }

	// function
	void Set(float x, float y, float z) { CCVector3::x = x; CCVector3::y = y; CCVector3::z = z; }
	void Set(CCVector3& p)				{ CCVector3::x = p.x; CCVector3::y = p.y; CCVector3::z = p.z; }
	float Magnitude();
	float MagnitudeSQ();
	float DotProduct(const CCVector3& other) const;
	CCVector3 CrossProduct(const CCVector3& p) const;
	CCVector3& Normalize();
	void SetLength(float newlength);
	void Invert();
	CCVector3 GetInterpolated(const CCVector3& other, float d) const;

	static const CCVector3 IDENTITY;
	static const CCVector3 AXISX;
	static const CCVector3 AXISY;
	static const CCVector3 AXISZ;
};



// inline functions ////////////////////////////////////////////////////////////////////
inline float CCVector3::Magnitude()
{
	return (float)sqrt(x*x+y*y+z*z);
}

inline float CCVector3::MagnitudeSQ()
{
	return (x*x+y*y+z*z);
}

inline float CCVector3::DotProduct(const CCVector3& other) const
{
	return x*other.x + y*other.y + z*other.z;
}

inline CCVector3 CCVector3::CrossProduct(const CCVector3& p) const
{
	return CCVector3(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
}

inline CCVector3& CCVector3::Normalize()
{
	float scale = (float)Magnitude();

	if (scale == 0)
		return *this;

	scale = (float)1.0f / scale;
	x *= scale;
	y *= scale;
	z *= scale;
	return *this;
}

inline void CCVector3::SetLength(float newlength)
{
	Normalize();
	*this *= newlength;
}

inline void CCVector3::Invert()
{
	x *= -1.0f;
	y *= -1.0f;
	z *= -1.0f;
}

inline CCVector3 CCVector3::GetInterpolated(const CCVector3& other, float d) const
{
	float inv = 1.0f - d;
	return CCVector3(other.x*inv + x*d,
						other.y*inv + y*d,
						other.z*inv + z*d);
}
