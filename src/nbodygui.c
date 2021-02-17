#include "nbody.h"
#include "functions.c"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#define MAX_RADIUS (20)

/**
 * Process the arguments for gui appliation 
 * @param argv, the arguments to parse
 * @param width, the width of the window
 * @param height, the height of the window
 * @param n_iterations, the number of iterations
 * @param n_bodies, the number of bodies
 * @param dt, the amount to step for each of the iterations
 * @param scale, the scale of the window 
 * @return the malloced bodies array of body structs
 */
struct body** process_arguments(char** argv, size_t* width, size_t* height, size_t* n_iterations, size_t* n_bodies, double* dt, double* scale) {

	// Extract the arguments from the command line
	if (long_conversion(width, argv[1])) return NULL;		// Convert the width
	if (long_conversion(height, argv[2])) return NULL;		// Convert the height
	if (long_conversion(n_iterations, argv[3])) return NULL;	// Conert the iterations
	if (double_conversion(dt, argv[4])) return NULL;		// Conert the dt
	if (double_conversion(scale, argv[7])) return NULL;		// Conert the scale

	struct body** bodies = NULL;

	// If it is searching for a file
	if (strncmp(argv[5], "-f", 3) == 0) {
	
		FILE* file = fopen(argv[6], "r");		// Open the file in read mode

		// If the file is not found
		if (file == NULL) {				
			fprintf(stderr, "Cannot find file.\n");
			return NULL;
		}

		*n_bodies = get_file_len(file);			// Get the length of the file
		rewind(file);					// Rewind back to the start of the file
		bodies = read_file(file, *n_bodies);		// Read the file from the array
		fclose(file);					// Close the file

	} else if (strncmp(argv[5], "-b", 3) == 0) {

		if (long_conversion(n_bodies, argv[6])) {	// Get the number of bodies
			return NULL;
		}

		bodies = gen_random_bodies(*n_bodies);	// Generate the random bodies
	} 

	// If invalid then print error message and exit
	else {
		printf("Invalid usage,\n./nbody-gui <resolution_width> <resolution_height> <iterations> <dt> (-b <bodies> | -f <filename>) (scale)\n");
		return NULL;
	}

	return bodies;
}


/**
 * Given a parameter it will scale to the bounds specified given window size
 * @param bound, the bound of the window
 * @param min, the min value 
 * @param max, the max value
 * @param x, the value to normalise
 * @return the normalised value
 */
double scale_point(size_t a, size_t b, double min, double max, double x) {
	// If invalid max and min then set to 1
	if (x == 0 || max - min == 0) {
		return a + x;
	}

	return ((x - min) * (b - a))/(max - min) + a;
}


int main(int argc, char** argv) {

	// Check if valid number of arguments have been passed
	if (argc != 8) {
		printf("Invalid usage,\n./nbody-gui <resolution_width> <resolution_height> <iterations> <dt> (-b <bodies> | -f <filename>) (scale)\n");
		return 1;
	}

	//Your SDL variables and variables for circle data and finished state
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Event event;
	int finished = 0;
	
	//Initialises SDL Video Acceleration
	int init_value = SDL_Init(SDL_INIT_VIDEO);

	//Check if return value is < 0
	if (init_value < 0) {
		printf("Error while initiating video.\n");
		return 1;
	}
	
	// Retrieve the arguments
	size_t width, height, n_iterations, n_bodies;
	double dt, scale;
	struct body** bodies = process_arguments(argv, &width, &height, &n_iterations, &n_bodies, &dt, &scale);

	// If invalid arguments return
	if (bodies == NULL || n_bodies == 0) {
		return 1;
	}

	// If width or height is less than 0 give error message
	if (width <= 0 || height <= 0) {
		printf("Invalid width or height <= 0.\n");
		clean_up(bodies, n_bodies);
		return 1;
	}

	// Get the min and max values
	double min_x = bodies[0]->x, min_y = bodies[0]->y;
	double max_x = bodies[0]->x, max_y = bodies[0]->y;
	double max_mass = bodies[0]->mass;
	for (size_t i = 1; i < n_bodies; i++) {
		min_x = (min_x < bodies[i]->x) ? min_x : bodies[i]->x;
		min_y = (min_y < bodies[i]->y) ? min_y : bodies[i]->y;
		max_x = (max_x > bodies[i]->x) ? max_x : bodies[i]->x;
		max_y = (max_y > bodies[i]->y) ? max_y : bodies[i]->y;
		max_mass = (max_mass > bodies[i]->mass) ? max_mass : bodies[i]->mass;
	}

	// Update x positions
	max_x *= 2;
	max_y *= 2;

	// If chance of getting divide by 0 errors
	if (max_y == 0) {
		max_y = height;
	}

	if (max_x == 0) {
		max_x = width;
	}

	/**
	 * Creates a window to display
	 * Allows you to specify the window x,y position
	 * Allows you to specify the width and height of the window
	 * Option for flags for the window and potentially the different instances
	 * you may want to use with it. SHOWN, makes the window visible
	 * Should check if it returns NULL
	 */
	window = SDL_CreateWindow("SDL Template",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height, SDL_WINDOW_SHOWN);

	// Check if invalid window returned
	if (window == NULL) {
		printf("Error while attempting to create to window.\n");
		clean_up(bodies, n_bodies);
		return 1;
	}
	
	/**
	 * Create Renderer retrieves the 2D rendering context of the window
	 * You will need to use this to use with SDL2_gfx library 
	 * This will give you access to the underlying rederer of the window
	 * Should check if it returns NULL
	 */
	renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED 
		| SDL_RENDERER_PRESENTVSYNC);

	// Check if the renderer creation fails
	if (renderer == NULL) {
		printf("Error while creating renderer.\n");
		clean_up(bodies, n_bodies);
		return 1;
	}
	

	// Get values for managing scale and ratio
	double x_ratio = (width - width/10)/(max_x + max_y);
	double y_ratio = (height - height/10)/(max_x + max_y);

	/**
	 * Render loop of your application
	 * You will perform your drawing in this loop here
	 */
	while(!finished && n_iterations-- > 0) {
		//Sets the background colour 
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);

		//Clears target with a specific drawing colour (prev function defines colour)
		SDL_RenderClear(renderer);
	
		//Updates the positions of bodies
		step(bodies, n_bodies, dt);

		//Draws a circle using a specific colour
		//Pixel is RGBA (0x(RED)(GREEN)(BLUE)(ALPHA), each 0-255
		for (size_t i = 0; i < n_bodies; i++) {
			filledCircleColor(renderer, x_ratio * bodies[i]->x + width/2, y_ratio * bodies[i]->y + height/2, scale_point(2, MAX_RADIUS, 0, max_mass, bodies[i]->mass) * scale, 0xFF0000FF);
		}

		//Updates the screen with newly renderered image
		SDL_RenderPresent(renderer);

		//Retrieves the events captured from SDL (just watching for windows close)
		if(SDL_PollEvent(&event)) {
			finished = (event.type == SDL_QUIT);
		}		
	}
	
	//Clean up functions
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();	
	clean_up(bodies, n_bodies);

	return 0;	
}
