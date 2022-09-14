#include <cstdio>
#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>
#include <bitset>
#include <ctime>
#include <time.h>
#include <string>
#include <chrono>
#include "cflobdd_node.h"
#include "cflobdd_t.h"
#include "cflobdd_int.h"
#include "assignment.h"
#include "cross_product.h" 
#include "tests_cfl.h"
#include "ntz_T.h"
// #include "symbolic-unsigned.h"
// #include "matrix1234_complex_double.h"
// #include "matrix1234_double.h"
// #include "vector_double.h"
// #include "vector_float_boost.h"
// #include "vector_complex_float_boost.h"
// #include "quantum_algos.h"
// #include "matrix1234_fourier.h"
// #include "Solver/uwr/matrix/HowellMatrix.h"
// #include "Solver/uwr/matrix/ModularSquareMatrix.h"
// #include "memory_check.h"
#include "matmult_map.h"
// #include "matrix1234_node.h"
#include "matrix1234_int.h"
using namespace CFL_OBDD;
using namespace SH_OBDD;
using namespace std::chrono;

void CFLTests::testTopNodes(){
	std::cout << "Test of TopNodes --------------------------------------" << std::endl;
  // Create all of the possible projection CFLOBDDTopNodes
     CFLOBDD F;
     for (unsigned int i = 0; i < (unsigned int)(1 << CFLOBDD::maxLevel); i++) {
       F = MkProjection(i);
	   F.PrintYield(&std::cout);
	   std::cout << std::endl;
     }
}

void CFLTests::testSatisfyingAssignments(){
	std::cout << "Testing finding satisfying assignments ------------" << std::endl;
  CFLOBDD F;
  F = MkTrue();
  /*#ifdef PATH_COUNTING_ENABLED
  std::cout << F.NumSatisfyingAssignments() << std::endl;
  #endif*/
  Assignment *assignmentPtr;
  if (F.FindOneSatisfyingAssignment(assignmentPtr)) {
    //std::cout << *assignmentPtr << std::endl; ETTODO - fix
    bool b = F.Evaluate(*assignmentPtr);
    std::cout << "Value = " << b << std::endl;
    delete assignmentPtr;
  }
  else {
    std::cout << "No satisfying assignment exists" << std::endl;
  }
  F.PrintYield(&std::cout);
  std::cout << std::endl;
}

void CFLTests::testParity()
{
  // Test of parity function ----------------------------------
  std::cout << "Test of parity function --------------------------------------" << std::endl;
  CFLOBDD F = MkParity();

  // if MaxLevel is 3 or 4, test all assignments
  if (CFLOBDD::maxLevel == 3 || CFLOBDD::maxLevel == 4) {

    std::cout << "Testing all assignments" << std::endl;

    unsigned int size = 1 << CFLOBDD::maxLevel;
    Assignment a(size);
    bool b;
    unsigned long int range = 1UL << size;
    for (unsigned long int i = 0UL; i < range; i++) {
      unsigned long int mask = 1UL;
      bool bb = false;
      for (int j = size - 1; j >= 0; j--) {
        a[j] = (i & mask);
        bb ^= a[j];
        mask = mask << 1;
      }
      b = F.Evaluate(a);
      // std::cout << a << ": " << b << std::endl;
      if (b != bb) {
        //std::cerr << a << ": " << b << std::endl; ETTODO -Fix
      }
    }
  }
  else {
    std::cout << "Cannot test all assignments: maxLevel must be 3 or 4" << std::endl;
  }
}

void CFLTests::testMkDetensorConstraintInterleaved()
{
	std::cout << "Test of MkDetensorConstraintInterleaved function --------------------------------------" << std::endl;

	CFLOBDD F;

	// FIXME TWR -- go back to testing only if MaxLevel is 3 or 4

	// if MaxLevel is 3 or 4, test all assignments
	if (CFLOBDD::maxLevel == 2 || CFLOBDD::maxLevel == 3 || CFLOBDD::maxLevel == 4) {
		F = Matrix1234Int::MkDetensorConstraintInterleaved(CFLOBDD::maxLevel);
	}
	else {
		F = Matrix1234Int::MkDetensorConstraintInterleaved(3);
	}

	std::cout << "Testing all assignments" << std::endl;

	unsigned int size = 1 << CFLOBDD::maxLevel;
	Assignment a(size);
	bool b;
	unsigned long int range = 1UL << size;
	for (unsigned long int i = 0UL; i < range; i++) { // for each assignment
		unsigned long int mask = 1UL;
		bool bb = true;
		for (int j = size - 1; j >= 0; j--) {  // for each variable
			a[j] = ((i & mask) != 0);
			if ((j / 4) * 4 == j) { // j is divisible by 4
				if (a[j + 2] != a[j + 1]) {  // mismatch for interleaved ordering
					bb = false;
				}
			}
			mask = mask << 1;
		}
		// bb == true iff a represents an assignment for (A,B,B,C) with the interleaved ordering
		b = F.Evaluate(a);
		// std::cout << a << ": " << b << std::endl;
		if (b != bb) {
			std::cout << "Error: ";
			a.print(std::cout);
			std::cout << ": " << b << " != " << bb << std::endl;
		}
	}
}

