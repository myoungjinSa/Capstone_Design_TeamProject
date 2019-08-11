#pragma once
#include <math.h>
#include "Precision.h"

namespace Physics
{
	extern real sleepEpsilon;

	void setSleepEpsilon(real value);

	real getSleepEpsilon();

	//Physics 네임 스페이스 안에 정의 되어 있어서 
	//XMFLOAT3과 충돌이 일어나지 않음
	class PVector3
	{
	public:
		real x;

		real y;

		real z;

	private:
		// padding to ensure 4 word alignment
		real pad;

	public:
		//default constructor creates a zero vector
		PVector3() : x(0), y(0), z(0) {}

		PVector3(const real x, const real y, const real z)
			:x(x), y(y), z(z) {}

		const static PVector3 GRAVITY;
        const static PVector3 HIGH_GRAVITY;
        const static PVector3 UP;
        const static PVector3 RIGHT;
        const static PVector3 OUT_OF_SCREEN;
        const static PVector3 X;
        const static PVector3 Y;
        const static PVector3 Z;


		real operator[](unsigned i) const
		{
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}

		real& operator[](unsigned i) 
		{
			if (i == 0) return x;
			if (i == 1) return y;
			return z;
		}


		void operator+=(const PVector3& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
		}

		//Returns the value of the given vector added to this.
		PVector3 operator+(const PVector3& v)const
		{
			return PVector3(x + v.x, y + v.y, z + v.z);
		}

		void operator-=(const PVector3& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
		}

		PVector3 operator-(const PVector3& v)const
		{
			return PVector3(x - v.x, y - v.y, z - v.z);
		}

		void operator*=(const real value)
		{
			x *= value;
			y *= value;
			z *= value;
		}

		PVector3 operator*(const real value) const
        {
            return PVector3(x*value, y*value, z*value);
        }

		
		PVector3 componentProduct(const PVector3 &vector) const
        {
            return PVector3(x * vector.x, y * vector.y, z * vector.z);
        }


		void componentProductUpdate(const PVector3 &vector)
		{
			x *= vector.x;
			y *= vector.y;
			z *= vector.z;
		}

		// 벡터 외적
		// U X V =		 |	i	j	k	|
		//				 |	u1	u2	u3	|
		//				 |	v1	v2	v3	|
		//||U X V||	= ||U||||V|||singθ|

		PVector3 vectorProduct(const PVector3 &vector)const
		{
			return PVector3(y*vector.z - z * vector.y,
							z*vector.x - x * vector.z,
							x*vector.y - y * vector.x);
		}


		void operator %=(const PVector3 &vector)
		{
			*this = vectorProduct(vector);
		}


		PVector3 operator%(const PVector3 &vector)const
		{
			return PVector3(y*vector.z - z * vector.y,
							z*vector.x - x * vector.z,
							x*vector.y - y * vector.x);
		}

		//벡터 내적
		real scalarProduct(const PVector3 &vector)const
		{
			return x * vector.x + y * vector.y + z * vector.z;
		}

		real operator*(const PVector3 &vector)const
		{
			return x * vector.x + y * vector.y + z * vector.z;
		}

		void addScaledVector(const PVector3& vector,real scale)
		{
			x += vector.x * scale;
			y += vector.y * scale;
			z += vector.z * scale;
		}

		//벡터의 크기 반환
		real magnitude() const
		{
			return real_sqrt(x*x + y * y + z * z);
		}

		//벡터의 제곱 크기 반환
		real squareMagnitude() const
		{
			return x * x + y * y + z * z;
		}

		//벡터의 최대 크기 제한
		void trim(real size)
		{
			if(squareMagnitude() > size * size)
			{
				normalize();
				x *= size;
				y *= size;
				z *= size;
			}
		}

		//Turns a non-zero vector into a vector of unit length.
		void normalize()
		{
			real l = magnitude();
			if (l > 0)
			{
				(*this) *= ((real)1) / l;
			}
		}

		PVector3 unit()const
		{
			PVector3 result = *this;
			result.normalize();
			return result;
		}

