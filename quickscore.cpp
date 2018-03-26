
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include "matrix.h"
#include "mex.h"

#include "common.h"
#include "quickscore.h"
#include "image.h"
#include "patch.h"
#include "score.h"
#include "location.h"

#define BOOST_LIB_DIAGNOSTIC

void mArrayToLocs(const mxArray *p, Location ** loc, int32_t *length, int8_t *foreground,
        size_t *foreground_dim, int64_t *foreground_scores);

//templateID holds pointer to templates array
//id will be assigned in each location and then
//converted to a pointer
bool startup = true;
int32_t templateIDSize = 20000;
Image ** templateID;

int64_t * results;
int frame;

inline Image * IDtoImagePtr(int32_t id) {
    if (id < templateIDSize)
        return templateID[id];
    printf("ERROR ID TOO LARGE\n");
    return NULL;
}

//this is what the call shoul look like
//results = quickscore(frame, patches, bool foreground, int num_foreground)
void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[]) {
    if (startup) {
        templateID = (Image **)malloc(sizeof(Image*) * templateIDSize);

        results = NULL;
        startup = false;
    }

    //here we check to see if the enable foreground flag is true
    bool enable_foreground = false;
    if (nrhs == 5) {
        //check if use foreground is true
		enable_foreground = true;
            //mexPrintf("foreground enabled/n");
    } else if (nrhs != 3) {
        //return if not right number of parametrs
        return;
    }
    //frame will load same regardless if foreground is enabled
    Patch * frame;
    int32_t numImage;
    mStructToImage(prhs[0], &frame, &numImage);

    //here we are loading the list of patches
    Patch *patches;
    int32_t numTemplates;
    mStructToImage(prhs[1], &patches, &numTemplates);


    //finally, we load the locations. this may or 
    //may not include the foreground estimation.
    Location * locations;
    int32_t numLocations;
    int8_t * foreground_input;

    if (!enable_foreground) {
        mArrayToLocs(prhs[2], &locations, &numLocations);
    } else {

        //this should contain the number of foreground patches to create
		int * m_ptr = (int*)mxGetData(prhs[3]);
        int num_foreground = *m_ptr;

        //represents the dimensions of the foreground arrays
        size_t foreground_dim[3] = {frame->y_, frame->x_, num_foreground};
        //this is what is goin to ultimately be returned to matlab
        mxArray * m_foregrounds = mxCreateNumericArray(3, foreground_dim, mxINT8_CLASS, mxREAL);
		//plhs[1] = m_foregrounds;

		int8_t * foreground = (int8_t *)mxGetData(m_foregrounds);
        //allocate memory for foreground comparision
        //this is an overided version of mArrayToLocs that also
        //computes foreground patches

        mxArray * m_foreground_scores = mxCreateNumericMatrix(num_foreground, 1, mxINT64_CLASS, mxREAL);
        plhs[1] = m_foreground_scores;
        int64_t * foreground_scores = (int64_t*)mxGetData(m_foreground_scores);

        mArrayToLocs(prhs[2], &locations, &numLocations, foreground, foreground_dim, foreground_scores);

        //now we need to get the input foreground comparision patch
        foreground_input = (int8_t *)mxGetData(prhs[4]);
        

    }

    
    //So i create an mxArray pointer with dimension 0x0, this avoids
    //zeroing out the array which in matlab is expensive
    //then i set the dimensions, allocate memory, and set the mxarray
    //pointer to the proper place
    plhs[0] = mxCreateNumericMatrix(0, 0, mxINT64_CLASS, mxREAL);
    mxSetM(plhs[0], numLocations);
    mxSetN(plhs[0], 1);

    results = (int64_t *) mxCalloc(numLocations, sizeof(int64_t));
    mxSetData(plhs[0], results);

   
    if (enable_foreground) {
        templateScoring((Image *)frame, locations, numLocations, results, foreground_input);
    } else {
        templateScoring((Image *)frame, locations, numLocations, results);
    }

    
    
    free(frame);
    free(locations);
    free(patches);
}

void printResults (int64_t * results, int32_t n) {
    for (int32_t i = 0; i < n; i++) {
        printf("score %i: %i\n", i, results[i]);
    }
}