void CFLTests::testMkCFLOBDDMatrixEqVoc14()
{
	std::cout << "Test of MkCFLOBDDMatrixEqVoc14 function --------------------------------------" << std::endl;

	CFLOBDD F, G, H;

	// FIXME TWR -- go back to testing only if MaxLevel is 3 or 4

	// if MaxLevel is 3 or 4, test all assignments
	if (CFLOBDD::maxLevel == 2 || CFLOBDD::maxLevel == 3 || CFLOBDD::maxLevel == 4) {
		F = Matrix1234Int::MkCFLOBDDMatrixEqVoc14(CFLOBDD::maxLevel);
		G = Matrix1234Int::MkDetensorConstraintInterleaved(CFLOBDD::maxLevel);
	}
	else {
		F = Matrix1234Int::MkCFLOBDDMatrixEqVoc14(3);
		G = Matrix1234Int::MkDetensorConstraintInterleaved(3);
	}
	H = MkAnd(F,G);

	std::cout << "Testing all assignments" << std::endl;

	unsigned int size = 1 << CFLOBDD::maxLevel;
	Assignment a(size);
	bool b;
	unsigned long int range = 1UL << size;
	for (unsigned long int i = 0UL; i < range; i++) { // for each assignment
		unsigned long int mask = 1UL;
		bool bb = true;
		for (int j = size - 1; j >= 0; j--) {  // for each variable
			a[j] = ((i & mask) != 0);
			if ((j / 4) * 4 == j) { // j is divisible by 4
				if (a[j + 3] != a[j]) {  // mismatch for interleaved ordering
					bb = false;
				}
			}
			mask = mask << 1;
		}
		// bb == true iff a represents an assignment for (A,B,C,A) with the interleaved ordering
		b = F.Evaluate(a);
		// std::cout << a << ": " << b << std::endl;
		if (b != bb) {
			std::cout << "Error: ";
			a.print(std::cout);
			std::cout << ": " << b << " != " << bb << std::endl;
		}
	}

	std::cout << "---F------------------------------------------------------------" << std::endl;
	std::cout << F << std::endl;
	F.PrintYield(&std::cout);
	std::cout << "----------------------------------------------------------------" << std::endl;
	std::cout << "---G------------------------------------------------------------" << std::endl;
	std::cout << G << std::endl;
	G.PrintYield(&std::cout);
	std::cout << "----------------------------------------------------------------" << std::endl;
	std::cout << "---H------------------------------------------------------------" << std::endl;
	std::cout << H << std::endl;
	H.PrintYield(&std::cout);
	std::cout << "----------------------------------------------------------------" << std::endl;
}

void CFLTests::testMkIdRelationInterleaved()
{
// Test of MkIdRelationInterleaved function ----------------------------------
    std::cout << "Test of MkIdRelationInterleaved function --------------------------------------" << std::endl;

    CFLOBDD F;

	// FIXME TWR -- go back to testing only if MaxLevel is 3 or 4
    // if MaxLevel is 3 or 4, test all assignments
    if (CFLOBDD::maxLevel == 2 || CFLOBDD::maxLevel == 3 || CFLOBDD::maxLevel == 4) {
		F = Matrix1234Int::MkIdRelationInterleaved(CFLOBDD::maxLevel);
    }
    else {
		F = Matrix1234Int::MkIdRelationInterleaved(3);
    }

    std::cout << "Testing all assignments" << std::endl;

    unsigned int size = 1 << CFLOBDD::maxLevel;
	Assignment a(size);
    bool b;
    unsigned long int range = 1UL << size;
    for (unsigned long int i = 0UL; i < range; i++) { // for each assignment
      unsigned long int mask = 1UL;
      bool bb = true;
      for (int j = size - 1; j >= 0; j--) {  // for each variable
        a[j] = ((i & mask) != 0);
        if ((j/2)*2 == j) { // j is even
            if (a[j+1] != a[j]) {  // mismatch for interleaved ordering
                bb = false;
            }
        }
        mask = mask << 1;
      }
      // bb == true iff a represents a diagonal element for the interleaved ordering
      b = F.Evaluate(a);
	  // std::cout << a << ": " << b << std::endl;
	  if (b != bb) {
		  std::cout << "Error: ";
		  a.print(std::cout);
		  std::cout << ": " << b << " != " << bb << std::endl;
      }
    }

}


