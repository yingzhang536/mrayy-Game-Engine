#pragma once

namespace msa {
#ifndef mPI
#define mPI       3.14159265358979323846
#endif
	
#ifndef mTWO_PI
#define mTWO_PI   6.28318530717958647693
#endif
	
#ifndef mM_TWO_PI
#define mM_TWO_PI   6.28318530717958647693
#endif
	
#ifndef mFOUR_PI
#define mFOUR_PI 12.56637061435917295385
#endif
	
#ifndef mHALF_PI
#define mHALF_PI  1.57079632679489661923
#endif
	
#ifndef mDEG_TO_RAD
#define mDEG_TO_RAD (PI/180.0)
#endif
	
#ifndef mRAD_TO_DEG
#define mRAD_TO_DEG (180.0/PI)
#endif
	
	// returns always positive modulo
	inline int mod(int dividend, int divisor) {
		dividend %= divisor;
		if(dividend<0) dividend += divisor;
		return dividend;
	}
	
	
	inline float fastInvSquareRoot(float x) {
		float xhalf = 0.5f*x;
		int i = *(int*)&x;
		i = 0x5f3759df - (i>>1);
		x = *(float*)&i;
		x = x*(1.5f - xhalf*x*x);
		return x;
	}
	
	//	inline void fastNormalize(Vec3f &p) {
	//		float f = fastInvSquareRoot(p.lengthSquared());
	//		p *= f;
	//	}
	
	
	template<typename T> void SWAP( T& a, T& b)						{ T tmp = b; b = a; a = tmp; }
	
	template<typename T>
	float mapRange(T value, T inputMin, T inputMax, T outputMin, T outputMax, bool clamp = false) {
		T outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
		
		if(clamp){
			if(outputMax < outputMin){
				if( outVal < outputMax )outVal = outputMax;
				else if( outVal > outputMin )outVal = outputMin;
			}else{
				if( outVal > outputMax )outVal = outputMax;
				else if( outVal < outputMin )outVal = outputMin;
			}
		}
		return outVal;
	}
	
	template <typename T>
	void bounce(T &pos, T &vel, T min, T max, float bounceFactor = 1) {
		if(pos < min) {
			pos = min;
			vel = fabs(vel) * bounceFactor;
		} else if(pos > max) {
			pos = max;
			vel = -fabs(vel) * bounceFactor;
		}
	}
	
	template <typename T>
	bool inRange(T a, T min, T max) {
		return (a >= min) && (a <= max);
	}

    
	
}