		bool operator==(const  PVector3& other) const
		{
			return x == other.x && y == other.y  && z == other.z;
		}

		bool operator!=(const PVector3& other)const
		{
			return !(*this == other);
		}



		 /**
         * Checks if this vector is component-by-component less than
         * the other.
         *
         * @note This does not behave like a single-value comparison:
         * !(a < b) does not imply (b >= a).
         */
		bool operator<(const PVector3& other)const
		{
			return x < other.x && y < other.y && z < other.z;
		}

		 /**
         * Checks if this vector is component-by-component less than
         * the other.
         *
         * @note This does not behave like a single-value comparison:
         * !(a < b) does not imply (b >= a).
         */
		bool operator>(const PVector3& other)const
		{
			return x > other.x && y > other.y && z > other.z;
		}


		  /**
         * Checks if this vector is component-by-component less than
         * the other.
         *
         * @note This does not behave like a single-value comparison:
         * !(a <= b) does not imply (b > a).
         */
        bool operator<=(const PVector3& other) const
        {
            return x <= other.x && y <= other.y && z <= other.z;
        }


		
        /**
         * Checks if this vector is component-by-component less than
         * the other.
         *
         * @note This does not behave like a single-value comparison:
         * !(a <= b) does not imply (b > a).
         */
        bool operator>=(const PVector3& other) const
        {
            return x >= other.x && y >= other.y && z >= other.z;
        }

		/** Zero all the components of the vector. */
        void clear()
        {
            x = y = z = 0;
        }

        /** Flips all the components of the vector. */
        void invert()
        {
            x = -x;
            y = -y;
            z = -z;
        }

	};


	 /**
     * Holds a three degree of freedom orientation.
     *
     * Quaternions have several mathematical 
	   properties that make them useful for
       representing orientations, but require four items of data to
       hold the three degrees of freedom. These four items of data can
       be viewed as the coefficients of a complex number with three
       imaginary parts. The mathematics of the quaternion is then
       defined and is roughly correspondent to the math of 3D
       rotations. A quaternion is only a valid rotation if it is
       normalised: i.e. it has a length of 1.
     *
     * @note Angular velocity and acceleration can be correctly
     * represented as vectors. Quaternions are only needed for
     * orientation.
     */

	/*
	
	
	*/
	class Quaternion
	{
	public:
		union{
			struct{
				//쿼터니언의 실수요소
				real r;

				//쿼터니언의 첫번째 복소수 요소
				real i;

				//쿼터니언의 두번째 복소수 요소
				real j;

				//쿼터니언의 세번째 복소수 요소
				real k;

			};

			//쿼터니언 데이터 배열
			real data[4];
		};


		 /**
         * The default constructor creates a quaternion representing
         * a zero rotation.
         */
		Quaternion() : r(1), i(0), j(0), k(0) {}



		 /**
         * The explicit constructor creates a quaternion with the given
         * components.
         *
         * @param r The real component of the rigid body's orientation
         * quaternion.
         *
         * @param i The first complex component of the rigid body's
         * orientation quaternion.
         *
         * @param j The second complex component of the rigid body's
         * orientation quaternion.
         *
         * @param k The third complex component of the rigid body's
         * orientation quaternion.
         *
         * @note The given orientation does not need to be normalised,
         * and can be zero. This function will not alter the given
         * values, or normalise the quaternion. To normalise the
         * quaternion (and make a zero quaternion a legal rotation),
         * use the normalise function.
         *
         * @see normalise
         */
        Quaternion(const real r, const real i, const real j, const real k)
            : r(r), i(i), j(j), k(k)
        {
        }
		 /**
         * Normalises the quaternion to unit length, making it a valid
         * orientation quaternion.
         */
        void normalize()
        {
            real d = r*r+i*i+j*j+k*k;

            // Check for zero length quaternion, and use the no-rotation
            // quaternion in that case.
            if (d < real_epsilon) {
                r = 1;
                return;
            }

            d = ((real)1.0)/real_sqrt(d);
            r *= d;
            i *= d;
            j *= d;
            k *= d;
        }
		/**
         * Multiplies the quaternion by the given quaternion.
         *
         * @param multiplier The quaternion by which to multiply.
         */
        void operator *=(const Quaternion &multiplier)
        {
            Quaternion q = *this;
            r = q.r*multiplier.r - q.i*multiplier.i -
                q.j*multiplier.j - q.k*multiplier.k;
            i = q.r*multiplier.i + q.i*multiplier.r +
                q.j*multiplier.k - q.k*multiplier.j;
            j = q.r*multiplier.j + q.j*multiplier.r +
                q.k*multiplier.i - q.i*multiplier.k;
            k = q.r*multiplier.k + q.k*multiplier.r +
                q.i*multiplier.j - q.j*multiplier.i;
        }