//this needs more error checking
//p is the pointer to the matlab image structure
//img is the c structure that contains the metadata
//arrayLength is the length of the image array
//for whatever reason this is following the matlab column major format
//hidous but less processing
void mArrayToLocs(const mxArray *p, Location ** loc, int32_t *length) {
    //put type checking here
    
    mwSize *loc_dim = (mwSize *) mxGetDimensions(p);
     
     *length = loc_dim[0]; //number of entries

    int32_t x_dim = loc_dim[1];
    
    *loc = (Location *) malloc(sizeof(Location) * (*length));

    int32_t *mp = (int32_t *)mxGetData(p);

    for (int32_t n = 0; n < *length; n++) {

        Location * loc_p = *(loc)+n;
        int32_t * mpp = mp + n;

        loc_p->x = *(mpp);
        loc_p->y = *(mpp+*length);
        loc_p->theta = *(mpp+2*(*length));

        int32_t id = *(mpp+3**length);
        loc_p->image = (Patch *)templateID[id];
    }
}

//this method with allocate location and return a value for length.
//however, it expects that foreground be already allocated 
//and than num_foreground be equal to the number of entries 
void mArrayToLocs(const mxArray *p, Location ** loc, int32_t *length, int8_t *foreground,
        size_t *foreground_dim, int64_t *foreground_scores) {
    //put type checking here
    
    mwSize *loc_dim = (mwSize *) mxGetDimensions(p);
     *length = loc_dim[0]; //number of entries
    int32_t x_dim = loc_dim[1];
    //allocate loc
    *loc = (Location *) malloc(sizeof(Location) * (*length));

    //this is a matlab data pointer that references the
    //arrray of locations. we will convert it into the
    //c style locations data type
    int32_t *mp = (int32_t *)mxGetData(p);

    for (int32_t n = 0; n < *length; n++) {

        Location * loc_p = *(loc)+n;
        int32_t * mpp = mp + n;

        loc_p->x = *(mpp);
        loc_p->y = *(mpp+*length);
        loc_p->theta = *(mpp+2*(*length));

        int32_t id = *(mpp+3**length);
        loc_p->image = (Patch *)templateID[id];

        int32_t foreground_id = *(mpp+4**length);
        //foreground id should be less than number of foreground frames total
        if (foreground_id == -1) {
            loc_p->foreground_frame = NULL;
        } else {
        //this needs to be tested bad
        //basically start at foreground and advance id *(w*h)
            int8_t *foreground_pointer = 
                (foreground + foreground_id*(foreground_dim[0]*foreground_dim[1]));

            loc_p->foreground_frame = foreground_pointer;

            int64_t * score_pointer = foreground_scores + foreground_id;
            loc_p->foreground_score = score_pointer;
        } 
    }
}


//returns a new pointer for img. expects arrayLength to already be allocated.
//for security reasons.
//using structures avoids the column major but maybe slower??
void mStructToImage(const mxArray *p, Patch ** img, int32_t *arrayLength) {
    if (!mxIsStruct(p)) {
        mexErrMsgIdAndTxt("MATLAB:imageTest:inputNotSTruct", "Input must be a structure");
    }
    *arrayLength = mxGetNumberOfElements(p);
    //*img = (Patch *)malloc(sizeof(Patch) * (*arrayLength));
    *img = new Patch[*arrayLength];
    for (int32_t n = 0; n < *arrayLength; n++) {
        
        //okay so what we got going on here is img_p is image_pointer iterator
        //iptr is interger pointer
        //and fpr is matlabs field pointer
        //fpr points to field in matlab
        //ipr points to the actual cell
        //copy into appropriate language
        Patch * img_p = *(img)+n;
        int32_t * iptr;
        mxArray *fpr;
        fpr = mxGetField(p,n,"x");

        iptr = (int32_t *)mxGetData(fpr);

        img_p->x_ = *iptr;

        
        fpr = mxGetField(p,n,"y");
        iptr = (int32_t *) mxGetData(fpr);
        img_p->y_ = *iptr;
        
        fpr = mxGetField(p,n,"n");
        iptr = (int32_t *) mxGetData(fpr);
        img_p->n_ = *iptr;

        fpr = mxGetField(p, n, "rgb");
        uint8_t * ui8ptr;
        ui8ptr = (uint8_t*) mxGetData(fpr);
        img_p->rgb_ = ui8ptr;
        
        fpr = mxGetField(p, n,"heatMap");
        double * dptr;
        dptr = (double*) mxGetData(fpr);
        img_p->heat_map_ = dptr;
       
        fpr = mxGetField(p,n,"id");
        iptr = (int32_t*) mxGetData(fpr);
        int32_t id = *iptr;
        img_p->id_=id;
        templateID[id] = img_p;
    }

    
    return;
}