void CFLTests::testAnd(){
  CFLOBDD F, G, H;
  std::cout << "Testing And ------------" << std::endl;

  F = MkProjection(0, 2);
  std::cout << "done" << std::endl;
  std::cout << F << std::endl;
  std::cout << "----------------------------------------------------------------" << std::endl;
  G = MkProjection(1, 2);
  std::cout << G << std::endl;
  std::cout << "----------------------------------------------------------------" << std::endl;
  H = MkAnd(F, G);
  std::cout << "----------------------------------------------------------------" << std::endl;
  std::cout << H << std::endl;
  std::cout << "----------------------------------------------------------------" << std::endl;
}

void CFLTests::testPlus(){
	CFLOBDD F, G, H;
	std::cout << "Testing Plus ------------" << std::endl;

	F = MkProjection(0);
	std::cout << F << std::endl;
	std::cout << "----------------------------------------------------------------" << std::endl;
	G = MkProjection(1);
	std::cout << G << std::endl;
	std::cout << "----------------------------------------------------------------" << std::endl;
	H = F + G;
	std::cout << "----------------------------------------------------------------" << std::endl;
	std::cout << H << std::endl;
	std::cout << "----------------------------------------------------------------" << std::endl;
	CFLOBDD J = H + H;
	std::cout << "----------------------------------------------------------------" << std::endl;
	std::cout << J << std::endl;
	std::cout << "----------------------------------------------------------------" << std::endl;

	/*	
	F.PrintYield(&std::cout);
	std::cout << std::endl;

	G.PrintYield(&std::cout);
	std::cout << std::endl;

	H.PrintYield(&std::cout);
	std::cout << std::endl;

	J.PrintYield(&std::cout);
	std::cout << std::endl;
	*/
}

void CFLTests::testTimes(){
	CFLOBDD F, G, H;
	std::cout << "Testing Times ------------" << std::endl;

	F = MkProjection(0);
	std::cout << F << std::endl;
	std::cout << "----------------------------------------------------------------" << std::endl;
	G = MkProjection(1);
	std::cout << G << std::endl;
	std::cout << "----------------------------------------------------------------" << std::endl;
	H = F + G;
	std::cout << "----------------------------------------------------------------" << std::endl;
	std::cout << H << std::endl;
	std::cout << "----------------------------------------------------------------" << std::endl;
	CFLOBDD J = H * H;
	std::cout << "----------------------------------------------------------------" << std::endl;
	std::cout << J << std::endl;
	std::cout << "----------------------------------------------------------------" << std::endl;
	CFLOBDD K1 = 3 * J;
	std::cout << "----------------------------------------------------------------" << std::endl;
	std::cout << K1 << std::endl;
	std::cout << "----------------------------------------------------------------" << std::endl;
	CFLOBDD K2 = J * 3;
	std::cout << "----------------------------------------------------------------" << std::endl;
	std::cout << K2 << std::endl;
	std::cout << "----------------------------------------------------------------" << std::endl;

	F.PrintYield(&std::cout);
	std::cout << std::endl;

	G.PrintYield(&std::cout);
	std::cout << std::endl;

	H.PrintYield(&std::cout);
	std::cout << std::endl;

	J.PrintYield(&std::cout);
	std::cout << std::endl;

	K1.PrintYield(&std::cout);
	std::cout << std::endl;

	K2.PrintYield(&std::cout);
	std::cout << std::endl;
}

void CFLTests::test1(){
  CFLOBDD F, G, H, I;
  F = MkProjection(3);
  G = MkProjection(7);
  H = MkProjection(5);
  I = MkIfThenElse(F, G, H);

  // if MaxLevel is 3 or 4, test all assignments
  if (CFLOBDD::maxLevel == 3 || CFLOBDD::maxLevel == 4) {

    std::cout << "Testing all assignments" << std::endl;

    unsigned int size = 1 << CFLOBDD::maxLevel;
    Assignment a(size);
    bool b;
    unsigned long int range = 1UL << size;
    for (unsigned long int i = 0UL; i < range; i++) {
      unsigned long int mask = 1UL;
      for (int j = size - 1; j >= 0; j--) {
        a[j] = (i & mask);
        mask = mask << 1;
      }
      b = I.Evaluate(a);
      if (b != ((a[3] && a[7]) || (!a[3] && a[5])) ) {
        //std::cerr << a << ": " << b << std::endl; ETTODO -Fix
      }
    }
  }
  else {
    std::cout << "Cannot test all assignments: maxLevel must be 3 or 4" << std::endl;
  }
}