		 /**
         * Adds the given vector to this, scaled by the given amount.
         * This is used to update the orientation quaternion by a rotation
         * and time.
         *
         * @param vector The vector to add.
         *
         * @param scale The amount of the vector to add.
         */
        void addScaledVector(const PVector3& vector, real scale)
        {
            Quaternion q(0,
                vector.x * scale,
                vector.y * scale,
                vector.z * scale);
            q *= *this;
            r += q.r * ((real)0.5);
            i += q.i * ((real)0.5);
            j += q.j * ((real)0.5);
            k += q.k * ((real)0.5);
        }
		void rotateByVector(const PVector3& vector)
        {
            Quaternion q(0, vector.x, vector.y, vector.z);
            (*this) *= q;
        }

	};


	/**
     * Holds a transform matrix, consisting of a rotation matrix and
     * a position. The matrix has 12 elements, it is assumed that the
     * remaining four are (0,0,0,1); producing a homogenous matrix.
     */
	class Matrix4
	{
	public:
		
		//Holds the transform matrix data in array form.

		real data[12];

		Matrix4()
        {
            data[1] = data[2] = data[3] = data[4] = data[6] =
                data[7] = data[8] = data[9] = data[11] = 0;
            data[0] = data[5] = data[10] = 1;
        }

		 /**
         * Sets the matrix to be a diagonal matrix with the given coefficients.
         */

		//대각 행렬
        void setDiagonal(real a, real b, real c)
        {
            data[0] = a;
            data[5] = b;
            data[10] = c;
        }

		 /**
         * Returns a matrix which is this matrix multiplied by the given
         * other matrix.
         */
        Matrix4 operator*(const Matrix4 &o) const
        {
            Matrix4 result;
            result.data[0] = (o.data[0]*data[0]) + (o.data[4]*data[1]) + (o.data[8]*data[2]);
            result.data[4] = (o.data[0]*data[4]) + (o.data[4]*data[5]) + (o.data[8]*data[6]);
            result.data[8] = (o.data[0]*data[8]) + (o.data[4]*data[9]) + (o.data[8]*data[10]);

            result.data[1] = (o.data[1]*data[0]) + (o.data[5]*data[1]) + (o.data[9]*data[2]);
            result.data[5] = (o.data[1]*data[4]) + (o.data[5]*data[5]) + (o.data[9]*data[6]);
            result.data[9] = (o.data[1]*data[8]) + (o.data[5]*data[9]) + (o.data[9]*data[10]);

            result.data[2] = (o.data[2]*data[0]) + (o.data[6]*data[1]) + (o.data[10]*data[2]);
            result.data[6] = (o.data[2]*data[4]) + (o.data[6]*data[5]) + (o.data[10]*data[6]);
            result.data[10] = (o.data[2]*data[8]) + (o.data[6]*data[9]) + (o.data[10]*data[10]);

            result.data[3] = (o.data[3]*data[0]) + (o.data[7]*data[1]) + (o.data[11]*data[2]) + data[3];
            result.data[7] = (o.data[3]*data[4]) + (o.data[7]*data[5]) + (o.data[11]*data[6]) + data[7];
            result.data[11] = (o.data[3]*data[8]) + (o.data[7]*data[9]) + (o.data[11]*data[10]) + data[11];

            return result;
        }


