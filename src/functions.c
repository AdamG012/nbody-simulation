#include "nbody.h"
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
double distance(double x1, double x2, double y1, double y2, double z1, double z2) {

	// TODO use more efficient square root algorithm
	return sqrt( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) + (z1 - z2) * (z1 - z2));
}


/**
 * Calculate the magnitude between different bodies in the simulation
 * @param b1, the struct of the first body
 * @param b2, the struct of the second body
 * @return double of the magnitude between the different bodies or -1.0 if invalid
 */
double magnitude(struct body* b1, struct body* b2, double dist) {
	// Check if either of the structs are null or distance is invalid
	if (b1 == NULL || b2 == NULL || dist < 0) {
		return -1.0;
	}

	if (dist == 0) {
		dist = 0.02;
	}

	return GCONST * ((b1->mass * b2->mass)/(dist * dist)); 
}


/**
* Update the position of the body
* @param b, the body to be updated
* @param dt, the amount to step for each iteration
*/
void update_body_position(struct body* b, double dt) {
	if (b == NULL) {
		return;
	}

	b->x += b->velocity_x * dt;
	b->y += b->velocity_y * dt;
	b->z += b->velocity_z * dt;
}


/**
 * Calculate the velocity of the body from all other bodies and update the positions in the process
 * Function responsible for controlling the simulation
 * @param bodies, the struct array of all the bodies
 * @param len, the length of the struct array of bodies
 * @param dt, the change in time
 */
void step(struct body** bodies, size_t len, double dt) {

	// Check if the parameters are invalid
	if (bodies == NULL) {
		return;
	}

	// Loop through all the bodies and calculate each step
	for (size_t i = 0; i < len; i++) {
		// Skip if invalid
		if (bodies[i] == NULL) {
			continue;
		}

		// The total sum of the velocity so far
		register double x = bodies[i]->x, y = bodies[i]->y, z = bodies[i]->z;
		register double mass = bodies[i]->mass;
		register double velocity_x = 0, velocity_y = 0, velocity_z = 0;
		
		// Loop through every other j
		for (size_t j = i + 1; j < len; j++) {
			// Skip if the same or invalid body
			if (bodies[j] == NULL) {
				printf("iuoiuo");
				continue;
			}
			double dist = distance(bodies[j]->x, x, bodies[j]->y, y, bodies[j]->z, z);
			// If they are too close
			if (dist == 0.0) {
				dist = 0.02;
			}
			double mag = magnitude(bodies[j], bodies[i], dist);
			// Calculate the updated distances
			double x_dist = (bodies[j]->x - x) / dist;
			double y_dist = (bodies[j]->y - y) / dist;
			double z_dist = (bodies[j]->z - z) / dist;
			// Calculate the new velocity
			velocity_x += (x_dist * mag / mass) * dt; 
			velocity_y += (y_dist * mag / mass) * dt;
			velocity_z += (z_dist * mag / mass) * dt;
		
			// Calculate the velocity for the j body by using previous values
			bodies[j]->velocity_x += (((-1 * x_dist) * mag) / bodies[j]->mass) * dt;
			bodies[j]->velocity_y += (((-1 * y_dist) * mag) / bodies[j]->mass) * dt;
			bodies[j]->velocity_z += (((-1 * z_dist) * mag) / bodies[j]->mass) * dt;
		}

		// Set the old velocities to the updated ones
		bodies[i]->velocity_x += velocity_x;
		bodies[i]->velocity_y += velocity_y;
		bodies[i]->velocity_z += velocity_z;

		// Update the position of the body
		update_body_position(bodies[i], dt);
	}
}


/**
 * Calculate the velocity of the body from all other bodies and update the positions in the process
 * Function responsible for controlling the simulation
 * @param bodies, the struct array of all the bodies
 * @param len, the length of the struct array of bodies
 * @param dt, the change in time
 * @param barrier, the barrier of the threads maxed at number of threads
 */
