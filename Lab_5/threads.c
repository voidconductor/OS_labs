#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <float.h>

#define MAX_THREADS 16
#define MAX_IMAGE_HEIGHT 1080
#define MAX_IMAGE_WIDTH 1920
#define MAX_NAME 256
#define MAX_BUFF 1024

struct funct_arg {
    int my_num;
    int total_num;
};

unsigned char image[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3];
unsigned char filteredImage[MAX_IMAGE_HEIGHT][MAX_IMAGE_WIDTH][3];
void *sobel_filter(void *arg);

int maxColor,width,height;

// READ FILE
void readPPMImageData(){
    char fileName[MAX_NAME];
    char imageBuff[MAX_BUFF];
    width = 0;
    height = 0;
    maxColor = 0;
    int x;
    int y;

    FILE* file;

    printf("------------------------------------------------------------\n");
    printf("Trying to read .ppm...\n");
    printf("------------------------------------------------------------\n\n");
    printf("Enter the file name (.ppm): ");
    scanf("%s", fileName);
    file = fopen(fileName, "rb");               //open the file specified by the user in binary read mode
    if(file == NULL){                           //but if the file was not found, terminate program
        printf("\nFile %s not found. Exit.\n", fileName);
        exit(1);
    }

    //The first step is to read in the file type and check it agains P6 (file type of .ppm images)
    fgets(imageBuff, MAX_BUFF, file);
    if(imageBuff[0] != 'P' || imageBuff[1] != '6'){
        printf("\nWrong type! Acceptable type is: %s --- Received type is: %c%c\n\n", "P6", imageBuff[0], imageBuff[1]);
    }
    printf("Magic Number is: %c%c\n", imageBuff[0], imageBuff[1]);

    while(width == 0 || height == 0){
        fgets(imageBuff, MAX_BUFF, file);
        if(imageBuff[0] != '#') {
            sscanf(imageBuff, "%d %d", &width, &height);
        }
    }
    printf("Width is: %d\n", width);
    printf("Height is: %d\n", height);
    //if(feof(file)){
    //
    //}

    while(maxColor == 0){
        fgets(imageBuff, MAX_BUFF, file);
        if(imageBuff[0] != '#') {
            sscanf(imageBuff, "%d", &maxColor);
        }
    }
    printf("Maximum color value is: %d\n", maxColor);

    for(x = 0; x < width; x++){
        for(y = 0; y < height; y++){
            image[y][x][0] = (unsigned char)fgetc(file); //Get Red value
            image[y][x][1] = (unsigned char)fgetc(file); //Get Green value
            image[y][x][2] = (unsigned char)fgetc(file); //Get Blue value
        }
    }
    printf("Finished reading image data!\n\n");

    fclose(file);
}
//WRITE NEW FILE
void createPPMImage(){
    char fileName[MAX_NAME];
    FILE* file;
    int x;
    int y;


    printf("Enter the name of new file (.ppm): ");
    scanf("%s", fileName);

    printf("Width is: %d\n", width);
    printf("Height is: %d\n", height);
    printf("Maximum color value is: %d\n", maxColor);

    file = fopen(fileName, "wb");
    fputs("P6\n", file);
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "%d\n", maxColor);

    for(x = 0; x < width; x++){
        for(y = 0; y < height; y++){
            fputc(filteredImage[y][x][0], file); //Write Red value
            fputc(filteredImage[y][x][0], file); //Write Green value
            fputc(filteredImage[y][x][0], file); //Write Blue value
        }
    }
    printf("Finished creating new filtered image!\n\n");

    fclose(file);
}
// MAIN PROGRAM
int main() {
	int res;
	pthread_t a_thread[MAX_THREADS];
	void *thread_result;
	int lots_of_threads;

    struct funct_arg argument;

    readPPMImageData(); //Reading file

    printf("Enter the even number of threads: ");
    scanf("%d", &argument.total_num);

    if(argument.total_num > MAX_THREADS)
    {
        printf("You can't start more than %d threads.\n",MAX_THREADS);
        argument.total_num = MAX_THREADS;
        printf("Number of threads set down to %d now.\n",argument.total_num);
    }
    for(lots_of_threads =0; lots_of_threads < argument.total_num; lots_of_threads++) {
        argument.my_num = lots_of_threads;
        res = pthread_create(&(a_thread[lots_of_threads]),
                            NULL, sobel_filter,
                            &argument);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
    sleep(1);
    }
    printf("Waiting for threads to finish...\n");
    for (lots_of_threads = argument.total_num -1; lots_of_threads >= 0; lots_of_threads--)
    {
        res = pthread_join(a_thread[lots_of_threads], &thread_result);
        if (res == 0) {
            printf("Picked up a thread\n");
        }
        else {
            perror("pthread_join failed");
        }
    }
    createPPMImage(); //creating image
    printf("All done\n");
    exit(EXIT_SUCCESS);
}

void *sobel_filter(void *arg) {
    struct funct_arg *data = arg;


    int horizFilter[3][3] = {{ 1,   0,  -1},
                             { 2,   0,  -2},
                             { 1,   0,  -1}};
    int vertFilter[3][3]  = {{ 1,   2,   1},
                             { 0,   0,   0},
                             {-1,  -2,  -1}};
    double pixVal;
    double horizPixVal;
    double vertPixVal;
    int x, y, i, j;

    int h_str = (height/data->total_num)*data->my_num;
    int h_fin = (height/data->total_num)*(data->my_num+1);
    if(h_str != 0) {h_str--;}
    if(h_fin != width) {h_fin++;}


    for(y = h_str+1; y < h_fin - 1; y++){
       for(x = 1; x < width - 1; x++) {
            pixVal = 0.0;
            horizPixVal = 0.0;
            vertPixVal = 0.0;
                for(j = -1; j <= 1; j++){
                    for(i = -1; i <= 1; i++){
                        horizPixVal += image[y + j][x + i][0] * horizFilter[j+ 1][i + 1];
                        vertPixVal += image[y + j][x + i][0] * vertFilter[j+ 1][i + 1];
                    }
                }
                pixVal = sqrt((horizPixVal * horizPixVal) + (vertPixVal * vertPixVal));
                if(pixVal > 255) pixVal = 255;

                filteredImage[y][x][0] = (unsigned char)pixVal;
        }
    }
    pthread_exit(NULL);
}