		        /**
         * Transform the given vector by this matrix.
         *
         * @param vector The vector to transform.
         */
        PVector3 operator*(const PVector3 &vector) const
        {
            return PVector3(
                vector.x * data[0] +
                vector.y * data[1] +
                vector.z * data[2] + data[3],

                vector.x * data[4] +
                vector.y * data[5] +
                vector.z * data[6] + data[7],

                vector.x * data[8] +
                vector.y * data[9] +
                vector.z * data[10] + data[11]
            );
        }

		        /**
         * Transform the given vector by this matrix.
         *
         * @param vector The vector to transform.
         */
        PVector3 transform(const PVector3 &vector) const
        {
            return (*this) * vector;
        }

		 /**
         * Returns the determinant of the matrix.
         */
        real getDeterminant() const;
		
		/**
         * Sets the matrix to be the inverse of the given matrix.
         *
         * @param m The matrix to invert and use to set this.
         */
        void setInverse(const Matrix4 &m);

		
        /** Returns a new matrix containing the inverse of this matrix. */
        Matrix4 inverse() const
        {
            Matrix4 result;
            result.setInverse(*this);
            return result;
        }
		 /**
         * Inverts the matrix.
         */
        void invert()
        {
            setInverse(*this);
        }


		        /**
         * Transform the given direction vector by this matrix.
         *
         * @note When a direction is converted between frames of
         * reference, there is no translation required.
         *
         * @param vector The vector to transform.
         */
        PVector3 transformDirection(const PVector3 &vector) const
        {
            return PVector3(
                vector.x * data[0] +
                vector.y * data[1] +
                vector.z * data[2],

                vector.x * data[4] +
                vector.y * data[5] +
                vector.z * data[6],

                vector.x * data[8] +
                vector.y * data[9] +
                vector.z * data[10]
            );
        }
		        /**
         * Transform the given direction vector by the
         * transformational inverse of this matrix.
         *
         * @note This function relies on the fact that the inverse of
         * a pure rotation matrix is its transpose. It separates the
         * translational and rotation components, transposes the
         * rotation, and multiplies out. If the matrix is not a
         * scale and shear free transform matrix, then this function
         * will not give correct results.
         *
         * @note When a direction is converted between frames of
         * reference, there is no translation required.
         *
         * @param vector The vector to transform.
         */
        PVector3 transformInverseDirection(const PVector3 &vector) const
        {
            return PVector3(
                vector.x * data[0] +
                vector.y * data[4] +
                vector.z * data[8],

                vector.x * data[1] +
                vector.y * data[5] +
                vector.z * data[9],

                vector.x * data[2] +
                vector.y * data[6] +
                vector.z * data[10]
            );
        }
		 /**
         * Transform the given vector by the transformational inverse
         * of this matrix.
         *
         * @note This function relies on the fact that the inverse of
         * a pure rotation matrix is its transpose. It separates the
         * translational and rotation components, transposes the
         * rotation, and multiplies out. If the matrix is not a
         * scale and shear free transform matrix, then this function
         * will not give correct results.
         *
         * @param vector The vector to transform.
         */
        PVector3 transformInverse(const PVector3 &vector) const
        {
            PVector3 tmp = vector;
            tmp.x -= data[3];
            tmp.y -= data[7];
            tmp.z -= data[11];
            return PVector3(
                tmp.x * data[0] +
                tmp.y * data[4] +
                tmp.z * data[8],

                tmp.x * data[1] +
                tmp.y * data[5] +
                tmp.z * data[9],

                tmp.x * data[2] +
                tmp.y * data[6] +
                tmp.z * data[10]
            );
        }

		 /**
         * Gets a vector representing one axis (i.e. one column) in the matrix.
         *
         * @param i The row to return. Row 3 corresponds to the position
         * of the transform matrix.
         *
         * @return The vector.
         */
        PVector3 getAxisVector(int i) const
        {
            return PVector3(data[i], data[i+4], data[i+8]);
        }