void CFLTests::test2(){
  CFLOBDD F, G, H, I;
  F = MkProjection(3);
  G = MkProjection(7);
  H = MkProjection(5);
  I = MkNegMajority(F, G, H);

  // if MaxLevel is 3 or 4, test all assignments
  if (CFLOBDD::maxLevel == 3 || CFLOBDD::maxLevel == 4) {

    std::cout << "Testing all assignments" << std::endl;

    unsigned int size = 1 << CFLOBDD::maxLevel;
    Assignment a(size);
    bool b;
    unsigned long int range = 1UL << size;
    for (unsigned long int i = 0UL; i < range; i++) {
      unsigned long int mask = 1UL;
      for (int j = size - 1; j >= 0; j--) {
        a[j] = (i & mask);
        mask = mask << 1;
      }
      b = I.Evaluate(a);
      if (b != ((a[7] && !a[3]) || (a[5] && !a[3]) || (a[7] && a[5])) ) {
        //std::cerr << a << ": " << b << std::endl; ETTODO -Fix
      }
    }
  }
  else {
    std::cout << "Cannot test all assignments: maxLevel must be 3 or 4" << std::endl;
  }
}

void CFLTests::test3(){
  CFLOBDD F, G, H, I, J, K, L, M, N, O, P, Q;

  std::cout << "Test 1: ---------------------------------" << std::endl;
  F = MkProjection(0);
  G = MkProjection(1);
  H = MkProjection(2);
  I = MkAnd(F, G);
  std::cout << "(x0 & x1)" << std::endl;
  I.PrintYield(&std::cout);
  std::cout << std::endl << std::endl;
  std::cout << "Restrict, x1 <- true" << std::endl;
  J = MkRestrict(I, 1, true);
  J.PrintYield(&std::cout);
  std::cout << std::endl << std::endl;
  std::cout << "Restrict, x1 <- false" << std::endl;
  K = MkRestrict(I, 1, false);
  K.PrintYield(&std::cout);
  std::cout << std::endl << std::endl;
  std::cout << "Test whether False == Restrict, x1 <- false [Expected answer: K == MkFalse()]" << std::endl;
  if (K != MkFalse()) {
    std::cout << "K != MkFalse()" << std::endl;
    std::cout << *K.root << std::endl;
  }
  else
    std::cout << "K == MkFalse()" << std::endl;
  std::cout << std::endl;
  std::cout << "Exists x1" << std::endl;
  L = MkExists(I, 1);
  L.PrintYield(&std::cout);
  std::cout << std::endl << std::endl;

  std::cout << "Test 2: ---------------------------------" << std::endl;

  F = MkProjection(1);
  G = MkProjection(4);
  H = MkProjection(7);
  I = MkAnd(F, G);
  std::cout << "(x1 & x4 & x7)" << std::endl;
  I.PrintYield(&std::cout);
  std::cout << std::endl << std::endl;
  std::cout << "Restrict, x4 <- true" << std::endl;
  J = MkRestrict(I, 4, true);
  J.PrintYield(&std::cout);
  std::cout << std::endl << std::endl;
  std::cout << "Restrict, x4 <- false" << std::endl;
  K = MkRestrict(I, 4, false);
  K.PrintYield(&std::cout);
  std::cout << std::endl << std::endl;
  std::cout << "Test whether False == Restrict, x4 <- false [Expected answer: K == MkFalse()]" << std::endl;
  if (K != MkFalse()) {
    std::cout << "K != MkFalse()" << std::endl;
    std::cout << *K.root << std::endl;
  }
  else
    std::cout << "K == MkFalse()" << std::endl;
  std::cout << std::endl;
  std::cout << "Exists x4" << std::endl;
  L = MkExists(I, 4);
  L.PrintYield(&std::cout);
  std::cout << std::endl << std::endl;

  std::cout << "Test 3: ---------------------------------" << std::endl;

  F = MkProjection(1);
  G = MkProjection(4);
  H = MkProjection(7);
  I = MkAnd(F, G);
  J = MkNot(F);
  K = MkProjection(5);
  L = MkAnd(J,K);
  M = MkOr(I,L);
  std::cout << "(x1 & x4 & x7) | (!x1 & x5)" << std::endl;
  M.PrintYield(&std::cout);
  std::cout << std::endl << std::endl;
  std::cout << "Restrict, x4 <- true" << std::endl;
  N = MkRestrict(M, 4, true);
  N.PrintYield(&std::cout);
  std::cout << std::endl << std::endl;
  std::cout << "Restrict, x4 <- false" << std::endl;
  O = MkRestrict(M, 4, false);
  O.PrintYield(&std::cout);
  std::cout << std::endl << std::endl;
  std::cout << "Exists x4" << std::endl;
  P = MkExists(M, 4);
  P.PrintYield(&std::cout);
  std::cout << std::endl << std::endl;
  std::cout << "Forall x4" << std::endl;
  Q = MkForall(M, 4);
  Q.PrintYield(&std::cout);
  std::cout << std::endl << std::endl;
}

