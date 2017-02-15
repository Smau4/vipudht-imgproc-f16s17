#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <gphoto2/gphoto2.h>

#include "stdafx.h"
#include "process_image.h"

static void
capture_to_memory(Camera *camera, GPContext *context, const char **ptr, unsigned long int *size) 
{
	int retval;
	CameraFile *file;
	CameraFilePath camera_file_path;

	printf("Capturing.\n");
	/* NOP: This gets overridden in the library to /capt0000.jpg */
	strcpy(camera_file_path.folder, "/");
	strcpy(camera_file_path.name, "foo.jpg");

	retval = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context);
	printf("  gp_camera_capture: %d\n", retval);

	printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

	retval = gp_file_new(&file);
	printf("  gp_file_new returned: %d\n", retval);
	retval = gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name,
		     GP_FILE_TYPE_NORMAL, file, context);
	printf("  gp_camera_file_get: %d\n", retval);

	gp_file_get_data_and_size (file, ptr, size);

	//printf("Deleting.\n");
	//retval = gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name, context);
	//printf("  gp_camera_file_detect: %d\n", retval);
	gp_file_free(file); 
}

static void
capture_to_file(Camera *camera, GPContext *context, char *fn) {
	int fd, retval;
	CameraFile *file;
	CameraFilePath camera_file_path;

	printf("Capturing.\n");

	/* NOP: This gets overridden in the library to /capt0000.jpg */
	strcpy(camera_file_path.folder, "/");
	strcpy(camera_file_path.name, "foo.jpg");

	retval = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context);
	printf("  Retval: %d\n", retval);

	printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

	fd = open(fn, O_CREAT | O_WRONLY, 0644);
	retval = gp_file_new_from_fd(&file, fd);
	printf("  Retval: %d\n", retval);
	retval = gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name,
		     GP_FILE_TYPE_NORMAL, file, context);
	printf("  Retval: %d\n", retval);

	printf("Deleting.\n");
	retval = gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name,
			context);
	printf("  Retval: %d\n", retval);

	gp_file_free(file);
}



int main(int argc, char **argv) 
{	
	/* Initialize libgphoto */
	Camera	*camera;
	int	retval;
	GPContext *context = sample_create_context();
	char	*data;
	unsigned long size;
	struct timeval tval;

	gp_camera_new(&camera);

	printf("Camera init.  Takes about 10 seconds.\n");
	retval = gp_camera_init(camera, context);
	if (retval != GP_OK) {
		printf("gp_camera_init: %d\n", retval);
		exit (1);
	}

	/* main loop */	
	while (1) 
	{	
		// capture raw image
		capture_to_memory(camera, context, (const char**)&data, &size);
				
		// process raw image / save targets to disk
		process_image(data, size);

		// wait for x seconds
		sleep(4);
	}	

	// Cleanup
	gp_camera_exit(camera, context);
	gp_camera_free(camera);
	gp_context_unref(context);
	return 0;
}
