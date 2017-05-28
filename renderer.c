#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>

extern int getRowNum();
extern uint64_t TIME;
int RENDER_WIDTH = 720;
int RENDER_HEIGHT = 480;
double RENDER_RATIO = 0;
int MAX_STEPS = 1500;
int *stages = 0;
double CAMERA_RES = .04;
int SEGMENTS = 8;
enum SHAPES{SPHERE, MATH};
typedef enum SHAPE SHAPE;
extern char* FILENAME;
png_bytepp rows;
void printDouble(double x1){
    printf("DOUBLE: %f\n", x1);
}
double lastV = -1;

double convertLLUtoDouble(uint64_t num){
    return num;
}

double USER_FUNC_abs(double a){
    return abs(a);
}

double USER_FUNC_sin(double a){
    return sin(a);
}

double USER_FUNC_cos(double a){
    return cos(a);
}

double USER_FUNC_tan(double a){
    return tan(a);
}

double USER_FUNC_asin(double a){
    return asin(a);
}

double USER_FUNC_acos(double a){
    return acos(a);
}

double USER_FUNC_atan(double o, double a){
    return atan2(o, a);
}

double USER_FUNC_pow(double base, double exp){
    return pow(base, exp);
}

double isLT(double a1, double a2){
    return a1 < a2;
}

double isLTE(double a1, double a2){
    return a1 <= a2;
}

double isGT(double a1, double a2){
    return a1 > a2;
}

double isGTE(double a1, double a2){
    return a1 >= a2;
}

double isEq(double a1, double a2){
    return a1 == a2;
}

double isNeq(double a1, double a2){
    return a1 != a2;
}

double plus(double a1, double a2){
    return a1 + a2;
}

double sub(double a1, double a2){
    return a1 - a2;
}

double mul(double a1, double a2){
    return a1 * a2;
}

double divide(double a1, double a2){
    return a1 / a2;
}

double mod(double a1, double a2){
    double store = a1 - (((int)(a1/a2)) * a2);
    if(store < 0){
        return a2 + store;
    }
    return store;
}

double and(double a1, double a2){
    return a1 && a2;
}

double or(double a1, double a2){
    return a1 || a2;
}

typedef struct ImageHolder{
    int width;
    int height;
    int mode;
    int depth;
    png_bytepp imageData;
    char* name;
    struct ImageHolder* next;
} ImageHolder;

typedef struct Vertex{
    double x;
    double y;
    double z;
    struct Vertex* next;
} Vertex;

typedef struct Face{
    Vertex* data;
    struct Face* next;
} Face;

ImageHolder *textures = 0;

void addIH(ImageHolder* IH){
    (IH -> next) = textures;
    textures = IH;
}

void readImage(char* name){
    int length = strlen(name) + 5;
    char newName[length];
    for(int i = 0; i < length - 5; i++){
        newName[i] = name[i];
    }
    newName[length - 5] = '.';
    newName[length - 4] = 'p';
    newName[length - 3] = 'n';
    newName[length - 2] = 'g';
    newName[length - 1] = 0;
    char* filename = (char*) &newName;
    printf(":D %s\n", filename);
    FILE* texture_pointer = fopen(filename, "rb");
    png_structp texture_struct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop texture_info = png_create_info_struct(texture_struct);
    png_init_io(texture_struct, texture_pointer);
    png_read_png(texture_struct, texture_info, 0, NULL);
    png_bytepp rows = png_get_rows(texture_struct, texture_info);
    int width = png_get_image_width(texture_struct, texture_info);
    int height = png_get_image_height(texture_struct, texture_info);
    ImageHolder *IH = malloc(sizeof(ImageHolder));
    (IH -> width) = width;
    (IH -> height) = height;
    (IH -> imageData) = rows;
    (IH -> name) = name;
    (IH -> mode) = png_get_color_type(texture_struct, texture_info);
    (IH -> depth) = png_get_bit_depth(texture_struct, texture_info);
    addIH(IH);
    printf("COLOR %d %d %d %d\n", rows[0][0], rows[0][1], rows[0][2], rows[0][3]);
    printf( "{: %p\n", texture_pointer);
    fclose(texture_pointer);
    printf("READING IMAGE: %s\n", newName);
}

