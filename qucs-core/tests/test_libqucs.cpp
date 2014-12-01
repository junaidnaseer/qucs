/*
 * test_libqucs.cpp - unit tests for Qucs core library
 *
 * Copyright (C) 2014 Guilherme Brondani Torri <guitorri@gmail.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */


/*

Not always needed to use qucs::
*/

#include <iostream>

#include "qucs_typedefs.h"
#include "real.h"
#include "complex.h"

#include "matrix.h"
#include "object.h"
#include "vector.h"

#include "fourier.h"

#include "module.h"

#include "components.h"

#include "gtest/gtest.h"  // Google Test

// tolerance on numeric comparison
double tol = 0.0001;

TEST(real, sqrt) {
  ASSERT_EQ (18.0, qucs::sqrt (324.0));
  ASSERT_EQ (0.0, qucs::sqrt (0.0));
  //ASSERT_EQ (-1, qucs::sqrt(-22.0)); //error NaN (depending on implementation)
}

TEST(real, jn_BesselFistKind) {
  // FIXME not in qucs::
  EXPECT_NEAR ( 0.44, jn (1, 1), 0.0001);
}

TEST(real, Factorial) {
  ASSERT_EQ ( 479001600, qucs::factorial (12) );
}

TEST (complex, cos) {
  nr_complex_t z = nr_complex_t (1.0, 1.0);
  EXPECT_NEAR ( 0.83370025, qucs::cos (z).real(), tol);
  EXPECT_NEAR (-0.98889770, qucs::cos (z).imag(), tol);
}

TEST (complex, jn_BesselFistKind) {
  nr_complex_t z = nr_complex_t (1.0, 1.0);
  EXPECT_NEAR ( 0.61416033492290361, qucs::jn (1, z).real(), tol);
  EXPECT_NEAR ( 0.36502802882708778, qucs::jn (1, z).imag(), tol);
}


TEST (matrix, getCols) {
    qucs::matrix data =  qucs::eye(3,3);
    EXPECT_EQ ( 3 , data.getCols() );
}

TEST (vector, sum) {
  qucs::vector vec = qucs::vector(3);
  EXPECT_EQ ( 3 , vec.getSize() );
  for (int k = 0; k < vec.getSize(); k++)
    vec.set(1, k);
  EXPECT_EQ ( 3.0 , qucs::sum(vec) );
}

TEST (fourier, fft) {
  // fft of a DC vector
  // in   [1, 1, 1, 1, 1, 1, 1, 1]
  // out  [8, 0, 0, 0, 0, 0, 0, 0]
  qucs::vector vec = qucs::vector(8);

  for (int k = 0; k < vec.getSize(); k++)
    vec.set(1, k);

  qucs::vector vdif  = qucs::fourier::fft_1d ( vec ) ;

  for (int k = 0; k < vec.getSize(); k++)
    if (k==0)
      EXPECT_EQ ( 8 , vdif.get(k).real() );
    else
      EXPECT_EQ ( 0 , vdif.get(k).real() );
}

TEST (module, constructor) {
  //std::cout << "can we print info"  << std::endl;
  module *m = new module ();
  EXPECT_EQ(NULL, m->definition);
  EXPECT_EQ(NULL, m->circreate);
  EXPECT_EQ(NULL, m->anacreate);
}

TEST (component, resistor_getType) {
  resistor *res = new resistor();
  //res->initDC();
  std::cout << CIR_RESISTOR << " -- "<< res->getType() << std::endl;
  EXPECT_EQ( CIR_RESISTOR, res->getType());
}


// --------------------

#include "tridiag.h"
#include "tvector.h"

TEST (tridiag, solve_s_cyc) {
/* # test program in Python
  import numpy as np
  A = np.array(
        [[-2,  1,  0,  0,  2],
         [ 1, -2,  1,  0,  0],
         [ 0,  1, -2,  1,  0],
         [ 0,  0,  1, -2,  1],
         [ 2,  0,  0,  1, -2]])
  b = np.array([1, 2, 3, 4, 5])
  x = np.linalg.solve(A,b)
  print x
  [ 8.21428571  3.85714286  1.5  2.14285714  6.78571429]
*/

  int n = 5;
  tvector<nr_double_t> x (n);
  x(0) = 8.21428571;
  x(1) = 3.85714286;
  x(2) = 1.5;
  x(3) = 2.14285714;
  x(4) = 6.78571429;

  tridiag<nr_double_t> sys;
  tvector<nr_double_t> o (n);
  tvector<nr_double_t> d (n);
  tvector<nr_double_t> b (n);

  for (int i = 0; i < n; i++) {
    d(i) = -2.;
    o(i) =  1.;
    b(i) = i+1.;
  }
  o(n-1) = 2.;

  /*
  for (int i = 0; i < n; i++) {
    std::cout << d(i);
  }
  std::cout << '\n';
  for (int i = 0; i < n; i++) {
    std::cout << o(i);
  }
  std::cout << '\n';
  for (int i = 0; i < n; i++) {
    std::cout << b(i);
  }
  std::cout << '\n';
  */
  sys.setDiagonal (&d);
  sys.setOffDiagonal (&o);
  sys.setRHS (&b);
  sys.setType (TRIDIAG_SYM_CYCLIC);
  sys.solve ();
  // in-place solver, result in b

  //for (int i = 0; i < n; i++) {
  //  std::cout << b(i) << ", ";
  //}
  //
  for (int i = 0; i < n; i++) {
    EXPECT_NEAR (x(i), b(i),tol);
  }
}
