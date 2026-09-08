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
    guided_openmp::MatrixArray matrices;
    for (auto& matrix : matrices) {
        matrix = guided_openmp::Matrix(2, 2);
        matrix.set(0, 0, 1.001);
        matrix.set(0, 1, 0.0);
        matrix.set(1, 0, 0.0);
        matrix.set(1, 1, 1.001);
    }
    matrices[0].set(0, 0, 1.0);
    matrices[0].set(0, 1, 2.0);
    matrices[0].set(1, 0, 3.0);
    matrices[0].set(1, 1, 4.0);
    matrices[1].set(0, 0, 5.0);
    matrices[1].set(0, 1, 6.0);
    matrices[1].set(1, 0, 7.0);
    matrices[1].set(1, 1, 8.0);

    const auto product = guided_openmp::multiplyMatrices(matrices, 2);
    double scale = 1.0;
    for (std::size_t index = 2; index < matrices.size(); ++index) {
        scale *= 1.001;
    }
    EXPECT_NEAR(product.get(0, 0), 19.0 * scale, 1e-10);
    EXPECT_NEAR(product.get(0, 1), 22.0 * scale, 1e-10);
    EXPECT_NEAR(product.get(1, 0), 43.0 * scale, 1e-10);
    EXPECT_NEAR(product.get(1, 1), 50.0 * scale, 1e-10);
}

TEST(MatrixProduct, RejectsInvalidThreadCounts) {
    const auto matrices = guided_openmp::createRandomMatrices(1, 625);
    EXPECT_THROW(guided_openmp::multiplyMatrices(matrices, 0), std::invalid_argument);
    EXPECT_THROW(guided_openmp::multiplyMatrices(matrices, 3), std::invalid_argument);
}

TEST(MatrixProduct, RejectsZeroSizedMatrices) {
    EXPECT_THROW(guided_openmp::createRandomMatrices(0), std::invalid_argument);
}
