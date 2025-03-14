#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <MagickCore/MagickCore.h>

#include "predprey.h"

unsigned int report_interval = 0;
unsigned int frame_interval;

ExceptionInfo *exception;
Image *camera_roll;
ImageInfo *camera_info;
unsigned char *camera_buffer;

void output_init(lattice_t *lattice, int quiet, unsigned int steps_per_report,
		char *path, char *outfile, unsigned int steps_per_frame) {

	if (lattice == NULL)
		fail("output_init(): lattice cannot be NULL");

	if (!quiet) {
		report_interval = steps_per_report;

		// Display run details
		printf("Simulating %dx%d lattice for %d time steps with rates:\n"
			"lambda = %f\nmu = %f\nsigma = %f\n", lattice->size, lattice->size,
			lattice->maxtime, lattice->lambda, lattice->mu, lattice->sigma);

		// If giving regular density reports, define columns now
		if (steps_per_report != 0) {
			printf("\ntime\tpredator density\tprey density\n");
		}
	}

	frame_interval = steps_per_frame;

	// Initialize image buffer and MagickCore structures
	camera_buffer = (unsigned char *)malloc(sizeof(unsigned char) * lattice->size * lattice->size * 3);
	if (camera_buffer == NULL)
		fail("output_init(): malloc() failed");

	MagickCoreGenesis(path, MagickTrue);
	exception = AcquireExceptionInfo();
	camera_roll = NewImageList();
	camera_info = CloneImageInfo((ImageInfo *)NULL);
	camera_info->file = fopen(outfile, "w+b");
	if (camera_info->file == NULL)
		fail("output_init(): couldn't open output file");
	strcpy(camera_info->filename, outfile);
	strcpy(camera_info->magick, "gif");
}

static void draw_frame(lattice_t *lattice) {
	// Fill buffer with pixel data
	for (unsigned int y = 0; y < lattice->size; y++) {
		for (unsigned int x = 0; x < lattice->size; x++) {
			size_t buffer_idx = (y * lattice->size + x) * 3;

			switch(get_cell(lattice, x, y)) {
				case PREDATOR:
					camera_buffer[buffer_idx] = 0xFF;
					camera_buffer[buffer_idx + 1] = 0x00;
					camera_buffer[buffer_idx + 2] = 0x00;
					break;

				case PREY:
					camera_buffer[buffer_idx] = 0x00;
					camera_buffer[buffer_idx + 1] = 0xFF;
					camera_buffer[buffer_idx + 2] = 0x00;
					break;

				case EMPTY:
				default:
					camera_buffer[buffer_idx] = 0xFF;
					camera_buffer[buffer_idx + 1] = 0xFF;
					camera_buffer[buffer_idx + 2] = 0xFF;
			}
		}
	}

	// Build frame from pixel data and add to camera roll
	Image *camera = ConstituteImage(lattice->size, lattice->size, "RGB",
		CharPixel, camera_buffer, exception);
	if (camera == NULL) {
		CatchException(exception);
		fail("draw_frame(): ConstituteImage() failed");
	}
	AppendImageToList(&camera_roll, camera);
}

void output(lattice_t *lattice) {
	if (lattice == NULL)
		fail("output(): lattice cannot be NULL");

	if (report_interval != 0 && lattice->time % report_interval == 0) {
		printf("%d\t%f\t\t%f\n", lattice->time, predator_density(lattice),
			prey_density(lattice));
	}

	if (lattice->time % frame_interval == 0) {
		draw_frame(lattice);
	}
}

void output_teardown(lattice_t *lattice) {
	if (report_interval != 0) {
		printf("Writing image data to %s...", camera_info->filename);
		fflush(NULL);
	}
	if (WriteImage(camera_info, camera_roll, exception) != MagickTrue) {
		CatchException(exception);
		fail("output_teardown(): failed to write image data");
	}
	fclose(camera_info->file);
	if (report_interval != 0)
		printf(" Done!\n");

	// Free everything
	DestroyImageInfo(camera_info);
	DestroyImageList(camera_roll);
	DestroyExceptionInfo(exception);
	MagickCoreTerminus();
	free(camera_buffer);
}