void CFLTests::testMatrixMultiplication()
{
	// Obsolete
	for (unsigned int i = 2; i <= CFLOBDDMaxLevel; i++) {
		CFLOBDD F, G, P, Q, R, I;
		std::cout << "Testing Matrix Multiplication ------------" << std::endl;

		std::cout << "First test projection ------------" << std::endl;
		F = Matrix1234Int::MkWalshVoc12(i);
		std::cout << "---F------------------------------------------------------------" << std::endl;
		// std::cout << F << std::endl;
		// F.PrintYield(&std::cout);
		std::cout << "----------------------------------------------------------------" << std::endl;

		G = Matrix1234Int::KroneckerProduct(F, F);
		std::cout << "----G-----------------------------------------------------------" << std::endl;
		// std::cout << G << std::endl;
		// G.PrintYield(&std::cout);
		std::cout << "----------------------------------------------------------------" << std::endl;

		// P = Matrix1234Int::MatrixProjectVoc23(G);
		// std::cout << "---P = MatrixProjectVoc23(G) -----------------------------------" << std::endl;
		// // std::cout << P << std::endl;
		// // P.PrintYield(&std::cout);
		// std::cout << "----------------------------------------------------------------" << std::endl;

		// Q = Matrix1234Int::MatrixMultiply(F, F);
		// std::cout << "---Q = MatrixMultiply(F, F) -----------------------------------" << std::endl;
		// // std::cout << Q << std::endl;
		// // Q.PrintYield(&std::cout);
		// std::cout << "----------------------------------------------------------------" << std::endl;

		I = Matrix1234Int::MkIdRelationInterleaved(i);
		std::cout << "--- I = MkIdRelationInterleaved(i) -----------------------------------" << std::endl;
		// std::cout << I << std::endl;
		// I.PrintYield(&std::cout);
		std::cout << "----------------------------------------------------------------" << std::endl;

		// R = Matrix1234Int::MatrixDetensor(I);
		// std::cout << "--- R = MatrixDetensor(I) -----------------------------------" << std::endl;
		// // std::cout << I << std::endl;
		// // R.PrintYield(&std::cout);
		// std::cout << "----------------------------------------------------------------" << std::endl;

		std::cout << "i = " << i << std::endl;
		unsigned int c = pow(2, i);
		std::cout << "c = " << c << std::endl;
		// if (Q == c * R)
		// 	std::cout << "Q == " << c << "*R" << std::endl;
		// else
		// std::cout << "Q != " << c << "*R" << std::endl;
	}

}

void CFLTests::testAllAssignments(){
  CFLOBDD F, G, H, I;
  F = MkProjection(3);
  G = MkProjection(7);
  H = MkExclusiveOr(F, G);
  I = MkNot(H);

  // if MaxLevel is 3 or 4, test all assignments
  if (CFLOBDD::maxLevel == 3 || CFLOBDD::maxLevel == 4) {

    std::cout << "Testing all assignments" << std::endl;

    unsigned int size = 1 << CFLOBDD::maxLevel;
    Assignment a(size);
    bool b;
    unsigned long int range = 1UL << size;
    for (unsigned long int i = 0UL; i < range; i++) {
      unsigned long int mask = 1UL;
      for (int j = size - 1; j >= 0; j--) {
        a[j] = (i & mask);
        mask = mask << 1;
      }
      b = I.Evaluate(a);
      // std::cout << a << ": " << b << std::endl;
      if (b != !(a[3] != a[7])) {
        //std::cerr << a << ": " << b << std::endl; ETTODO -Fix
      }
    }
  }
  else {
    std::cout << "Cannot test all assignments: maxLevel must be 3 or 4" << std::endl;
  }
}

