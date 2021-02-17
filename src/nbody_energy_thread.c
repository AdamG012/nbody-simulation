#include "nbody.h"
#include "functions.c"

/**
 * Manage the threaded runtime of the nbody simulation
 * @param N_THREADS, number of threads
 * @param bodies, the array of the body objects
 * @param n_bodies, the number of bodies
 * @apram iterations, the number of iterations to perform
 * @param dt, the step for iterations
 */
void run_threaded(size_t N_THREADS, struct body** bodies, size_t n_bodies, size_t iterations, double dt) {
	if (N_THREADS == 0) {
		return;
	}

	// Initialise the barrier
	pthread_barrier_t barrier;
	if (pthread_barrier_init(&barrier, NULL, N_THREADS)) {
		fprintf(stderr, "Error initialising barrier.\n");
		return;
	}

	// Create the threads and data ptrs
	pthread_t* threads = malloc(sizeof(pthread_t) * N_THREADS);		
	struct thread_data** tdata = malloc(sizeof(struct thread_data*) * N_THREADS);
	for (size_t i = 0; i < N_THREADS; i++) {
		tdata[i] = malloc(sizeof(struct thread_data));
	}

	size_t thread_segment = n_bodies/N_THREADS;

	size_t energy_segment = (n_bodies * (n_bodies + 1))/(2 * N_THREADS);
	size_t start = 0, end = 1;

	for (size_t i = 0, j = 0, total = 0; j < N_THREADS && i < n_bodies; i++) {
		total += (n_bodies - i);
		if (total >= energy_segment) {
			tdata[j]->e_start = start;
			tdata[j]->e_end = end;
			total = 0;
			start = end;
			j++;
		}

		else if (j == N_THREADS - 1) {
			tdata[j]->e_start = start;
			tdata[j]->e_end = n_bodies;
			break;
		}
		end++;
	}

	// Declare initial and final energy to compare
	register double initial_energy = 0.0;
	register double final_energy = 0.0;

	// Loop through and initialise the thread data
	for (size_t i = 0; i < N_THREADS; i++) {
		tdata[i]->bodies = bodies;
		tdata[i]->n_bodies = n_bodies;
		tdata[i]->iterations = iterations;
		tdata[i]->start = i * thread_segment;
		tdata[i]->end = (i + 1) * thread_segment;
		tdata[i]->initial_energy = 0;
		tdata[i]->final_energy = 0;
		tdata[i]->barrier = &barrier;
		tdata[i]->dt = dt;

		// If it is final thread then complete the rest
		if (i == N_THREADS - 1) {
			tdata[i]->end = n_bodies;
		}
		pthread_create(threads+i, NULL, worker, tdata[i]);
	}

	for (int i = 0; i < N_THREADS; i++) {
		pthread_join(threads[i], NULL);
		initial_energy += tdata[i]->initial_energy;
		final_energy += tdata[i]->final_energy;
	}

	compare_energy(initial_energy, final_energy);

	// Free data in each thread
	for (size_t i = 0; i < N_THREADS; i++) {
		free(tdata[i]);
	}

	// Deallocate mmeory for threads and data
	free(threads);
	free(tdata);
	pthread_barrier_destroy(&barrier);

}


/**
 * Initalise the program using the given starting parameters
 * @param bodies, the struct array of all the bodies
 * @param n_bodies, the number of bodies
 * @param iterations, the number of iterations
 * @param df, the rate of change 
 * @param is_threaded, whether to activate parallelism or not
 */
void init(struct body** bodies, size_t n_bodies, size_t iterations, double dt, int is_threaded, size_t N_THREADS) {
	// Run a threaded solution
	if (is_threaded) {			
		run_threaded(N_THREADS, bodies, n_bodies, iterations, dt);
		return;
	}

	double initial_energy = 0, final_energy = 0;		// The final and start energies of the system

	initial_energy = energy(bodies, n_bodies, 0, n_bodies);	// Get the initial energy of the system
	// Step through a single threaded implementation
	while (iterations-- > 0) {
		step(bodies, n_bodies, dt);
		initial_energy = energy(bodies, n_bodies, 0, n_bodies);	// Get the initial energy of the system
	}
	final_energy = energy(bodies, n_bodies, 0, n_bodies);	// Get the energy of the system after exiting
	compare_energy(initial_energy, final_energy);


}


int main(int argc, char** argv) {
	// Check if the number of arguments is valid
	if (argc != 5 && argc != 7) {
		fprintf(stderr, "Invalid number of arguments.\nUsage: ./nbody <iterations> <change_of_time> <-b n_bodies | -f file_name> [ -t NUM_THREADS]\n");
		return 1;
	}

	size_t n_iterations = 0, n_bodies = 0;			// Declare required variables
	int is_threaded = 0;
	struct body** bodies = NULL;
	size_t N_THREADS = 1;

	// Check for the threaded parameter argument
	if (argc == 7) {
		if (strncmp(argv[5], "-t", 3) == 0) {		// Check if we want a threaded run
			is_threaded = 1;
		} else {
			fprintf(stderr, "Invalid option.\nUsage: ./nbody <iterations> <change_of_time> <-b n_bodies | -f file_name> [ -t NUM_THREADS]\n");
			return 1;
		}
		if (long_conversion(&N_THREADS, argv[6]) || N_THREADS == 0) {
			printf("Invalid number of threads.\n");
			return 1;
		}
	}


	// Get the long conversion of the iterations
	if (long_conversion(&n_iterations, argv[1])) {
		printf("Invalid number of iterations.\n");
		return 1;
	}

	// Get the conversion of the double
	double dt = 0.01;
	if (double_conversion(&dt, argv[2])) {
		printf("Invalid dt value.\n");
		return 1;

	}

	// If it is searching for a file
	if (strncmp(argv[3], "-f", 3) == 0) {
	
		FILE* file = fopen(argv[4], "r");		// Open the file in read mode
		// If the file is not found
		if (file == NULL) {				
			fprintf(stderr, "Cannot find file.\n");
			return 1;
		}
		n_bodies = get_file_len(file);			// Get the length of the file
		rewind(file);					// Rewind back to the start of the file
		bodies = read_file(file, n_bodies);		// Read the file from the array
		fclose(file);					// Close the file

	} else if (strncmp(argv[3], "-b", 3) == 0) {
		
		// Get the number of bodies
		if (long_conversion(&n_bodies, argv[4])) {
			printf("Invalid n_bodies value.\n");
			return 1;
		}
		bodies = gen_random_bodies(n_bodies);		// Generate the random bodies

	} else {
		fprintf(stderr, "Invalid choice use -b or -f.\nUsage: ./nbody <iterations> <change_of_time> <-b n_bodies | -f file_name> [ -t NUM_THREADS]\n");
		return -1;
	}

	// If too many threads
	if (N_THREADS > n_bodies) {
		fprintf(stderr, "Too many threads > n_bodies.\n");
		return 1;
	}

	init(bodies, n_bodies, n_iterations, dt, is_threaded, N_THREADS);		// Initialise the steps
	clean_up(bodies, n_bodies);					// Clean up the bodies array
	return 0;
}
