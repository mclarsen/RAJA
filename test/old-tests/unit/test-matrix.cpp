//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-19, Lawrence Livermore National Security, LLC
// and RAJA project contributors. See the RAJA/COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

///
/// Source file containing tests for basic simd/simt vector operations
///

#include "RAJA/RAJA.hpp"
#include "RAJA_gtest.hpp"
#include <stdlib.h>

using MatrixTestTypes = ::testing::Types<

    // Test automatically wrapped types to make things easier for users
    RAJA::FixedMatrix<double, 4, 4>
  >;


template <typename NestedPolicy>
class MatrixTest : public ::testing::Test
{
protected:

  MatrixTest() = default;
  virtual ~MatrixTest() = default;

  virtual void SetUp()
  {
  }

  virtual void TearDown()
  {
  }
};
TYPED_TEST_SUITE_P(MatrixTest);


/*
 * We are using ((double)rand()/RAND_MAX) for input values so the compiler cannot do fancy
 * things, like constexpr out all of the intrinsics.
 */

TYPED_TEST_P(MatrixTest, MatrixCtor)
{

  using matrix_t = TypeParam;
  using element_t = typename matrix_t::element_type;

  matrix_t m_empty;
  ASSERT_FLOAT_EQ(m_empty.get(0,0), 0);

  matrix_t m_bcast(element_t(1));
  ASSERT_FLOAT_EQ(m_bcast.get(0,0), 1);

  matrix_t m_copy(m_bcast);
  ASSERT_FLOAT_EQ(m_copy.get(0,0), 1);


}

TYPED_TEST_P(MatrixTest, MatrixGetSet)
{

  using matrix_t = TypeParam;
  using element_t = typename matrix_t::element_type;

  matrix_t m;
  for(camp::idx_t i = 0;i < matrix_t::s_num_rows; ++ i){
    for(camp::idx_t j = 0;j < matrix_t::s_num_cols; ++ j){
      m.set(i,j, element_t(NO_OPT_ZERO + i+j*j));
      ASSERT_FLOAT_EQ(m.get(i,j), element_t(i+j*j));
    }
  }

  // Use assignment operator
  matrix_t m2;
  m2 = m;

  // Check values are same as m
  for(camp::idx_t i = 0;i < matrix_t::s_num_rows; ++ i){
    for(camp::idx_t j = 0;j < matrix_t::s_num_cols; ++ j){
      ASSERT_FLOAT_EQ(m2.get(i,j), element_t(i+j*j));
    }
  }

}

TYPED_TEST_P(MatrixTest, MatrixVector)
{

  using matrix_t = TypeParam;
  using element_t = typename matrix_t::element_type;
  using row_vector_t = typename matrix_t::row_vector_type;

  // initialize a matrix and vector
  matrix_t m;
  row_vector_t v;
  for(camp::idx_t j = 0;j < matrix_t::s_num_cols; ++ j){
    for(camp::idx_t i = 0;i < matrix_t::s_num_rows; ++ i){
      m.set(i,j, element_t(NO_OPT_ZERO + i+j*j));
    }
    v.set(j, j*2);
  }

  // matrix vector product
  // note mv is not necessarily the same type as v
  auto mv = m*v;

  // check result
  for(camp::idx_t i = 0;i < matrix_t::s_num_rows; ++ i){
    element_t expected(0);

    for(camp::idx_t j = 0;j < matrix_t::s_num_cols; ++ j){
      expected += m.get(i,j)*v[j];
    }

    ASSERT_FLOAT_EQ(mv[i], expected);
  }

}

TYPED_TEST_P(MatrixTest, MatrixMatrix)
{

  using A_t = TypeParam;
  using B_t = A_t;
  using element_t = typename A_t::element_type;

  // initialize two matrices
  A_t A;
  for(camp::idx_t j = 0;j < A_t::s_num_cols; ++ j){
    for(camp::idx_t i = 0;i < A_t::s_num_rows; ++ i){
      A.set(i,j, element_t(NO_OPT_ZERO + i+j*j));
    }
  }

  B_t B;
  for(camp::idx_t j = 0;j < B_t::s_num_cols; ++ j){
    for(camp::idx_t i = 0;i < B_t::s_num_rows; ++ i){
      B.set(i,j, element_t(NO_OPT_ZERO + i*i+j*j));
    }
  }

  // matrix matrix product
  auto C = A*B;

  // check result
  for(camp::idx_t i = 0;i < A_t::s_num_rows; ++ i){

    for(camp::idx_t j = 0;j < B_t::s_num_cols; ++ j){

      // do dot product to compute C(i,j)
      element_t expected(0);
      for(camp::idx_t k = 0;k < A_t::s_num_cols; ++ k){
        expected += A.get(i, k) * B(k,j);
      }

      ASSERT_FLOAT_EQ(C.get(i,j), expected);
    }
  }

}

TYPED_TEST_P(MatrixTest, MatrixMatrixAccumulate)
{

  using A_t = TypeParam;
  using B_t = A_t;
  using element_t = typename A_t::element_type;

  // initialize two matrices
  A_t A;
  for(camp::idx_t j = 0;j < A_t::s_num_cols; ++ j){
    for(camp::idx_t i = 0;i < A_t::s_num_rows; ++ i){
      A.set(i,j, element_t(NO_OPT_ZERO + i+j*j));
    }
  }

  B_t B;
  for(camp::idx_t j = 0;j < B_t::s_num_cols; ++ j){
    for(camp::idx_t i = 0;i < B_t::s_num_rows; ++ i){
      B.set(i,j, element_t(NO_OPT_ZERO + i*i+j*j));
    }
  }

  using C_t = decltype(A*B);

  C_t C;
  for(camp::idx_t j = 0;j < C_t::s_num_cols; ++ j){
    for(camp::idx_t i = 0;i < C_t::s_num_rows; ++ i){
      C.set(i,j, element_t(NO_OPT_ZERO + 2*i+3*j));
    }
  }

  // matrix matrix product
  auto Z1 = A*B+C;

  // explicit
  auto Z2 = A.multiply_accumulate(B, C);

  // check result
  for(camp::idx_t i = 0;i < A_t::s_num_rows; ++ i){

    for(camp::idx_t j = 0;j < B_t::s_num_cols; ++ j){

      // do dot product to compute C(i,j)
      element_t expected = C(i,j);
      for(camp::idx_t k = 0;k < A_t::s_num_cols; ++ k){
        expected += A.get(i, k) * B(k,j);
      }

      ASSERT_FLOAT_EQ(Z1.get(i,j), expected);
      ASSERT_FLOAT_EQ(Z2.get(i,j), expected);
    }
  }

}



REGISTER_TYPED_TEST_SUITE_P(MatrixTest, MatrixCtor,
                                        MatrixGetSet,
                                        MatrixVector,
                                        MatrixMatrix,
                                        MatrixMatrixAccumulate);

INSTANTIATE_TYPED_TEST_SUITE_P(SIMD, MatrixTest, MatrixTestTypes);