void readObj(char* name){
    FILE* objfile = fopen(name,"r");
    fseek(objfile, 0, SEEK_END);
    int length = ftell(objfile);
    printf("%d\n", length);
    char* sss = malloc(length + 2);
    fseek(objfile, 0, SEEK_SET);
    fread(sss, 1, length + 1, objfile);
    sss[length + 1] = 0;
    int current = 0;
    while(sss[current] != 0){
        printf("LINE START: ");
        while(sss[current] != '\n'){
            printf("%c", sss[current]);
            current++;
        }
        printf("\n");
        current++;
    }
}

typedef struct BoundingBox{
    double low_x;
    double high_x;
    double low_y;
    double high_y;
    double low_z;
    double high_z;    
} BoundingBox;

typedef struct DOUBLE_COLOR{
    double r;
    double g;
    double b;
    double a;
} DOUBLE_COLOR;

typedef struct RGB{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGB;
typedef struct Texture{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Texture;

typedef struct Vector{
    double x;
    double y;
    double z;
} Vector;

void rotateVector(Vector* a1, Vector* a2);
RGB *CAMERA_COL = 0;
Vector *CAMERA_DIR = 0;
Vector *CAMERA_UP = 0;
Vector *CAMERA_SIDE = 0;
Vector *CAMERA_POS = 0;
void (*CAMERA_TEXTURE)() = 0;

typedef struct Light{
    Vector position;
    RGB color;
    struct Light* next;
} Light;

Light* lightChain = 0;

/*

   ---SHAPE TYPES---
    0: point-defined
    1: math-defined
    2: Sphere
*/
typedef struct Shape{
    BoundingBox bounds;
    Vector position;
    Vector orientation;
    Vector scale;
    Texture color;
    double (*mii)();
    void (*texture)();
    int type;
    struct Shape* next;
} Shape;

Shape* shapeStart = 0;


//SPECIAL TRIGGERS

void setTexture(Shape* current, void* func){
    printf("Setting texture!\n");
    if(current == 0){
        printf("SETTING CAMERA TEX\n");
        CAMERA_TEXTURE = func;
    } else {
        (current -> texture) = func;
    }
}

void map(DOUBLE_COLOR* dc, char* name, double x, double y){
    ImageHolder *ihp = textures;
    while(strcmp(ihp -> name, name) != 0){
        ihp = ihp -> next;
    }
    ImageHolder ih = ihp[0];
    int px = ((int)(x * ih.width)) % ih.width;
    if(px < 0){
        px = ih.width + px;
    }
    int py = ((int)(y * ih.height)) % ih.height;
    if(py < 0){
        py = ih.height + py;
    }
    //printf("%d/%d %d/%d\n", px, ih.width, py, ih.height);
    int mode = ih.mode;
    int scale = (mode == PNG_COLOR_TYPE_RGB_ALPHA) ? 4 : 3;
    (dc -> r) = ih.imageData[py][scale * px];
    (dc -> g) = ih.imageData[py][scale * px + 1];
    (dc -> b) = ih.imageData[py][scale * px + 2];
    (dc -> a) = (mode == PNG_COLOR_TYPE_RGB_ALPHA) ? ih.imageData[py][scale * px + 3] : 255;
    //printf("END MAP\n");
}
//Attribute setter functions
void set_x(Shape* current, double am){
    printf("Setting x to %f\n", am);
    if(current == 0){
        printf("Setting CAMERA_POS X to %f\n", am);
        CAMERA_POS -> x = am;
        return;
    }
    (current -> position).x = am;
}

void set_y(Shape* current, double am){
    printf("Setting y to %f\n", am);
    if(current == 0){
        printf("Setting CAMERA_POS Y to %f\n", am);
        CAMERA_POS -> y = am;
        return;
    }
    (current -> position).y = am;
}

void set_z(Shape* current, double am){
    printf("Setting z to %f\n", am);
    if(current == 0){
        printf("Setting CAMERA_POS Z to %f\n", am);
        CAMERA_POS -> z = am;
        return;
    }
    (current -> position).z = am;
}

void set_r(Shape* current, double am){
    if(current == 0){
        CAMERA_COL -> r = am;
        return;
    }
    (current -> color).r = am;
}

void set_g(Shape* current, double am){
    if(current == 0){
        CAMERA_COL -> g = am;
        return;
    }
    (current -> color).g = am;
}

void set_b(Shape* current, double am){
    if(current == 0){
        CAMERA_COL -> b = am;
        return;
    }
    (current -> color).b = am;
}

void set_sx(Shape* current, double am){
    printf("Setting sx to %f\n", am);
    (current -> scale).x = am;
}

void set_sy(Shape* current, double am){
    printf("Setting sy to %f\n", am);
    (current -> scale).y = am;
}

void set_sz(Shape* current, double am){
    printf("Setting sz to %f\n", am);
    (current -> scale).z = am;
}

void set_ox(Shape* current, double am){
    printf("Setting ox to %f\n", am);
    (current -> orientation).x = am;
}

void set_oy(Shape* current, double am){
    printf("Setting oy to %f\n", am);
    (current -> orientation).y = am;
}

void set_oz(Shape* current, double am){
    printf("Setting oz to %f\n", am);
    (current -> orientation).z = am;
}

void set_light_x(Light* current, double am){
    (current -> position).x = am;
}

void set_light_y(Light* current, double am){
    (current -> position).y = am;
}

void set_light_z(Light* current, double am){
    (current -> position).z = am;
}

void set_light_r(Light* current, double am){
    (current -> color).r = am;
}

void set_light_g(Light* current, double am){
    (current -> color).g = am;
}

void set_light_b(Light* current, double am){
    (current -> color).b = am;
}
//End Attribute setter functions

void addShape(Shape* toAdd){
    (toAdd -> orientation).x = 0;
    (toAdd -> orientation).y = 0;
    (toAdd -> orientation).z = 0;
    (toAdd -> scale).x = 1;
    (toAdd -> scale).y = 1;
    (toAdd -> scale).z = 1;
    (toAdd -> next) = shapeStart;
    (toAdd -> texture) = 0;
    shapeStart = toAdd;
    printf("Added shape with type: %d\n", toAdd -> type);
}

void addLight(Light* toAdd){
    (toAdd -> next) = lightChain;
    lightChain = toAdd;
    printf("Added light\n");
}

Shape* createSphere(){
    Shape* newSphere = malloc(sizeof(Shape));
    (newSphere -> position).x = 0;
    (newSphere -> position).y = 0;
    (newSphere -> position).z = 0;
    (newSphere -> color).r = 255;
    (newSphere -> color).g = 255;
    (newSphere -> color).b = 255;
    (newSphere -> color).a = 255;
    (newSphere -> type) = SPHERE;
    addShape(newSphere);
    return newSphere;
}

Shape* createMath(double (*p)()){
    Shape* newMath = malloc(sizeof(Shape));
    (newMath -> position).x = 0;
    (newMath -> position).y = 0;
    (newMath -> position).z = 0;
    (newMath -> color).r = 255;
    (newMath -> color).a = 255;
    (newMath -> type) = MATH;
    (newMath -> mii) = p;
    addShape(newMath);
    return newMath;
}

Light* createLight(){
    Light* newLight = malloc(sizeof(Light));
    (newLight -> position).x = 0;
    (newLight -> position).y = 0;
    (newLight -> position).z = 0;
    (newLight -> color).r = 255;
    (newLight -> color).g = 255;
    (newLight -> color).b = 255;
    addLight(newLight);
    return newLight;
}
double getMagnitude(Vector *v){
        return sqrt((v -> x) * (v -> x) + (v -> y) * (v -> y) + (v -> z) * (v -> z));
}
Vector correct(Shape s, double x, double y, double z){
    Vector v = {(x - s.position.x) / (s.scale.x), (y - s.position.y) / s.scale.y, (z - s.position.z) / s.scale.z};
    rotateVector(&v, &(s.orientation));
    return v;
}
int isInside(Shape s, double x, double y, double z){
    Vector v = correct(s, x, y, z);
    x = v.x;
    y = v.y;
    z = v.z;
    if((s.type) == SPHERE){
        Vector dist;
        dist.x = x;
        dist.y = y;
        dist.z = z;
        return (getMagnitude(&dist) < 1);
    } else if (s.type == MATH) {
        return (s.mii)(x, y, z);        
    }
}

int isInAnyShape(double x, double y, double z){
    Shape* current = shapeStart;
    while(current != 0){
        if(isInside(current[0], x, y, z)){
            return 1;
        }
        current = current -> next;
    }
    return 0;
}

void scaleVector(Vector *v, double newMagnitude){
    double magnitude = getMagnitude(v);
    v -> x = newMagnitude * (v -> x) / magnitude;
    v -> y = newMagnitude * (v -> y) / magnitude;
    v -> z = newMagnitude * (v -> z) / magnitude;
}

void normalizeVector(Vector *v){
    scaleVector(v, 1);
}

void rotateVector(Vector *v, Vector *rots){
    if((rots -> x == 0) && (rots -> y == 0) && (rots -> z == 0)){
        return;
    }
    double sx = v -> x;
    double sy = v -> y;
    double sz = v -> z;
    v -> x = (sx * cos(rots -> z) - sy * sin(rots -> z));
    v -> y = (sy * cos(rots -> z) + sx * sin(rots -> z));   
    sx = v -> x;
    sy = v -> y;
    v -> x = (sx * cos(rots -> y) - sz * sin(rots -> y));
    v -> z = (sz * cos(rots -> y) + sx * sin(rots -> y));
    sx = v -> x;
    sz = v -> z;
    v -> y = (sy * cos(rots -> x) - sz * sin(rots -> x));
    v -> z = (sz * cos(rots -> x) + sy * sin(rots -> x)); 
}

void rotateCamera(double rx, double ry, double rz){
    Vector rots;
    rots.x = rx;
    rots.y = ry;
    rots.z = rz;
    rotateVector(CAMERA_DIR, &rots);
    rotateVector(CAMERA_UP, &rots);
    rotateVector(CAMERA_SIDE, &rots);
}

//Don't store outside of stack pls me
Vector addVectors(Vector v1, Vector v2){
    
}

Vector getLightHitPercentage(Vector startPos){
    Vector maxPercent;
    maxPercent.x = 0;
    maxPercent.y = 0;
    maxPercent.z = 0;
    Light* currentLight = lightChain;
    while(currentLight != 0){
        Vector diff;
        diff.x = (currentLight -> position).x - startPos.x;
        diff.y = (currentLight -> position).y - startPos.y;
        diff.z = (currentLight -> position).z - startPos.z;
        double distance = getMagnitude(&diff);
        double resolution = distance / 1000;
        scaleVector(&diff, CAMERA_RES);
        Vector currentPos;
        currentPos.x = startPos.x + diff.x; //Don't start inside the shape since there will be a collision.
        currentPos.y = startPos.y + diff.y;
        currentPos.z = startPos.z + diff.z;
        scaleVector(&diff, resolution);
        double percent = 1.0;
        double redStrength = (currentLight -> color).r / 256.0;
        double greenStrength = (currentLight -> color).g / 256.0;
        double blueStrength = (currentLight -> color).b / 256.0;
        for(int am = 0; am < 1000; am++){
            if(isInAnyShape(currentPos.x, currentPos.y, currentPos.z)){
                percent = percent - (resolution / .12);
            }
            percent -= (resolution / 120);
            currentPos.x += diff.x;
            currentPos.y += diff.y;
            currentPos.z += diff.z;
        }
        if(percent * redStrength > maxPercent.x){
            maxPercent.x = percent * redStrength;
        }
        if(percent * greenStrength > maxPercent.y){
            maxPercent.y = percent * greenStrength;
        }
        if(percent * blueStrength > maxPercent.z){
            maxPercent.z = percent * blueStrength;
        }
        currentLight = (currentLight -> next);
    }
    return maxPercent;
}

void setPixelColor(int x, int y, int r, int g, int b, int a){
    rows[y][4 * x] = r;
    rows[y][4 * x + 1] = g;
    rows[y][4 * x + 2] = b;
    rows[y][4 * x + 3] = a;
}
void renderPixel(int x, int y){
    double xness = ((x - (RENDER_WIDTH / 2.0)) / RENDER_WIDTH);
    double yness = (((RENDER_HEIGHT / 2.0) - y) / RENDER_HEIGHT) * RENDER_RATIO;
    Vector currentDir;
    Vector currentPos;

    currentPos.x = CAMERA_POS -> x;
    currentPos.y = CAMERA_POS -> y;
    currentPos.z = CAMERA_POS -> z;

    currentDir.x = (CAMERA_DIR -> x) + (CAMERA_SIDE -> x) * xness + (CAMERA_UP -> x) * yness;
    currentDir.y = (CAMERA_DIR -> y) + (CAMERA_SIDE -> y) * xness + (CAMERA_UP -> y) * yness;
    currentDir.z = (CAMERA_DIR -> z) + (CAMERA_SIDE -> z) * xness + (CAMERA_UP -> z) * yness;

    scaleVector(&currentDir, CAMERA_RES);
    
    int currentSteps = 0;
    DOUBLE_COLOR dc = {CAMERA_COL -> r, CAMERA_COL -> g, CAMERA_COL -> b, 255};
    if(CAMERA_TEXTURE != 0){
        CAMERA_TEXTURE((x + 0.0) / RENDER_WIDTH, (y + 0.0) / RENDER_HEIGHT, 0.0, &dc);
    }
    setPixelColor(x, y, dc.r, dc.g, dc.b, dc.a);
    while(currentSteps < MAX_STEPS) {
        
        //Go through shapes and see if inside
        Shape *currentShape = shapeStart;
        while(currentShape != 0){
            if(isInside(currentShape[0], currentPos.x, currentPos.y, currentPos.z)){

                //correct our point
                Vector pointIn = {currentPos.x, currentPos.y, currentPos.z};
                Vector pointOut = {currentPos.x - currentDir.x, currentPos.y - currentDir.y, currentPos.z - currentDir.z};
                Vector midpoint = {(pointIn.x + pointOut.x) / 2, (pointIn.y + pointOut.y) / 2, (pointIn.z + pointOut.z) / 2};
                for(int i = 0; i < 100; i++){
                    if(isInside(currentShape[0], midpoint.x, midpoint.y, midpoint.z)){
                        pointIn = midpoint;
                    } else {
                        pointOut = midpoint;
                    }
                    midpoint.x = (pointIn.x + pointOut.x) / 2;
                    midpoint.y = (pointIn.y + pointOut.y) / 2;
                    midpoint.z = (pointIn.z + pointOut.z) / 2;
                }
                currentPos = midpoint;
                Vector lightHitPercentage = getLightHitPercentage(currentPos); //x=r, y=g, z=b
                //printf("%d, %f, %f\n", (currentShape -> color).g, lightHitPercentage, (((currentShape -> color).g) * lightHitPercentage));
                DOUBLE_COLOR dc = {(currentShape -> color).r, (currentShape -> color).g, (currentShape -> color).b, (currentShape -> color).a};
                Vector corrected = correct(currentShape[0], currentPos.x, currentPos.y, currentPos.z);
                if((currentShape -> texture) != 0){
                    (currentShape -> texture)(corrected.x, corrected.y, corrected.z, &dc);
                }
                dc.r = ((int)dc.r) % 256;
                dc.g = ((int)dc.g) % 256;
                dc.b = ((int)dc.b) % 256;
                dc.a = ((int)dc.a) % 256;
                setPixelColor(x, y, (int)(dc.r * lightHitPercentage.x), (int)(dc.g * lightHitPercentage.y), (int)(dc.b * lightHitPercentage.z), dc.a);
                return;
            }
            currentShape = currentShape -> next;
        }

        currentPos.x += currentDir.x;
        currentPos.y += currentDir.y;
        currentPos.z += currentDir.z;
        currentSteps++;
    }

    //printf("[%d, %d] with direction vector [%f, %f, %f]\n", x, y, currentDir.x, currentDir.y, currentDir.z);
}

int threadRender(){
    int rowToRender = getRowNum();
    while(rowToRender < RENDER_HEIGHT){
        for(int x = 0; x < RENDER_WIDTH; x++){
            renderPixel(x, rowToRender);
        }
        rowToRender = getRowNum();
    }
}

void renderSegment(void* segmentP){
    int segment = ((int*)segmentP)[0];
    printf("RENDERING SEGMENT %d\n", segment);
    //TODO account for heights not div by 4
    for(int y = segment * (RENDER_HEIGHT / SEGMENTS); y < (segment + 1) * (RENDER_HEIGHT / SEGMENTS); y++){
        for(int x = 0; x < RENDER_WIDTH; x++){
            renderPixel(x, y);
        }
    }
    printf("FINISHED RENDERING SEGMENT %d\n", segment);
}
void startup(){
    stages = malloc(sizeof(int) * SEGMENTS);
    for(int i = 0; i < SEGMENTS; i++){
        stages[i] = i;
    }
    /*
    lightChain = malloc(sizeof(Light));
    (lightChain -> position).x = 2;
    (lightChain -> position).y = -10;
    (lightChain -> position).z = 10;
    (lightChain -> color).r = 255;
    (lightChain -> color).g = 255;
    (lightChain -> color).b = 200;
     Light* lightSecond = malloc(sizeof(Light));
    (lightChain -> next) = lightSecond;
    (lightSecond -> position).x = -2;
    (lightSecond -> position).y = -10;
    (lightSecond -> position).z = 10;
    (lightSecond -> color).r = 200;
    (lightSecond -> color).g = 200;
    (lightSecond -> color).b = 255;
    lightSecond -> next = 0;
    */
    /*
    Shape* second = malloc(sizeof(Shape));
    shapeStart = malloc(sizeof(Shape));
    shapeStart -> type = 2;
    (shapeStart -> scale).x = 1;
    (shapeStart -> scale).y = 1;
    (shapeStart -> scale).z = 1;
    (shapeStart -> position).x = 0;
    (shapeStart -> position).y = 1;
    (shapeStart -> position).z = 0;
    (shapeStart -> color).r = 255;
    (shapeStart -> color).g = 255;
    (shapeStart -> color).b = 255;
    (shapeStart -> color).a = 255;
    (shapeStart -> next) = second;
    second -> type = 2;
    (second -> scale).x = .7;
    (second -> scale).y = .7;
    (second -> scale).z = .7;
    (second -> position).x = 0;
    (second -> position).y = 1;
    (second -> position).z = 1.45;
    (second -> color).r = 255;
    (second -> color).g = 255;
    (second -> color).b = 255;
    (second -> color).a = 255;
    (second -> next) = 0;
    */

    RENDER_RATIO = ((double)RENDER_HEIGHT) / RENDER_WIDTH;

    CAMERA_COL = malloc(sizeof(RGB));
    CAMERA_COL -> r = 0;
    CAMERA_COL -> g = 0;
    CAMERA_COL -> b = 0;

    CAMERA_DIR = malloc(sizeof(Vector));
    CAMERA_DIR -> x = 0;
    CAMERA_DIR -> y = 1;
    CAMERA_DIR -> z = 0;

    CAMERA_UP = malloc(sizeof(Vector));
    CAMERA_UP -> x = 0;
    CAMERA_UP -> y = 0;
    CAMERA_UP -> z = 1;

    CAMERA_SIDE = malloc(sizeof(Vector));
    CAMERA_SIDE -> x = 1;
    CAMERA_SIDE -> y = 0;
    CAMERA_SIDE -> z = 0;

    CAMERA_POS = malloc(sizeof(Vector));
    CAMERA_POS -> x = 0;
    CAMERA_POS -> y = -10;
    CAMERA_POS -> z = 0;
}
void render(uint64_t time){
    printf("TIME DETECTED AS %lu\n", time);
    char* filename = malloc(80);
    sprintf(filename, "render-%s-%lu.png", FILENAME, time);
    FILE* fp = fopen(filename, "wb");
    normalizeVector(CAMERA_DIR);
    printf("CAMERA DIR: %f, %f, %f\n", CAMERA_DIR->x, CAMERA_DIR->y, CAMERA_DIR->z);
    rotateCamera(0, 0, 0);
    printf("CAMERA DIR: %f, %f, %f\n", CAMERA_DIR->x, CAMERA_DIR->y, CAMERA_DIR->z);
    png_structp png_structure = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_structure){
        printf("Life isn't good\n");
    }
    png_infop png_info = png_create_info_struct(png_structure);
    if(!png_info){
        printf("Life is still not good\n");
    }
    png_init_io(png_structure, fp);
    rows = calloc(RENDER_HEIGHT, sizeof(png_bytep));
    for(int i = 0; i < RENDER_HEIGHT; i++){
        rows[i] = malloc(4 * RENDER_WIDTH);
    }
    //pthread_t *thr0 = malloc(sizeof(pthread_t));
    pthread_t* renderThreads[SEGMENTS - 1];
    //pthread_t *thr1 = malloc(sizeof(pthread_t));
    //pthread_t *thr2 = malloc(sizeof(pthread_t));
    //pthread_t *thr3 = malloc(sizeof(pthread_t));
    for(int i = 1; i < SEGMENTS; i++){
        renderThreads[i - 1] = malloc(sizeof(pthread_t));
        pthread_create(renderThreads[i - 1], NULL, (void*)threadRender, NULL);
    }
    //pthread_create(thr2, NULL, (void*)renderSegment, &(stages[2]));
    //pthread_create(thr3, NULL, (void*)renderSegment, &(stages[3]));
    //renderSegment(&(stages[0]));
    threadRender();
    //pthread_join(thr1[0], NULL);
    //pthread_join(thr2[0], NULL);
    //pthread_join(thr3[0], NULL);
    for(int i = 0; i < SEGMENTS - 1; i++){
        pthread_join(renderThreads[i][0], NULL);
    }
    Vector *v = malloc(sizeof(Vector));
    (v -> x) = 2;
    (v -> y) = 2;
    (v -> z) = 2;
    normalizeVector(v);
    png_set_IHDR(png_structure, png_info, RENDER_WIDTH, RENDER_HEIGHT, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_set_rows(png_structure, png_info, rows);
    png_write_png(png_structure, png_info, 0, NULL);
    png_write_end(png_structure, png_info);
    fclose(fp);
}