// Tests based on the Inverse Reed-Muller matrix --------------------------------
void CFLTests::testInverseReedMull()
{
	CFLOBDD F = Matrix1234Int::MkInverseReedMullerInterleaved(2);    // IRM(2)
     F.PrintYield(&std::cout);
     std::cout << std::endl;
     unsigned int nodeCount, edgeCount;
	 unsigned int returnEdgesCount, returnEdgesObjCount;
     F.CountNodesAndEdges(nodeCount, edgeCount, returnEdgesCount, returnEdgesObjCount);
     std::cout << nodeCount << ", " << edgeCount << std::endl;
}

  // Tests based on the Walsh matrix --------------------------------
void CFLTests::testWalsh()
{
     unsigned int nodeCount, edgeCount;
	 unsigned int returnEdgesCount, returnEdgesObjCount;

	 std::cout << "Testing Walsh matrix creation and manipulation ------------" << std::endl;
	 
	 CFLOBDD F = Matrix1234Int::MkWalshInterleaved(2);

  	 F.PrintYield(&std::cout);
	 std::cout << std::endl;

	 std::cout << "F = MkWalshVoc12(3)" << std::endl;
	 F = Matrix1234Int::MkWalshVoc12(3);
	 F.PrintYield(&std::cout);
	 std::cout << std::endl;

	 std::cout << "H = MatrixShiftVocs12To34(F)" << std::endl;
	 CFLOBDD H = Matrix1234Int::MatrixShiftVocs12To34(F);
	 H.PrintYield(&std::cout);
	 std::cout << std::endl;
	 H.CountNodesAndEdges(nodeCount, edgeCount, returnEdgesCount, returnEdgesObjCount);
	 std::cout << nodeCount << ", " << edgeCount << std::endl;

     F.CountNodesAndEdges(nodeCount, edgeCount, returnEdgesCount, returnEdgesObjCount);
     std::cout << nodeCount << ", " << edgeCount << std::endl;

	 CFLOBDD G = Matrix1234Int::MkDetensorConstraintInterleaved(3);
	 G.PrintYield(&std::cout);
	 std::cout << std::endl;
	 G.CountNodesAndEdges(nodeCount, edgeCount, returnEdgesCount, returnEdgesObjCount);
	 std::cout << nodeCount << ", " << edgeCount << std::endl;

	  CFLOBDD J = H * G;
	 J.PrintYield(&std::cout);
	 std::cout << std::endl;

	 std::cout << "K = MkOr(MkProjection(3), MkProjection(7))" << std::endl;
	 CFLOBDD K = MkOr(MkProjection(3), MkProjection(7));
	 std::cout << "---K------------------------------------------------------------" << std::endl;
	 std::cout << K << std::endl;
	 std::cout << "----------------------------------------------------------------" << std::endl;
	 K.PrintYield(&std::cout);
	 std::cout << std::endl;
	 K.CountNodesAndEdges(nodeCount, edgeCount, returnEdgesCount, returnEdgesObjCount);
	 std::cout << nodeCount << ", " << edgeCount << std::endl;

	 /*
	 std::cout << "L = H * K" << std::endl;
	 CFLOBDD L = H * K;
	 L.PrintYield(&std::cout);
	 std::cout << std::endl;
	 L.CountNodesAndEdges(nodeCount, edgeCount);
	 std::cout << nodeCount << ", " << edgeCount << std::endl;
	 */

	 std::cout << "M = MatrixShiftVoc42(K)" << std::endl;
	 CFLOBDD M = Matrix1234Int::MatrixShiftVoc42(K);
	 std::cout << "-----M----------------------------------------------------------" << std::endl;
	 std::cout << M << std::endl;
	 std::cout << "----------------------------------------------------------------" << std::endl;
	 M.PrintYield(&std::cout);
	 std::cout << std::endl;
	 M.CountNodesAndEdges(nodeCount, edgeCount, returnEdgesCount, returnEdgesObjCount);
	 std::cout << nodeCount << ", " << edgeCount << std::endl;
}


