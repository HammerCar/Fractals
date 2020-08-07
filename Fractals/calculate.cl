typedef double2 complex;

inline complex cadd(complex a, complex b)
{
	return (complex)(a.x + b.x, a.y + b.y);
}

inline complex cmult(complex a, complex b) {
	return (complex)(a.x*b.x - a.y*b.y, a.x*b.y + a.y*b.x);
}

inline float cabs(complex a)
{
	return (sqrt(a.x*a.x + a.y*a.y));
}


__kernel void calculate(__global const double* parameters, __global char* values)
{
	// Get the index of the current element to be processed
	int id = get_global_id(0);

	double re = id % WIDTH;
	double im = id / WIDTH;
	double posX = parameters[0]; // X_POS
	double posY = parameters[1]; // Y_POS
	double width = WIDTH;
	double zoom = ZOOM;

	// z = z^2 + c, determines if a single complex number (pixel) is in the
	// mandelbrot set.
	int currentIteration = 0;

	// Convert the current pixel to a complex number.
	// Re and Im are multiplied by 4 to scale the image properly. Offset -2
	// centers the image.

	double zr = 0;
	double zi = 0;
	double cr = re * (4 / zoom) / width + (posX - (2 / zoom));
	double ci = im * (4 / zoom) / width + (-posY - (2 / zoom));

	//complex z = (0, 0);
	//complex c = (re * (4 / zoom) / width + ( posX - (2 / zoom)), 
	//			 im * (4 / zoom) / width + (-posY - (2 / zoom)));

	// The complex number is not in the set if it escapes to infinity.
	while (pow(zr, 2) + pow(zi, 2) < 4 /*cabs(z) < 2*/ && currentIteration < MAX_ITERATIONS)
	{
		//z = pow(z, 2) + c;

		//z = cadd(cmult(z, z), c);

		double zrTemp = pow(zr, 2) - pow(zi, 2) + cr;
		zi = 2 * zr * zi + ci;
		zr = zrTemp;

		//zr = pow(zr, 2) - pow(zi, 2) + cr;
		//zr = 2 * zr * zi + ci;

		currentIteration++;
	}

	// Returns a color value for a single pixel.
	if (currentIteration < MAX_ITERATIONS)
		values[id] = 255 * ((double)currentIteration / MAX_ITERATIONS);
	else
		values[id] = 0;
}