        /**
         * Sets this matrix to be the rotation matrix corresponding to
         * the given quaternion.
         */
        void setOrientationAndPos(const Quaternion &q, const PVector3 &pos)
        {
            data[0] = 1 - (2*q.j*q.j + 2*q.k*q.k);
            data[1] = 2*q.i*q.j + 2*q.k*q.r;
            data[2] = 2*q.i*q.k - 2*q.j*q.r;
            data[3] = pos.x;

            data[4] = 2*q.i*q.j - 2*q.k*q.r;
            data[5] = 1 - (2*q.i*q.i  + 2*q.k*q.k);
            data[6] = 2*q.j*q.k + 2*q.i*q.r;
            data[7] = pos.y;

            data[8] = 2*q.i*q.k + 2*q.j*q.r;
            data[9] = 2*q.j*q.k - 2*q.i*q.r;
            data[10] = 1 - (2*q.i*q.i  + 2*q.j*q.j);
            data[11] = pos.z;
        }

		 ///**
   //      * Fills the given array with this transform matrix, so it is
   //      * usable as an open-gl transform matrix. OpenGL uses a column
   //      * major format, so that the values are transposed as they are
   //      * written.
   //      */
   //     void fillGLArray(float array[16]) const
   //     {
   //         array[0] = (float)data[0];
   //         array[1] = (float)data[4];
   //         array[2] = (float)data[8];
   //         array[3] = (float)0;

   //         array[4] = (float)data[1];
   //         array[5] = (float)data[5];
   //         array[6] = (float)data[9];
   //         array[7] = (float)0;

   //         array[8] = (float)data[2];
   //         array[9] = (float)data[6];
   //         array[10] = (float)data[10];
   //         array[11] = (float)0;

   //         array[12] = (float)data[3];
   //         array[13] = (float)data[7];
   //         array[14] = (float)data[11];
   //         array[15] = (float)1;
   //     }

		//DX
		void fillDXArray(float array[16])const
		{
			array[0] = (float)data[0];
			array[1] = (float)data[1];
			array[2] = (float)data[2];
			array[3] = (float)data[3];


			array[4] = (float)data[4];
			array[5] = (float)data[5];
			array[6] = (float)data[6];
			array[7] = (float)data[7];

			array[8] = (float)data[8];
			array[9] = (float)data[9];
			array[10] = (float)data[10];
			array[11] = (float)data[11];

			array[12] = (float)0;
			array[13] = (float)0;
			array[14] = (float)0;
			array[15] = (float)1;
		}
	};

	  /**
     * Holds an inertia tensor, consisting of a 3x3 row-major matrix.
     * This matrix is not padding to produce an aligned structure, since
     * it is most commonly used with a mass (single real) and two
     * damping coefficients to make the 12-element characteristics array
     * of a rigid body.
     */

	class Matrix3
	{
	public:
		real data[9];

		Matrix3()
        {
            data[0] = data[1] = data[2] = data[3] = data[4] = data[5] =
                data[6] = data[7] = data[8] = 0;
        }

		 /**
         * Creates a new matrix with the given three vectors making
         * up its columns.
         */
        Matrix3(const PVector3 &compOne, const PVector3 &compTwo,
            const PVector3 &compThree)
        {
            setComponents(compOne, compTwo, compThree);
        }

		 /**
         * Creates a new matrix with explicit coefficients.
         */
        Matrix3(real c0, real c1, real c2, real c3, real c4, real c5,
            real c6, real c7, real c8)
        {
            data[0] = c0; data[1] = c1; data[2] = c2;
            data[3] = c3; data[4] = c4; data[5] = c5;
            data[6] = c6; data[7] = c7; data[8] = c8;
        }
		 /**
         * Sets the matrix to be a diagonal matrix with the given
         * values along the leading diagonal.
         */

