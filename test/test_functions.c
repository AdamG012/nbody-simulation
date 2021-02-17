#include "../src/nbody.h"
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <math.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/Automated.h>
#include <assert.h>
#include "../src/functions.c"


/******** DISTANCE METHOD TEST *****************/
void test_valid_distance(void) {
	double ret_val = distance(1.0, 2.0, 1.0, 2.0, 1.0, 2.0);
	CU_ASSERT_DOUBLE_EQUAL(ret_val, sqrt(3), 0.01);
}

void test_zero_distance(void) {
	double ret_val = distance(0, 0, 0, 0, 0, 0);
	CU_ASSERT_DOUBLE_EQUAL(ret_val, 0.0, 0.01);
}

/********************************************/


/******** MAGNITUDE METHOD TEST ***********/
void test_nullbody1_magnitude(void) {
	struct body b = { 1.0, 1.0, 1,0, 1.0, 1.0, 1.0};
	double mag = magnitude(NULL, &b, 1.0);
	CU_ASSERT_DOUBLE_EQUAL(mag, -1.0, 0.01);
}

void test_nullbody2_magnitude(void) {
	struct body b = { 1.0, 1.0, 1,0, 1.0, 1.0, 1.0};
	double mag = magnitude(&b, NULL, 1.0);
	CU_ASSERT_DOUBLE_EQUAL(mag, -1.0, 0.01);
}

void test_negdistance_magnitude(void) {
	struct body b1 = { 1.0, 1.0, 1,0, 1.0, 1.0, 1.0};
	struct body b2 = { 2.0, 2.0, 2,0, 1.0, 1.0, 1.0};
	double mag = magnitude(&b1, &b2, -10);
	CU_ASSERT_DOUBLE_EQUAL(mag, -1.0, 0.01);
}

void test_zerodistance_magnitude(void) {
	struct body b1 = { 1.0, 1.0, 1,0, 1.0, 1.0, 1.0};
	struct body b2 = { 2.0, 2.0, 2,0, 1.0, 1.0, 1.0};
	double mag = magnitude(&b1, &b2, 0);
	CU_ASSERT_DOUBLE_EQUAL(mag, 0.02 * GCONST * b1.mass * b2.mass / sqrt(3), 0.01);
}

void test_valid_magnitude(void) {
	struct body b1 = { 1.0, 1.0, 1,0, 1.0, 1.0, 1.0};
	struct body b2 = { 3.0, 1.0, 3,0, 1.0, 1.0, 2.0};
	double mag = magnitude(&b1, &b2, 10);
	double expected = GCONST * (1.0 * 2.0)/100;
	CU_ASSERT_DOUBLE_EQUAL(mag, expected, 0.01);
}

/******** ENERGY METHOD TEST ***********/
void test_nullstructs_energy(void) {
	double e = energy(NULL, 1, 1, 2);
	CU_ASSERT_DOUBLE_EQUAL(e, -1.0, 0.01);
}


void test_startinvalid_energy(void) {
	struct body b1 = { 1.0, 1.0, 1,0, 1.0, 1.0, 1.0};
	struct body b2 = { 2.0, 2.0, 2,0, 1.0, 1.0, 1.0};
	struct body** bodies = malloc(sizeof(struct body*) * 2);
	bodies[0] = &b1;
	bodies[1] = &b2;
	double mag = energy(bodies, 2, 100, 1);
	CU_ASSERT_DOUBLE_EQUAL(mag, -1.0, 0.01);
	free(bodies);
}


void test_valid_energy(void) {
	struct body b1 = { 1.0, 1.0, 1,0, 1.0, 1.0, 1.0};
	struct body b2 = { .x = 2.0, .y = 2.0, .z = 2,0, 
		.velocity_y = 1.0, .velocity_z = 1.0, .mass = 1.0};
	struct body** bodies = malloc(sizeof(struct body*) * 2);
	bodies[0] = &b1;
	bodies[1] = &b2;
	double mag = energy(bodies, 2, 0, 2);
	CU_ASSERT_DOUBLE_EQUAL(mag, 2.0, 0.01);
	free(bodies);
}

/* *********************************** */


/******** STEP METHOD TEST ***********/
void test_validpositionsb1x_step(void) {
	struct body b1 = { 1.0, 1.0, 1, 0, 1.0, 1.0, 1.0};
	struct body b2 = { .x = 2.0, .y = 2.0, .z = 2,0, 
		.velocity_y = 1.0, .velocity_z = 1.0, .mass = 1.0};

	struct body** bodies = malloc(sizeof(struct body*) * 2);
	bodies[0] = &b1;
	bodies[1] = &b2;
	step(bodies, 2, 1);
	CU_ASSERT_DOUBLE_EQUAL(b1.x, 1.0, 0.1);
	free(bodies);
}