void CFLTests::testCanonicalness(){
  // DeMorgan's law as a test of canonicalness ------------------
     std::cout << "Test of DeMorgan's law" << std::endl;
     CFLOBDD F, G, H, I, J, K;
     F = MkProjection(3);
     G = MkProjection(7);
     H = MkNand(F, G);
     I = MkNot(F);
     J = MkNot(G);
     K = MkOr(I, J);
   
     std::cout << (H == K) << std::endl;
   
     unsigned int nodeCount, edgeCount;
	 unsigned int returnEdgesCount, returnEdgesObjCount;
     F.CountNodesAndEdges(nodeCount, edgeCount, returnEdgesCount, returnEdgesObjCount);
     std::cout << nodeCount << ", " << edgeCount << std::endl;
     G.CountNodesAndEdges(nodeCount, edgeCount, returnEdgesCount, returnEdgesObjCount);
     std::cout << nodeCount << ", " << edgeCount << std::endl;
     H.CountNodesAndEdges(nodeCount, edgeCount, returnEdgesCount, returnEdgesObjCount);
     std::cout << nodeCount << ", " << edgeCount << std::endl;
     I.CountNodesAndEdges(nodeCount, edgeCount, returnEdgesCount, returnEdgesObjCount);
     std::cout << nodeCount << ", " << edgeCount << std::endl;
     J.CountNodesAndEdges(nodeCount, edgeCount, returnEdgesCount, returnEdgesObjCount);
     std::cout << nodeCount << ", " << edgeCount << std::endl;
     K.CountNodesAndEdges(nodeCount, edgeCount, returnEdgesCount, returnEdgesObjCount);
     std::cout << nodeCount << ", " << edgeCount << std::endl;
}

void CFLTests::testMkAdditionInterleaved()
{

	// Test of the addition relation --------------------------------
	std::cout << "Test of MkAdditionInterleaved function --------------------------------------" << std::endl;
	auto start = high_resolution_clock::now();
	CFLOBDDNodeHandle::InitAdditionInterleavedTable();
	CFLOBDD AdditionRel = MkAdditionInterleaved();
	auto end = high_resolution_clock::now();
	std::cout << "AdditionRel created" << std::endl;
	auto duration = duration_cast<milliseconds>(end - start);

	/*
	// if MaxLevel is between 2 and 5 (inclusive), test AdditionRel against relation created via recursion and by brute force
	if (2 <= CFLOBDD::maxLevel && CFLOBDD::maxLevel <= 5) {
		CFLOBDD AdditionRelRecursive = MkAdditionInterleavedRecursive();
		std::cout << "AdditionRelRecursive created" << std::endl;
		CFLOBDD AdditionRelBruteForce = MkAdditionInterleavedBruteForce();
		std::cout << "AdditionRelBruteForce created" << std::endl;
		if (AdditionRelRecursive == AdditionRelBruteForce)
			std::cout << "AdditionRelRecursive == AdditionRelBruteForce" << std::endl;
		else
			std::cout << "AdditionRelRecursive != AdditionRelBruteForce" << std::endl;

		if (AdditionRelRecursive == AdditionRel)
			std::cout << "AdditionRelRecursive == AdditionRel" << std::endl;
		else
			std::cout << "AdditionRelRecursive != AdditionRel" << std::endl;

		AdditionRelRecursive.PrintYield(&std::cout);
		std::cout << std::endl << std::endl;
		AdditionRelBruteForce.PrintYield(&std::cout);
		std::cout << std::endl << std::endl;
		AdditionRel.PrintYield(&std::cout);
	}
*/
	//     AdditionRel.PrintYield(std::cout);
	std::cout << std::endl;
	unsigned int nodeCount, edgeCount;
	unsigned int returnEdgesCount, returnEdgesObjCount;
	AdditionRel.CountNodesAndEdges(nodeCount, edgeCount, returnEdgesCount, returnEdgesObjCount);
	std::cout << duration.count() << " " <<  nodeCount << ", " << edgeCount << ", " << returnEdgesCount << 
		", " << returnEdgesObjCount << " " << (nodeCount + edgeCount) << std::endl;
/*#ifdef PATH_COUNTING_ENABLED
	if (2 <= CFLOBDD::maxLevel && CFLOBDD::maxLevel <= 5) {
		std::cout << "CFLOBDD::maxLevel = " << CFLOBDD::maxLevel << "; NumSatisfyingAssignments = " << AdditionRel.NumSatisfyingAssignments() << std::endl;
	}
	else {
		std::cout << "Cannot count the number of satisfying assignments: CFLOBDD::maxLevel = " << CFLOBDD::maxLevel << "; you must have 2 <= maxLevel <= 5" << std::endl;
	}
#endif*/
	if (2 <= CFLOBDD::maxLevel && CFLOBDD::maxLevel <= 31) {
		// temp := AdditionRel with the low-order bits of x and y restricted to 1
		CFLOBDD xRestriction = MkXBit(0);  // low-order bit == 1
		CFLOBDD yRestriction = MkYBit(0);  // low-order bit == 1
		CFLOBDD temp = MkAnd(AdditionRel, MkAnd(xRestriction, yRestriction));
		// Test whether temp => zRestriction
		CFLOBDD zRestriction = MkNot(MkZBit(0));  // low-order bit == 0
		CFLOBDD t2 = MkImplies(temp, zRestriction);
		CFLOBDD tt = MkTrue();
		if (t2 == tt) {
			std::cout << "True" << std::endl;
		}
		else {
			std::cout << "False" << std::endl;
		}
	}

	return;
}