        void setDiagonal(real a, real b, real c)
        {
            setInertiaTensorCoeffs(a, b, c);
        }
		/*
			Inertia tensor - 관성 텐서
			일반적으로 강체가 회전운동을 할 때 각운동량과 각속도 사이의 관계를 나타내는 텐서이다.
			병진운동에서의 질량과 비슷하게 생각할 수 있는 양으로, 회전운동에서의 묵직한 정도를 뜻한다. 
			관성텐서의 6개 성분 중 3개는 관성모멘트, 3개는 관성곱을 의미한다. 
			


			병진운동에 대해서 질량과 대응하는 회전운동의 '묵직한 정도(inertia)'를 말한다. 강체가 회전운동을 할 때, 각속도벡터의 방향은 항상 회전축의 방향과 일치한다. 
			그러나 보통 각운동량벡터의 방향은 회전축과 일치하지 않으며, 각운동량벡터의 x성분은 각속도의 x성분 뿐 아니라 y성분이나 z성분과도 관계가 있다. 
			마찬가지로 각운동량벡터의 y성분, z성분도 제각기 각속도벡터의 3가지 성분과 관계가 있다.

			각운동량 H의 성분을 Hx,Hy,Hz로 나타내고 각속도 ω의 성분을 ωx,ωy,ωz로 나타내면 
			 Hx＝Ixxωx＋Ixyωy＋Ixzωz 
			 Hy＝Iyxωx＋Iyyωy＋Iyzωz 
			  Hz＝Izxωx＋Izyωy＋Izzωz 
			의 관계가 성립한다. 위의 식 중 9개 상수의 짝을 관성텐서라고 한다. 
			관성텐서는 대칭텐서이므로 Ixy＝Iyx, Iyz＝Izy, Izx＝Ixz 라는 관계가 성립된다. 
			따라서 관성텐서의 독립된 성분은 6개이다. 
			관성텐서의 성분 중에서 Ixx, Iyy, Izz는 각각 x축, y축, z축에 관한 관성모멘트라고 하며,
			－Ixy,－Iyz,－Izx는 각각 x축과 y축, y축과 z축, z축과 x축에 관한 관성곱이라고 한다. 
		*/

		 void setInertiaTensorCoeffs(real ix, real iy, real iz,
            real ixy=0, real ixz=0, real iyz=0)
		{
            data[0] = ix;
            data[1] = data[3] = -ixy;
            data[2] = data[6] = -ixz;
            data[4] = iy;
            data[5] = data[7] = -iyz;
            data[8] = iz;
        }

		 /**
         * Sets the value of the matrix as an inertia tensor of
         * a rectangular block aligned with the body's coordinate
         * system with the given axis half-sizes and mass.
         */
        void setBlockInertiaTensor(const PVector3 &halfSizes, real mass)
        {
            PVector3 squares = halfSizes.componentProduct(halfSizes);
            setInertiaTensorCoeffs(0.3f*mass*(squares.y + squares.z),
                0.3f*mass*(squares.x + squares.z),
                0.3f*mass*(squares.x + squares.y));
        }
		 /**
         * Sets the matrix to be a skew symmetric matrix based on
         * the given vector. The skew symmetric matrix is the equivalent
         * of the vector product. So if a,b are vectors. a x b = A_s b
         * where A_s is the skew symmetric form of a.
         */
        void setSkewSymmetric(const PVector3 vector)
        {
            data[0] = data[4] = data[8] = 0;
            data[1] = -vector.z;
            data[2] = vector.y;
            data[3] = vector.z;
            data[5] = -vector.x;
            data[6] = -vector.y;
            data[7] = vector.x;
        }

		 /**
         * Sets the matrix values from the given three vector components.
         * These are arranged as the three columns of the vector.
         */
        void setComponents(const PVector3 &compOne, const PVector3 &compTwo,
            const PVector3 &compThree)
        {
            data[0] = compOne.x;
            data[1] = compTwo.x;
            data[2] = compThree.x;
            data[3] = compOne.y;
            data[4] = compTwo.y;
            data[5] = compThree.y;
            data[6] = compOne.z;
            data[7] = compTwo.z;
            data[8] = compThree.z;

        }

		 /**
         * Transform the given vector by this matrix.
         *
         * @param vector The vector to transform.
         */
        PVector3 operator*(const PVector3 &vector) const
        {
            return PVector3(
                vector.x * data[0] + vector.y * data[1] + vector.z * data[2],
                vector.x * data[3] + vector.y * data[4] + vector.z * data[5],
                vector.x * data[6] + vector.y * data[7] + vector.z * data[8]
            );
        }