void step_parallel(struct body** bodies, size_t len, size_t start, size_t end, double dt, pthread_barrier_t* barrier) {

	// Check if the parameters are invalid
	if (bodies == NULL || start >= end || end > len) {
		return;
	}

	// Loop through all the bodies and calculate each step
	for (size_t i = start; i < end; i++) {
		// Skip if invalid
		if (bodies[i] == NULL) {
			continue;
		}
		// The total sum of the velocity so far
		register double x = bodies[i]->x, y = bodies[i]->y, z = bodies[i]->z;
		register double velocity_x = 0, velocity_y = 0, velocity_z = 0;
		register double mass = bodies[i]->mass;

		// Loop through every other j
		for (size_t j = 0; j < len; j++) {

			// Skip if it is the same
			if (j == i || bodies[j] == NULL || bodies[i] == NULL) {
				continue;
			}

			// Get the distance and the magnitude between the two bodies
			double dist = distance(bodies[j]->x, x, bodies[j]->y, y, bodies[j]->z, z);
			if (dist == 0.0) {
				dist = 0.2;
			}
			double mag = magnitude(bodies[j], bodies[i], dist);

			// Calculate the updated velocities
			velocity_x += ((bodies[j]->x - x)/dist) * mag / mass * dt;
		       	velocity_y += ((bodies[j]->y - y)/dist) * mag / mass * dt;
			velocity_z += ((bodies[j]->z - z)/dist) * mag / mass * dt;
		}

		if (bodies[i] == NULL) {
			continue;
		}

		// Set the old velocities to the updated ones
		bodies[i]->velocity_x += velocity_x;
		bodies[i]->velocity_y += velocity_y;
		bodies[i]->velocity_z += velocity_z;

	}
	// Wait for the barrier
	pthread_barrier_wait(barrier);
	// Loop through all the bodies and update their values after all threads have finished
	for (size_t i = start; i < end; i++) {
		if (bodies[i] == NULL) {
			continue;
		}
		update_body_position(bodies[i], dt);
	}
}


/**
 * Calculate the total energy of the simulation
 * @param bodies, the struct of all bodies
 * @param len, the length of the bodies array
 * @param start, the start index 
 * @param end, the end index
 * @return the total energy of the system, or -1.0 if invalid
 */
double energy(struct body** bodies, size_t len, size_t start, size_t end) {

	// Return if the parameters are invalid
	if (bodies == NULL || len <= 0 || start >= end) {
		return -1.0;
	}

	register double energy = 0.0;
	// Loop through all the bodies
	for (size_t i = start; i < end; i++) {
		if (bodies[i] == NULL) {
			continue;
		}
		double mass = bodies[i]->mass;
		register double x = bodies[i]->x, y = bodies[i]->y, z = bodies[i]->z;
		energy += (mass * (bodies[i]->velocity_x * bodies[i]->velocity_x + bodies[i]->velocity_y * bodies[i]->velocity_y +  bodies[i]->velocity_z * bodies[i]->velocity_z) / 2);
		for (size_t j = i + 1; j < len; j++) {
			energy -= GCONST * (mass * bodies[j]->mass) / distance(bodies[j]->x, x, bodies[j]->y, y, bodies[j]->z, z);
		}
	}

	return energy;
}


/**
 * The worker function for the threads
 * @param arg, the thread data structure
 */
void* worker(void* arg) {
	struct thread_data* tdata = (struct thread_data*)arg;
	tdata->initial_energy = energy(tdata->bodies, tdata->n_bodies, tdata->start, tdata->end);
	while (tdata->iterations-- > 0) {
		pthread_barrier_wait(tdata->barrier);
		step_parallel(tdata->bodies, tdata->n_bodies, tdata->start, tdata->end, tdata->dt, tdata->barrier);
	}
	tdata->final_energy = energy(tdata->bodies, tdata->n_bodies, tdata->start, tdata->end);
	return NULL;
}


/**
 * Given two energy values compare and print out whether they are equal
 * @param initial_energy, the initial energy before the step
 * @param final_energy, the final energy after the step
 */
void compare_energy(double initial_energy, double final_energy) {

	// Compare energy states after and before calls to step
	printf("Initial energy of system: %f\n", initial_energy);
	printf("Final energy of system: %f\n", initial_energy);
	// Check whether final and initial energy of the system match
	if (initial_energy - final_energy < 0.01) {
	  printf("Equal energy states after.\n");
	} else {
	  printf("Energy states not equal ERROR.\n");
	}

	return;
}


/**
 * Generate the array of struct bodies
 * @param n_bodies, the number of bodies 
 * @return bodies struct array containing all randomly generated bodies
 */
struct body** gen_random_bodies(size_t n_bodies) {

	// If the parameter is invalid
	if (n_bodies <= 0) {
		return NULL;
	}

	// Create and allocate the bodies struct array
	struct body** bodies = malloc(sizeof(struct body*) * n_bodies);

	srand(time(NULL));