bool CFLTests::runTests(const char *arg, int size, int seed){
	CFLOBDDNodeHandle::InitNoDistinctionTable();
	CFLOBDDNodeHandle::InitAdditionInterleavedTable();
	CFLOBDDNodeHandle::InitReduceCache();
	InitPairProductCache();
	InitTripleProductCache();
	Matrix1234Int::Matrix1234Initializer();
	
	std::string curTest = arg;  
	if (curTest == "TopNode") {
		CFLTests::testTopNodes();
	} else if (curTest == "SatisfyingAssignment") {
		CFLTests::testSatisfyingAssignments();
	} else if (curTest == "DetensorConstraintInterleaved") {
		testMkDetensorConstraintInterleaved();
	} else if (curTest == "CFLOBDDMatrixEqVoc14") { 
		testMkCFLOBDDMatrixEqVoc14();
	// } else if (curTest == "testMkFourierMatrix") {
	// 	testMkFourierMatrix(size);
	} else if (curTest == "InterleavedId") {
		CFLTests::testMkIdRelationInterleaved();
	} else if (curTest == "Parity") {
		CFLTests::testParity(); 
	} else if (curTest == "And") {
	    CFLTests::testAnd();
	} else if (curTest == "Plus") {
		CFLTests::testPlus();
	} else if (curTest == "Times") {
	    CFLTests::testTimes();
	} else if (curTest == "MatrixMultiplication") {
		CFLTests::testMatrixMultiplication();
	// } else if (curTest == "RestrictExists") {
	// 	CFLTests::test_restrict_exists_and_forall();
	// } else if (curTest == "Karatsuba") {			
	// 	CFLTests::testKaratsuba();
	} else if (curTest == "Test1") {
		CFLTests::test1();
	} else if (curTest == "Test2") {
		CFLTests::test2();
	} else if (curTest == "Test3") {
		CFLTests::test3();
	} else if (curTest == "AllAssign") {
		CFLTests::testAllAssignments();
	} else if (curTest == "Canonicalness") {
		CFLTests::testCanonicalness();
	// } else if (curTest == "DeMorgans") {
	// 	CFLTests::test_demorgans();
	} else if (curTest == "InvReedMull") {
		CFLTests::testInverseReedMull();
	} else if (curTest == "Walsh") {
		CFLTests::testWalsh();
	} else if (curTest == "AdditionInterleaved") {
		CFLTests::testMkAdditionInterleaved();
	// } else if (curTest == "Permute") {
	// 	CFLTests::testPermute();
	// } else if (curTest == "BasisVector") {
	// 	CFLTests::testBasisVector();
	// } else if (curTest == "testBasicOperations") {
	// 	CFLTests::testBasicOperations(size);
	// } else if (curTest == "testShorsAlgo") {
	// 	CFLTests::testShorsAlgo();
	// } else if (curTest == "testShortestPath") {
	// 	CFLTests::testShortestPath();
	// } else if (curTest == "testGHZAlgo") {
	// 	CFLTests::testGHZAlgo(size);
	// } else if (curTest == "testBVAlgo") {
	// 	CFLTests::testBVAlgo(size, seed);
	// } else if (curTest == "testDJAlgo") {
	// 	CFLTests::testDJAlgo(size, seed);
	// } else if (curTest == "testGroversAlgo") {
	// 	CFLTests::testGroversAlgo(size, seed);
	// } else if (curTest == "testSimonsAlgo") {
	// 	CFLTests::testSimonsAlgo(size, seed);
	// } else if (curTest == "testSimonsAlgoNew") {
	// 	CFLTests::testSimonsAlgoNew(size);
	// }
	// else {
	// 	std::cout << "Unrecognized test name: " << curTest << std::endl;
	}

	DisposeOfTripleProductCache();
	DisposeOfPairProductCache();
	CFLOBDDNodeHandle::DisposeOfReduceCache();

	return false;
}