        /**
         * Transform the given vector by this matrix.
         *
         * @param vector The vector to transform.
         */
        PVector3 transform(const PVector3 &vector) const
        {
            return (*this) * vector;
        }

		
        /**
         * Transform the given vector by the transpose of this matrix.
         *
         * @param vector The vector to transform.
         */
        PVector3 transformTranspose(const PVector3 &vector) const
        {
            return PVector3(
                vector.x * data[0] + vector.y * data[3] + vector.z * data[6],
                vector.x * data[1] + vector.y * data[4] + vector.z * data[7],
                vector.x * data[2] + vector.y * data[5] + vector.z * data[8]
            );
        }

        /**
         * Gets a vector representing one row in the matrix.
         *
         * @param i The row to return.
         */
        PVector3 getRowVector(int i) const
        {
            return PVector3(data[i*3], data[i*3+1], data[i*3+2]);
        }


		
   //  * Gets a vector representing one axis (i.e. one column) in the matrix.
   //  *
   //  * @param i The row to return.
   //  *
   //  * @return The vector.
   //      
        PVector3 getColumVector(int i) const
        {
            return PVector3(data[i], data[i+3], data[i+6]);
        }

		//DX //Row 
		PVector3 getAxisVector(int i)const
		{
			return PVector3(data[i], data[i + 1], data[i + 2]);
		}

		        /**
         * Sets the matrix to be the inverse of the given matrix.
         *
         * @param m The matrix to invert and use to set this.
         */
        void setInverse(const Matrix3 &m)
        {
            real t4 = m.data[0]*m.data[4];
            real t6 = m.data[0]*m.data[5];
            real t8 = m.data[1]*m.data[3];
            real t10 = m.data[2]*m.data[3];
            real t12 = m.data[1]*m.data[6];
            real t14 = m.data[2]*m.data[6];

            // Calculate the determinant
            real t16 = (t4*m.data[8] - t6*m.data[7] - t8*m.data[8]+
                        t10*m.data[7] + t12*m.data[5] - t14*m.data[4]);

            // Make sure the determinant is non-zero.
            if (t16 == (real)0.0f) return;
            real t17 = 1/t16;

            data[0] = (m.data[4]*m.data[8]-m.data[5]*m.data[7])*t17;
            data[1] = -(m.data[1]*m.data[8]-m.data[2]*m.data[7])*t17;
            data[2] = (m.data[1]*m.data[5]-m.data[2]*m.data[4])*t17;
            data[3] = -(m.data[3]*m.data[8]-m.data[5]*m.data[6])*t17;
            data[4] = (m.data[0]*m.data[8]-t14)*t17;
            data[5] = -(t6-t10)*t17;
            data[6] = (m.data[3]*m.data[7]-m.data[4]*m.data[6])*t17;
            data[7] = -(m.data[0]*m.data[7]-t12)*t17;
            data[8] = (t4-t8)*t17;
        }

		 /** Returns a new matrix containing the inverse of this matrix. */
        Matrix3 inverse() const
        {
            Matrix3 result;
            result.setInverse(*this);
            return result;
        }


		 /**
         * Inverts the matrix.
         */
        void invert()
        {
            setInverse(*this);
        }
		 /**
         * Sets the matrix to be the transpose of the given matrix.
         *
         * @param m The matrix to transpose and use to set this.
         */
        void setTranspose(const Matrix3 &m)
        {
            data[0] = m.data[0];
            data[1] = m.data[3];
            data[2] = m.data[6];
            data[3] = m.data[1];
            data[4] = m.data[4];
            data[5] = m.data[7];
            data[6] = m.data[2];
            data[7] = m.data[5];
            data[8] = m.data[8];
        }

		 /** Returns a new matrix containing the transpose of this matrix. */
        Matrix3 transpose() const
        {
            Matrix3 result;
            result.setTranspose(*this);
            return result;
        }

