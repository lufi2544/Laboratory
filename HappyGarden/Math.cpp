#include "Math.h"
#include "Assertion.h"
#include <math.h>


//Make i, j uint32

const float& ISHMatrix::GetElementValue(uint32 i, uint32 j)
{
	return m_MatrixComponents[i][j]->val;
}

void ISHMatrix::SetElementValue(float i, float j, float newValue)
{
	m_MatrixComponents[i][j]->val = newValue;
}

// TODO calculate the determinant.
float ISHMatrix::CalculateDeterminant()
{

	if (!IsQuadratic())
	{
		ASSERT(IsQuadratic());
		return -1;
	}

	float det = 0;

	// We just have a 1X1 Matrix here
	if ((m_numRows == 1) && (m_numColumns == 1))
	{
		return GetElementValue(0, 0);
	}

	//  Have in mind that the math formula is meant to be used with matreces with the format
	//  ( first element of the matrix as (1,1), but we are using the (0,0) 
	//  that is why we add 1 in the PowValue local variable  )
	for (uint32 ItColumn = 0; ItColumn < m_numColumns; ++ItColumn)
	{

		float Value = m_MatrixComponents[0][ItColumn]->val;
		float PowValue = pow(-1, (1 + (m_MatrixComponents[0][ItColumn]->j + 1)));
		std::unique_ptr<ISHMatrix> MatrixMinor = std::unique_ptr<ISHMatrix>(ComputeMatrixMinor(0, ItColumn));
		float RecursiveDeterminant = MatrixMinor.get()->CalculateDeterminant();

		det += Value * PowValue * RecursiveDeterminant;
	}

	return det;
}

ISHMatrix* ISHMatrix::ComputeMatrixMinor(float _i, float _j)
{


	ASSERT(m_numRows > 0 && m_numColumns > 0);
	if (!(m_numRows > 0 && m_numColumns > 0))
	{
		return nullptr;
	}

	if (!IsQuadratic())
	{
		ASSERT(IsQuadratic());
		return nullptr;
	}


	if ((m_numRows == 1) && m_numColumns == 1)
	{
		// If we have a 1X1 matrix, we just return the matrix itself
		std::vector<float> Row;
		Row.push_back(GetElementValue(0, 0));
		return new ISHMatrix(1, 1, Row);
	}


	std::vector<std::vector<ISHMatrixComponent*>> MinorMatrixValues;
	for (int it_row = 0; it_row < m_numRows; ++it_row)
	{
		std::vector<ISHMatrixComponent*> MinorMatrixRow;

		for (int it_column = 0; it_column < m_numColumns; ++it_column)
		{
			float i = m_MatrixComponents[it_row][it_column]->i;
			float j = m_MatrixComponents[it_row][it_column]->j;

			if (i != _i && j != _j)
			{
				ISHMatrixComponent* component = new ISHMatrixComponent(m_MatrixComponents[it_row][it_column]->val);
				MinorMatrixRow.push_back(component);

			}
		}
		if (MinorMatrixRow.size() > 0)
		{
			MinorMatrixValues.push_back(MinorMatrixRow);
		}

	}

	auto a = new ISHMatrix(MinorMatrixValues);
	return a;
}

ISHMatrix* ISHMatrix::ComputeTranspose()
{

	ISHMatrix matrixCopy = *this;
	ISHMatrix* transpose = new ISHMatrix(m_numColumns, m_numRows);

	// Change the i and j of the components of the matrix, then we just compute the new matrix.	
	for (int r = 0; r < matrixCopy.m_numRows; ++r)
	{
		for (int c = 0; c < matrixCopy.m_numColumns; ++c)
		{

			float n_i = matrixCopy.m_MatrixComponents[r][c]->j;
			float n_j = matrixCopy.m_MatrixComponents[r][c]->j;

			matrixCopy.m_MatrixComponents[r][c]->i = n_i;
			matrixCopy.m_MatrixComponents[r][c]->j = n_j;

			transpose->m_MatrixComponents[c][r] = matrixCopy.m_MatrixComponents[r][c];
		}
	}

	return transpose;
}