void test_validpositionsb1y_step(void) {
	struct body b1 = { 1.0, 1.0, 1, 0, 1.0, 1.0, 1.0};
	struct body b2 = { .x = 2.0, .y = 2.0, .z = 2,0, 
		.velocity_y = 1.0, .velocity_z = 1.0, .mass = 1.0};

	struct body** bodies = malloc(sizeof(struct body*) * 2);
	bodies[0] = &b1;
	bodies[1] = &b2;
	step(bodies, 2, 1);
	CU_ASSERT_DOUBLE_EQUAL(b1.y, 2.0, 0.1);
	free(bodies);
}


void test_validpositionsb1z_step(void) {
	struct body b1 = { 1.0, 1.0, 1, 0, 1.0, 1.0, 1.0};
	struct body b2 = { .x = 2.0, .y = 2.0, .z = 2,0, 
		.velocity_y = 1.0, .velocity_z = 1.0, .mass = 1.0};

	struct body** bodies = malloc(sizeof(struct body*) * 2);
	bodies[0] = &b1;
	bodies[1] = &b2;
	step(bodies, 2, 1);
	CU_ASSERT_DOUBLE_EQUAL(b1.z, 2.0, 0.1);
	free(bodies);
}

void test_validpositionsb2x_step(void) {
	struct body b1 = { 1.0, 1.0, 1, 0, 1.0, 1.0, 1.0};
	struct body b2 = { .x = 2.0, .y = 2.0, .z = 2,0, 
		.velocity_y = 1.0, .velocity_z = 1.0, .mass = 1.0};

	struct body** bodies = malloc(sizeof(struct body*) * 2);
	bodies[0] = &b1;
	bodies[1] = &b2;
	step(bodies, 2, 1);
	CU_ASSERT_DOUBLE_EQUAL(b2.x, 2.0, 0.1);
	free(bodies);
}


void test_validpositionsb2y_step(void) {
	struct body b1 = { 1.0, 1.0, 1, 0, 1.0, 1.0, 1.0};
	struct body b2 = { .x = 2.0, .y = 2.0, .z = 2,0, 
		.velocity_y = 1.0, .velocity_z = 1.0, .mass = 1.0};

	struct body** bodies = malloc(sizeof(struct body*) * 2);
	bodies[0] = &b1;
	bodies[1] = &b2;
	step(bodies, 2, 1);
	CU_ASSERT_DOUBLE_EQUAL(b2.y, 3.0, 0.1);
	free(bodies);
}


void test_validpositionsb2z_step(void) {
	struct body b1 = { 1.0, 1.0, 1, 0, 1.0, 1.0, 1.0};
	struct body b2 = { .x = 2.0, .y = 2.0, .z = 2,0, 
		.velocity_y = 1.0, .velocity_z = 1.0, .mass = 1.0};

	struct body** bodies = malloc(sizeof(struct body*) * 2);
	bodies[0] = &b1;
	bodies[1] = &b2;
	step(bodies, 2, 1);
	CU_ASSERT_DOUBLE_EQUAL(b2.z, 3.0, 0.1);
	free(bodies);
}


void test_validpositionslarge_step(void) {
	struct body b1 = { 1.0, 1.0, 1, 0, 1.0, 1.0, 1.0};
	struct body b2 = { .x = 2.0, .y = 2.0, .z = 2,0, 
		.velocity_y = 1.0, .velocity_z = 1.0, .mass = 1.0};

	struct body** bodies = malloc(sizeof(struct body*) * 2);
	bodies[0] = &b1;
	bodies[1] = &b2;
	for (int i = 0; i < 1000; i++) {
		step(bodies, 2, 1);
	}
	CU_ASSERT_DOUBLE_EQUAL(b1.x, 1.00006, 0.1);
	CU_ASSERT_DOUBLE_EQUAL(b1.y, 1001.0, 0.1);
	CU_ASSERT_DOUBLE_EQUAL(b1.z, 1001.0, 0.1);
	CU_ASSERT_DOUBLE_EQUAL(b2.x, 1.994, 0.1);
	CU_ASSERT_DOUBLE_EQUAL(b2.y, 1001.9994, 0.1);
	CU_ASSERT_DOUBLE_EQUAL(b2.z, 1001.9994, 0.1);
	free(bodies);
}


