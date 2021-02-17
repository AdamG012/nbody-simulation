#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <pthread.h>
#include <errno.h>


/**
 * Determine the distance between two bodies
 * @param x1, the x position of the first body
 * @param x2, the x position of the second body
 * @param y1, the y position of the first body
 * @param y2, the y position of the second body
 * @param z1, the z position of the first body
 * @param z2, the z position of the second body
 * @return the distance between the two bodies or -1.0 if invalid
 */
double distance(double x1, double x2, double y1, double y2, double z1, double z2);


/**
 * Calculate the magnitude between different bodies in the simulation
 * @param b1, the struct of the first body
 * @param b2, the struct of the second body
 * @return double of the magnitude between the different bodies or -1.0 if invalid
 */
double magnitude(struct body* b1, struct body* b2, double dist);


/**
 * Calculate the velocity of the body from all other bodies and update the positions in the process
 * Function responsible for controlling the simulation
 * @param bodies, the struct array of all the bodies
 * @param len, the length of the struct array of bodies
 * @param dt, the change in time
 */
void step(struct body** bodies, size_t len, double dt);


/**
 * Calculate the velocity of the body from all other bodies and update the positions in the process
 * Function responsible for controlling the simulation
 * @param bodies, the struct array of all the bodies
 * @param len, the length of the struct array of bodies
 * @param dt, the change in time
 */
void step_parallel(struct body** bodies, size_t len, size_t start, size_t end, double dt, pthread_barrier_t barrier);


/**
 * Calculate the total energy of the simulation
 * @param bodies, the struct of all bodies
 * @param len, the length of the bodies array
 * @param start, the start index 
 * @param end, the end index
 * @return the total energy of the system, or -1.0 if invalid
 */
double energy(struct body** bodies, size_t len, size_t start, size_t end);


/**
 * The worker function for the threads
 * @param arg, the thread data structure
 */
void* worker(void* arg);


/**
 * Given two energy values compare and print out whether they are equal
 * @param initial_energy, the initial energy before the step
 * @param final_energy, the final energy after the step
 */
void compare_energy(double initial_energy, double final_energy);


/**
 * Generate the array of struct bodies
 * @param n_bodies, the number of bodies 
 * @return bodies struct array containing all randomly generated bodies
 */
struct body** gen_random_bodies(size_t n_bodies);


/**
 * Read the bodies from a file into a bodies struct array 
 * @param file_name, the name of the file
 * @return bodies, the struct array of bodies or NULL if invalid
 */
struct body** read_file(FILE* file, size_t n_bodies);


/**
 * Get the length of a file based on amount of lines
 * Returns -1 if file is null
 */
size_t get_file_len(FILE* f);


/**
 * Clear up all memory associated with bodies
 */
void clean_up(struct body** bodies, size_t n_bodies);
	

/**
 * Convert a given str into a long value
 * @param var, the variable to copy the conversion into it
 * @param str, the str of the long
 * @return 0 if valid conversion or 1 if NAN or overflow or underflow detected
 */
int long_conversion(size_t* var, char* str);


/**
 * Convert a given str into a double value
 * @param var, the variable to copy the conversion into it
 * @param str, the str of the double
 * @return 0 if valid conversion or 1 if NAN or overflow or underflow detected
 */
int double_conversion(double* var, char* str);

#endif