	// For every single body allocate memory for it and initialise random values
	for (size_t i = 0; i < n_bodies; i++) {
		bodies[i] = malloc(sizeof(struct body));
		bodies[i]->x = (double)(rand() - RAND_MAX/2);
		bodies[i]->y = (double)(rand() - RAND_MAX/2);
		bodies[i]->z = (double)(rand() - RAND_MAX/2);
		bodies[i]->mass = (double)(rand()/(i + 1));
		bodies[i]->velocity_x = (double)(rand() - RAND_MAX/2);
		bodies[i]->velocity_y = (double)(rand() - RAND_MAX/2);
		bodies[i]->velocity_z = (double)(rand() - RAND_MAX/2);
	}
	return bodies;
}


/**
 * Read the bodies from a file into a bodies struct array 
 * @param file_name, the name of the file
 * @return bodies, the struct array of bodies or NULL if invalid
 */
struct body** read_file(FILE* file, size_t n_bodies) {

	// Check if the filename is invalid
	if (file == NULL || n_bodies <= 0) {
		return NULL;
	}

	// Allocate memory for the bodies array struct
	struct body** bodies = malloc(sizeof(struct body*) * n_bodies);

	int r = 1;
	int i = 0;

	// Loop through the files
	while (r != EOF && i < n_bodies) {
		bodies[i] = malloc(sizeof(struct body));
		fscanf(file, "%lf,%lf,%lf,%lf,%lf,%lf,%lf", &(bodies[i]->x), &(bodies[i]->y), &(bodies[i]->z), &(bodies[i]->velocity_x), &(bodies[i]->velocity_y), &(bodies[i]->velocity_z), &(bodies[i]->mass));
		i++;
	}

	return bodies;
}


/**
 * Get the length of a file based on amount of lines
 * Returns -1 if file is null
 */
size_t get_file_len(FILE* f) {
	// If the file is invalid
	if (f == NULL) {
		return -1;
	}

	char c;
	size_t num_lines = 0;
	size_t len = 0;
	
	// Keep reading until EOF
	while ((c = fgetc(f)) != EOF) {
		len++;
		// If we see a new line then increment
		if (len > 8 && c == '\n') {
			num_lines += 1;
			len = 0;
		}
	}
	return num_lines;
}


/**
 * Clear up all memory associated with bodies
 */
void clean_up(struct body** bodies, size_t n_bodies) {
	
	// If the bodies pointer is already NULL
	if (bodies == NULL) {
		return;
	}

	// Loop through the bodies struct array freeing every struct
	for (int i = 0; i < n_bodies; i++) {
		// If it is NULL then skip
		if (bodies[i] == NULL) {
			continue;
		}
		free(bodies[i]);
		bodies[i] = NULL;
	}
	free(bodies);
}


/**
 * Convert a given str into a long value
 * @param var, the variable to copy the conversion into it
 * @param str, the str of the long
 * @return 0 if valid conversion or 1 if NAN or overflow or underflow detected
 */
int long_conversion(size_t* var, char* str) {

	// If invalid return
	if (var == NULL || str == NULL) {
		return 1;
	}
	char* endptr;				// Used for error checking
	*var = strtol(str, &endptr, 10);	// Convert the string into a long

	// Check whether valid or invalid conversion to long
	if ((errno == ERANGE && (*var == LONG_MAX || *var == LONG_MIN)) || (errno != 0 && *var == 0)) {
		perror("strtol");
		return 1;
	}

	// Check whether it did not find a number
	if (endptr == str) {
		fprintf(stderr, "Invalid arguments: not a number.\n");
		return 1;
	}

	return 0;
}


/**
 * Convert a given str into a double value
 * @param var, the variable to copy the conversion into it
 * @param str, the str of the double
 * @return 0 if valid conversion or 1 if NAN or overflow or underflow detected
 */
int double_conversion(double* var, char* str) {

	// If invalid return
	if (var == NULL || str == NULL) {
		return 1;
	}
	char* endptr;				// Used for error checking
	*var = strtod(str, &endptr);		// Convert the string into a long

	// Check whether valid or invalid conversion to long double
	if (endptr == str || (errno == ERANGE && (*var == HUGE_VALL || *var == HUGE_VALF)) || (errno != 0 && *var == 0)) {
		perror("strtod");
		exit(EXIT_FAILURE);
	}

	// Check whether it did not find a number
	if (endptr == str) {
		fprintf(stderr, "Invalid arguments: not a double.\n");
		return 1;
	}

	return 0;
}