void test_validenergy_step(void) {
	struct body b1 = { 1.0, 1.0, 1, 0, 1.0, 1.0, 1.0};
	struct body b2 = { .x = 2.0, .y = 2.0, .z = 2,0, 
		.velocity_y = 1.0, .velocity_z = 1.0, .mass = 1.0};

	struct body** bodies = malloc(sizeof(struct body*) * 2);
	bodies[0] = &b1;
	bodies[1] = &b2;
	double previous_energy = energy(bodies, 2, 0, 2);
	step(bodies, 2, 1);
	double final_energy = energy(bodies, 2, 0, 2);
	CU_ASSERT_DOUBLE_EQUAL(previous_energy, final_energy, 0.01);
	free(bodies);
}


void test_validenergylarge_step(void) {
	struct body b1 = { 1.0, 1.0, 1, 0, 1.0, 1.0, 1.0};
	struct body b2 = { .x = 2.0, .y = 2.0, .z = 2,0, 
		.velocity_y = 1.0, .velocity_z = 1.0, .mass = 1.0};

	struct body** bodies = malloc(sizeof(struct body*) * 2);
	bodies[0] = &b1;
	bodies[1] = &b2;
	double previous_energy = energy(bodies, 2, 0, 2);
	for (size_t i = 0; i < 1000; i++) {
		step(bodies, 2, 1);
	}
	double final_energy = energy(bodies, 2, 0, 2);
	CU_ASSERT_DOUBLE_EQUAL(previous_energy, final_energy, 0.01);
	free(bodies);
}


void test_validenergylargerandom_step(void) {
	struct body** bodies = gen_random_bodies(1000);
	double previous_energy = energy(bodies, 2, 0, 2);
	for (size_t i = 0; i < 1000; i++) {
		step(bodies, 2, 1);
	}
	double final_energy = energy(bodies, 2, 0, 2);
	CU_ASSERT_DOUBLE_EQUAL(previous_energy, final_energy, 0.01);
	clean_up(bodies, 1000);
}
/* *********************************** */

void* testcases[] = {
	&test_valid_distance,
	&test_zero_distance,
	&test_nullbody1_magnitude,
	&test_nullbody2_magnitude,
	&test_negdistance_magnitude,
	&test_zerodistance_magnitude,
	&test_valid_magnitude,
	&test_nullstructs_energy,
	&test_startinvalid_energy,
	&test_valid_energy,
	&test_validpositionsb1x_step,
	&test_validpositionsb1y_step,
	&test_validpositionsb1z_step,
	&test_validpositionsb2x_step,
	&test_validpositionsb2y_step,
	&test_validpositionsb2z_step,
	&test_validpositionslarge_step,
	&test_validenergy_step,
	&test_validenergylarge_step,
	&test_validenergylargerandom_step,
};

char* testcase_description[] = {
	"test_valid_distance",
	"test_zero_distance",
	"test_nullbody1_magnitude",
	"test_nullbody2_magnitude",
	"test_negdistance_magnitude",
	"test_zerodistance_magnitude",
	"test_valid_magnitude",
	"test_nullstructs_energy",
	"test_startinvalid_energy",
	"test_valid_energy",
	"test_validpositionsb1x_step",
	"test_validpositionsb1y_step",
	"test_validpositionsb1z_step",
	"test_validpositionsb2x_step",
	"test_validpositionsb2y_step",
	"test_validpositionsb2z_step",
	"test_validpositionslarge_step",
	"test_validenergy_step",
	"test_validenergylarge_step",
	"test_validenergylargerandom_step",
};

int init_suite(void) {
	return 0;
}

int clean_suite(void) {
	return 0;
}

int main() {

	CU_pSuite p_suite = NULL;

	// Initalise the test registry
	if ( CUE_SUCCESS != CU_initialize_registry() )
		return CU_get_error();

	// Add a suite to the CUnit
	p_suite = CU_add_suite( "test_suite", init_suite, clean_suite );
	if ( NULL == p_suite ) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	// Add all the tests to the suite
	for (int i = 0; i < 20; i++) {
		if (CU_add_test(p_suite, testcase_description[i], testcases[i]) == NULL) {
			CU_cleanup_registry();
			return CU_get_error();
		}
	}

	// Run all the tests setting verbose mode and showing all failures.
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_basic_show_failures(CU_get_failure_list());
	CU_cleanup_registry();
	return CU_get_error();

}