		 /**
         * Returns a matrix which is this matrix multiplied by the given
         * other matrix.
         */
        Matrix3 operator*(const Matrix3 &o) const
        {
            return Matrix3(
                data[0]*o.data[0] + data[1]*o.data[3] + data[2]*o.data[6],
                data[0]*o.data[1] + data[1]*o.data[4] + data[2]*o.data[7],
                data[0]*o.data[2] + data[1]*o.data[5] + data[2]*o.data[8],

                data[3]*o.data[0] + data[4]*o.data[3] + data[5]*o.data[6],
                data[3]*o.data[1] + data[4]*o.data[4] + data[5]*o.data[7],
                data[3]*o.data[2] + data[4]*o.data[5] + data[5]*o.data[8],

                data[6]*o.data[0] + data[7]*o.data[3] + data[8]*o.data[6],
                data[6]*o.data[1] + data[7]*o.data[4] + data[8]*o.data[7],
                data[6]*o.data[2] + data[7]*o.data[5] + data[8]*o.data[8]
                );
        }


		/**
         * Multiplies this matrix in place by the given other matrix.
         */
        void operator*=(const Matrix3 &o)
        {
            real t1;
            real t2;
            real t3;

            t1 = data[0]*o.data[0] + data[1]*o.data[3] + data[2]*o.data[6];
            t2 = data[0]*o.data[1] + data[1]*o.data[4] + data[2]*o.data[7];
            t3 = data[0]*o.data[2] + data[1]*o.data[5] + data[2]*o.data[8];
            data[0] = t1;
            data[1] = t2;
            data[2] = t3;

            t1 = data[3]*o.data[0] + data[4]*o.data[3] + data[5]*o.data[6];
            t2 = data[3]*o.data[1] + data[4]*o.data[4] + data[5]*o.data[7];
            t3 = data[3]*o.data[2] + data[4]*o.data[5] + data[5]*o.data[8];
            data[3] = t1;
            data[4] = t2;
            data[5] = t3;

            t1 = data[6]*o.data[0] + data[7]*o.data[3] + data[8]*o.data[6];
            t2 = data[6]*o.data[1] + data[7]*o.data[4] + data[8]*o.data[7];
            t3 = data[6]*o.data[2] + data[7]*o.data[5] + data[8]*o.data[8];
            data[6] = t1;
            data[7] = t2;
            data[8] = t3;
        }


		 /**
         * Multiplies this matrix in place by the given scalar.
         */
        void operator*=(const real scalar)
        {
            data[0] *= scalar; data[1] *= scalar; data[2] *= scalar;
            data[3] *= scalar; data[4] *= scalar; data[5] *= scalar;
            data[6] *= scalar; data[7] *= scalar; data[8] *= scalar;
        }

        /**
         * Does a component-wise addition of this matrix and the given
         * matrix.
         */
        void operator+=(const Matrix3 &o)
        {
            data[0] += o.data[0]; data[1] += o.data[1]; data[2] += o.data[2];
            data[3] += o.data[3]; data[4] += o.data[4]; data[5] += o.data[5];
            data[6] += o.data[6]; data[7] += o.data[7]; data[8] += o.data[8];
        }

        /**
         * Sets this matrix to be the rotation matrix corresponding to
         * the given quaternion.
         */
        void setOrientation(const Quaternion &q)
        {
            data[0] = 1 - (2*q.j*q.j + 2*q.k*q.k);
            data[1] = 2*q.i*q.j + 2*q.k*q.r;
            data[2] = 2*q.i*q.k - 2*q.j*q.r;
            data[3] = 2*q.i*q.j - 2*q.k*q.r;
            data[4] = 1 - (2*q.i*q.i  + 2*q.k*q.k);
            data[5] = 2*q.j*q.k + 2*q.i*q.r;
            data[6] = 2*q.i*q.k + 2*q.j*q.r;
            data[7] = 2*q.j*q.k - 2*q.i*q.r;
            data[8] = 1 - (2*q.i*q.i  + 2*q.j*q.j);
        }

        /**
         * Interpolates a couple of matrices.
         */
        static Matrix3 linearInterpolate(const Matrix3& a, const Matrix3& b, real prop);


	};

}