#ifndef NBODY_H
#define NBODY_H
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <stdatomic.h>
#include <pthread.h>


struct body {
	double x;
	double y;
	double z;
	double velocity_x;
	double velocity_y;
	double velocity_z;
	double mass;
};

struct thread_data {
	struct body** bodies;
	size_t n_bodies;
	size_t iterations;
	size_t start;
	size_t end;
	double initial_energy;
	double final_energy;
	double dt;
	pthread_barrier_t* barrier;
};

#define PI (3.141592653589793)
#define SOLARMASS (4 * PI * PI)
#define NDAYS (365.25)
#define GCONST (6.67e-11) 
#define DEFAULT_RAND_BOUND (1000000)

/*
void step(struct body** bodies, size_t n_bodies, double dt);
void step_parallel(struct body** bodies, size_t len, size_t start, size_t end, double dt);
double energy(struct body** bodies, size_t len);
struct body** gen_random_bodies(size_t n_bodies);
struct body** read_file(FILE* file, size_t n_bodies);
size_t get_file_len(FILE* f);
void clean_up(struct body** bodies, size_t n_bodies);
int long_conversion(size_t* var, char* str);
double distance(struct body* b1, struct body* b2);
double magnitude(struct body* b1, struct body* b2, double distance);
*/

#endif
