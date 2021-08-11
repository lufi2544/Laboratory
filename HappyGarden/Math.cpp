#include "Math.h"
#include "Assertion.h"
#include <math.h>



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
		std::unique_ptr<ISHMatrix> MatrixMinor = ComputeMatrixMinor(0, ItColumn);
		float RecursiveDeterminant = MatrixMinor->CalculateDeterminant();

		det += Value * PowValue * RecursiveDeterminant;
	}

	return det;
}

std::unique_ptr<ISHMatrix> ISHMatrix::ComputeMatrixMinor(float _i, float _j)
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
		return std::make_unique<ISHMatrix>(1, 1, Row);
	}


	std::vector<std::vector<std::shared_ptr<ISHMatrixComponent>>> MinorMatrixValues;
	for (int r = 0; r < m_numRows; ++r)
	{
		std::vector<std::shared_ptr<ISHMatrixComponent>> MinorMatrixRow;

		for (int c = 0; c < m_numColumns; ++c)
		{
			float i = m_MatrixComponents[r][c]->i;
			float j = m_MatrixComponents[r][c]->j;

			if (i != _i && j != _j)
			{
				MinorMatrixRow.push_back(std::make_shared<ISHMatrixComponent>(m_MatrixComponents[r][c]->val));
			}
		}
		if (MinorMatrixRow.size() > 0)
		{
			MinorMatrixValues.push_back(MinorMatrixRow);
		}

	}

	return std::make_unique<ISHMatrix>(MinorMatrixValues);
}

std::unique_ptr<ISHMatrix> ISHMatrix::ComputeTranspose()
{

	ISHMatrix matrixCopy = *this;
	std::unique_ptr<ISHMatrix> transpose = std::make_unique<ISHMatrix>(m_numColumns, m_numRows);

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
			transpose->m_MatrixComponents[c][r]->i = c;
			transpose->m_MatrixComponents[c][r]->j = r;
		}
	}

	return transpose;
}

std::unique_ptr<ISHMatrix> ISHMatrix::ComputeInverse()
{

	if (!IsQuadratic() || !IsInvertible())
	{

		std::cout << "The matrix has no inverse." << std::endl;

		return nullptr;
	}

	return (*ComputeAdjunt()->ComputeTranspose() * ((float)1 / CalculateDeterminant()));
}

std::unique_ptr<ISHMatrix> ISHMatrix::ComputeAdjunt()
{

	if (!IsQuadratic())
	{
		return nullptr;
	}

	std::unique_ptr<ISHMatrix> returnMatrix = std::make_unique<ISHMatrix>(m_numRows, m_numColumns);

	for (auto rows : m_MatrixComponents)
	{
		// Same case with the adjunt and the determinant when it comes to the format of the first matrix element.
		for (auto elements : rows)
		{
			std::unique_ptr<ISHMatrix> Matrix = ComputeMatrixMinor((elements)->i, elements->j);
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

void ISHMatrix::ComputeMatrixFromVector(std::vector<float>& InComponentValues)
{
	int matrixSize = m_numColumns * m_numRows;
	ASSERT(InComponentValues.size() <= matrixSize);

	for (int RowIdx = 0; RowIdx < m_numRows; ++RowIdx)
	{
		std::vector<std::shared_ptr<ISHMatrixComponent>> Row;
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
			Row.push_back((std::make_shared<ISHMatrixComponent>(RowIdx, ColumnIdx, InComponentValues[Idx])));


		}

		m_MatrixComponents.push_back(Row);
	}


	// We adjust the columns if necessary
	if (m_numRows == 1)
	{
		m_numColumns = m_numRows;
	}
}

void ISHMatrix::ComputeMatriFromMatrixComponents(std::vector<std::vector<std::shared_ptr<ISHMatrixComponent>>>& compononents)
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

std::ostream& operator << (std::ostream& os, ISHMatrix matrix)
{


	float u = 1;
	std::vector<std::vector<std::shared_ptr<ISHMatrixComponent>>>::iterator it_MatrixRows = matrix.m_MatrixComponents.begin();

	while (it_MatrixRows != matrix.m_MatrixComponents.end())
	{
		std::vector<std::shared_ptr<ISHMatrixComponent>>::iterator it_it_MatrixRows = (*it_MatrixRows).begin();

		while (it_it_MatrixRows != (*it_MatrixRows).end())
		{

			if (it_it_MatrixRows->get()->i != u)
			{
				u = it_it_MatrixRows->get()->i;
				std::cout << std::endl;

			}

			std::cout << it_it_MatrixRows->get()->val << "  ";

			it_it_MatrixRows++;
		}


		it_MatrixRows++;
	}

	return os;
};