ISHMatrix* ISHMatrix::ComputeInverse()
{
	ISHMatrix* returnMatrix = new ISHMatrix();

	if (!IsQuadratic() || !IsInvertible())
	{

		std::cout << "The matrix has no inverse." << std::endl;

		return nullptr;
	}
	std::unique_ptr<ISHMatrix> MatrixAdjunt = std::unique_ptr<ISHMatrix>(ComputeAdjunt());
	returnMatrix = *MatrixAdjunt.get()->ComputeTranspose() * ((float)1 / CalculateDeterminant());
	return returnMatrix;
}

ISHMatrix* ISHMatrix::ComputeAdjunt()
{

	if (!IsQuadratic())
	{
		return nullptr;
	}

	ISHMatrix* returnMatrix = new ISHMatrix(m_numRows, m_numColumns);

	for (auto rows : m_MatrixComponents)
	{

		// Same case with the adjunt and the determinant when it comes to the format of the first matrix element.
		for (auto elements : rows)
		{

			std::unique_ptr<ISHMatrix> Matrix = std::unique_ptr<ISHMatrix>(ComputeMatrixMinor((elements)->i, elements->j));
			float a = Matrix->CalculateDeterminant();
			float i = std::pow(-1, (elements->i + 1 + elements->j + 1));
			float n_elementVal = i * a;

			returnMatrix->SetElementValue(elements->i, elements->j, n_elementVal);
		}

	}

	return returnMatrix;
}

bool ISHMatrix::IsQuadratic()
{
	return (m_numRows == m_numColumns);
}

bool ISHMatrix::IsInvertible()
{
	return CalculateDeterminant() != 0;
}

void ISHMatrix::ComputeEmptyMatrix()
{

	for (uint32 ItRow = 0; ItRow < m_numRows; ++ItRow)
	{
		std::vector<ISHMatrixComponent*>l_Row;

		for (uint32 ItColumn = 0; ItColumn < m_numColumns; ++ItColumn)
		{

			l_Row.push_back(new ISHMatrixComponent(ItRow, ItColumn, 0));

		}

		m_MatrixComponents.push_back(l_Row);
	}
}

void ISHMatrix::ComputeMatrixFromVector(std::vector<float>& InComponentValues)
{
	int matrixSize = m_numColumns * m_numRows;
	ASSERT(InComponentValues.size() <= matrixSize);

	for (int RowIdx = 0; RowIdx < m_numRows; ++RowIdx)
	{
		std::vector<ISHMatrixComponent*> Row;
		for (int ColumnIdx = 0; ColumnIdx < m_numColumns; ++ColumnIdx)
		{

			if (InComponentValues.size() < matrixSize)
			{
				uint32 diff = matrixSize - InComponentValues.size();
				// the matrix rows do not match the InMatrixRow size, so we add 0 in the positions
				// that would be blank.
				for (uint32 Idx = 0; Idx < diff; ++Idx)
				{
					InComponentValues.push_back(0);
				}
			}

			auto Idx = (m_numColumns * RowIdx) + ColumnIdx;
			Row.push_back((new ISHMatrixComponent(RowIdx, ColumnIdx, InComponentValues[Idx])));


		}

		m_MatrixComponents.push_back(Row);
	}


	// We adjust the columns if necessary
	if (m_numRows == 1)
	{
		m_numColumns = m_numRows;
	}
}

void ISHMatrix::ComputeMatriFromMatrixComponents(std::vector<std::vector<ISHMatrixComponent*>>& compononents)
{
	int RowIdx = 0;
	auto it_rows = compononents.begin();

	m_numColumns = it_rows->size();
	m_numRows = compononents.size();
	m_MatrixComponents = compononents;

	while (it_rows != compononents.end())
	{
		int ColumnIdx = 0;
		auto it_columns = it_rows->begin();

		while (it_columns != it_rows->end())
		{

			(*it_columns)->i = RowIdx;
			(*it_columns)->j = ColumnIdx;
			++it_columns;
			++ColumnIdx;
		}
		++RowIdx;
		++it_rows;
	}

}
