
#include <vector>
#include "Platform.h"

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

	};

	ISHMatrix(uint32 m, uint32 n)
		: m_numRows(m),
		m_numColumns(n)
	{
		ComputeEmptyMatrix();
	};

	ISHMatrix(uint32 _Rows, uint32 _Columns, std::vector<float> members)
		: m_numColumns(_Columns),
		m_numRows(_Rows)
	{
		ComputeMatrixFromVector(members);
	};

	ISHMatrix(std::vector<std::vector<ISHMatrixComponent*>>& Incomponents)
	{
		ComputeMatriFromMatrixComponents(Incomponents);
	};

	// Columns
	uint32 m_numColumns = 0;

	//Rows
	uint32 m_numRows = 0;
	std::vector<std::vector<ISHMatrixComponent*>> m_MatrixComponents;


	float	   CalculateDeterminant();
	ISHMatrix* ComputeMatrixMinor(float i, float j);
	ISHMatrix* ComputeTranspose();
	ISHMatrix* ComputeInverse();
	ISHMatrix* ComputeAdjunt();
	bool			   IsQuadratic();
	bool			   IsInvertible();

	const float& GetElementValue(uint32 i, uint32 j);
	void SetElementValue(float i, float j, float newValue);

	// Helper function for constructing a new matrix with a certain m rows and n columns
	void  ComputeEmptyMatrix();

	inline ISHMatrix* operator * (float num)
	{
		for (auto& rows : m_MatrixComponents)
		{
			for (auto& elements : rows)
			{

				elements->val *= num;

			}
		}

		return this;
	}

	void operator = (ISHMatrix* matrix)
	{
		m_MatrixComponents = matrix->m_MatrixComponents;
	}
private:

	//  Helper function that creates a matrix ( constructor ).
	void ComputeMatrixFromVector(std::vector<float>& InComponentsValues);
	void ComputeMatriFromMatrixComponents(std::vector<std::vector<ISHMatrixComponent*>>& compononents);
};