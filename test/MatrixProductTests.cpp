#include "guided_openmp/MatrixProduct.hpp"

#include <gtest/gtest.h>

TEST(MatrixProduct, CreatesTwoHundredMatrices) {
    const auto matrices = guided_openmp::createRandomMatrices(2, 625);
    EXPECT_EQ(matrices.size(), 200);
    for (const auto& matrix : matrices) {
        EXPECT_EQ(matrix.getNumRows(), 2);
        EXPECT_EQ(matrix.getNumCols(), 2);
    }
}

TEST(MatrixProduct, MultipliesEveryMatrix) {
    const auto matrices = guided_openmp::createRandomMatrices(1, 625);
    const auto product = guided_openmp::multiplyMatrices(matrices);

    double expected = 1.0;
    for (const auto& matrix : matrices) {
        expected *= matrix.get(0, 0);
    }
    EXPECT_DOUBLE_EQ(product.get(0, 0), expected);
}

TEST(MatrixProduct, RejectsZeroSizedMatrices) {
    EXPECT_THROW(guided_openmp::createRandomMatrices(0), std::invalid_argument);
}
