
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
	ISHMatrix(ISHMatrix* m)
	{
		*this = *m;
	}

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

	ISHMatrix(std::vector<std::vector<std::shared_ptr<ISHMatrixComponent>>>& Incomponents)
	{
		ComputeMatriFromMatrixComponents(Incomponents);
	};

	~ISHMatrix()
	{
		m_MatrixComponents.clear();
	}

	// Columns
	uint32 m_numColumns = 0;

	//Rows
	uint32 m_numRows = 0;
	std::vector<std::vector<std::shared_ptr<ISHMatrixComponent>>> m_MatrixComponents;

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
	void  ComputeEmptyMatrix();

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
private:

	//  Helper function that creates a matrix ( constructor ).
	void ComputeMatrixFromVector(std::vector<float>& InComponentsValues);
	void ComputeMatriFromMatrixComponents(std::vector<std::vector<std::shared_ptr<ISHMatrixComponent>>>& compononents);
};