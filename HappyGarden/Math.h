
#include <vector>
#include "Platform.h"

struct ISHMatrixComponent;

namespace MathTypeDefs
{
    typedef std::vector<std::vector<std::shared_ptr<ISHMatrixComponent>>> MatrixComponentVec;
}

using namespace MathTypeDefs;

struct ISHMatrixComponent
{
	ISHMatrixComponent()
	{
		i = 1;
		j = 1;
		val = 0;

	};

	ISHMatrixComponent(float _i, float _j, float _val)
		: i(_i),
		j(_j),
		val(_val)
	{};

	ISHMatrixComponent(float _val)
		: i(-1),
		j(-1),
		val(_val)
	{};

	float		i;
	float		j;
	float	  val;
};

struct ISHMatrix
{

	ISHMatrix()
		: ISHMatrix(1, 1)
	{
	}

	ISHMatrix(ISHMatrix* m)
	{
		*this = *m;
	}

    void Power();

	ISHMatrix(uint32 m, uint32 n)
		: m_numRows(m),
		m_numColumns(n)
	{
        for (uint32 r = 0; r < m_numRows; ++r)
        {
            std::vector<std::shared_ptr<ISHMatrixComponent>>l_Row;

            for (uint32 c = 0; c < m_numColumns; ++c)
            {

                l_Row.push_back(std::make_shared<ISHMatrixComponent>(r, c, 0));

            }

            m_MatrixComponents.push_back(l_Row);
        }
	}

	ISHMatrix(uint32 _Rows, uint32 _Columns, std::vector<float> members)
		: m_numColumns(_Columns),
		m_numRows(_Rows)
	{
		ComputeMatrixFromVector(members);
	}

	ISHMatrix(MatrixComponentVec& Incomponents)
	{
		ComputeMatriFromMatrixComponents(Incomponents);
	}

	~ISHMatrix()
	{
		m_MatrixComponents.clear();
	}

	// Columns
	uint32 m_numColumns = 0;

	//Rows
	uint32 m_numRows = 0;
    MatrixComponentVec m_MatrixComponents;

	//We could return raw pointers here as the Matrix is treaded as
	//a non-dynamically allocated object, the API uses smart pointers.

	std::unique_ptr<ISHMatrix> ComputeMatrixMinor(float i, float j);
	std::unique_ptr<ISHMatrix> ComputeTranspose();
	std::unique_ptr<ISHMatrix> ComputeInverse();
	std::unique_ptr<ISHMatrix> ComputeAdjunt();
	float	   CalculateDeterminant();
	bool			    IsQuadratic();
	bool			   IsInvertible();

	const float& GetElementValue(uint32 i, uint32 j);
	void SetElementValue(float i, float j, float newValue);

	// Helper function for constructing a new matrix with a certain m rows and n columns
    inline void  ComputeEmptyMatrix()
     {
         for (uint32 r = 0; r < m_numRows; ++r)
         {
             std::vector<std::shared_ptr<ISHMatrixComponent>>l_Row;

             for (uint32 c = 0; c < m_numColumns; ++c)
             {

                 l_Row.push_back(std::make_shared<ISHMatrixComponent>(r, c, 0));

             }

             m_MatrixComponents.push_back(l_Row);
         }
     }

	inline std::unique_ptr<ISHMatrix> operator * (float num)
	{
		for (auto& rows : m_MatrixComponents)
		{
			for (auto& elements : rows)
			{

				elements->val *= num;

			}
		}

		return std::make_unique<ISHMatrix>(this);
	}

	void operator = (ISHMatrix* matrix)
	{
		m_MatrixComponents = matrix->m_MatrixComponents;
		m_numColumns = matrix->m_numColumns;
		m_numRows = matrix->m_numRows;
	}


    void operator = (const ISHMatrix& matrix)
    {
        m_MatrixComponents = matrix.m_MatrixComponents;
        m_numColumns = matrix.m_numColumns;
        m_numRows = matrix.m_numRows;
    }

private:

	//  Helper function that creates a matrix ( constructor ).
	void ComputeMatrixFromVector(std::vector<float>& InComponentsValues);
	void ComputeMatriFromMatrixComponents(MatrixComponentVec& compononents);
};