#include <cstdint>
#include <cstring>
#include <stddef.h>
#include "../common.h"
#include "../patch.h"

void mexFunction(int nlhs, mxArray *plhs[],
        int nrhs, const mxArray *prhs[]) {
    
    uint8_t *input_rgb = (uint8_t *)mxGetData(prhs[0]);
    int32_t *input_x = (int32_t *)mxGetData(prhs[1]);
    int32_t *input_y = (int32_t *)mxGetData(prhs[2]);
    int32_t *input_theta = (int32_t *)mxGetData(prhs[3]);
   
    int32_t input_size_heatmap = *input_x * *input_y;
	double *input_heatmap = new double[input_size_heatmap];

	for (int i = 0; i < *input_x * *input_y; i++) {
		input_heatmap[i] = 1.0;
	}
    Patch input_image(*input_x, *input_y, input_rgb, input_heatmap);



    Patch output_image(0, 0, 0, 0);
    //Image square_image(0,0,0);
    
    rotateImage(&input_image, output_image, *input_theta);
    
    int32_t dims[3] = {output_image.y_, output_image.x_, 3};
    int32_t ndims = 3;
    
	plhs[0] = mxCreateNumericArray(ndims, dims, mxUINT8_CLASS, mxREAL);
    uint8_t *output_rgb = (uint8_t *)mxGetData(plhs[0]);
	memcpy(output_rgb, output_image.rgb_, output_image.imageSize());

    plhs[1] = mxCreateDoubleMatrix(output_image.y_, output_image.x_, mxREAL);
    double * output_heatmap = (double *)mxGetPr(plhs[1]);
    memcpy(output_heatmap, output_image.heat_map_, output_image.heatMapSize());
    
}
