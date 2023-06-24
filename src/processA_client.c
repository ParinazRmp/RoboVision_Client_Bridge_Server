/**
 * @file ProcessA_client.cpp
 * @brief This is the main file of the project.
 */

#include "./../include/processA_utilities.h"
#include <bmpfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <strings.h>
#include <unistd.h>

const int SIZE = 1600 * 600 * sizeof(int); /**< The size (in bytes) of the shared memory object */

bmpfile_t *bmp; /**< Pointer to the BMP file */
rgb_pixel_t pixel = {0, 90, 0, 0}; /**< RGB pixel value */
const char* name = "SharedMemory"; /**< Name of the shared memory object */
int shm_fd; /**< Shared memory file descriptor */
int* ptr; /**< Pointer to the shared memory object */
bool print_flag = false; /**< Flag for printing */
int width = 1600; /**< Width of the image */
int height = 600; /**< Height of the image */
int depth = 4; /**< Depth of the image */

/**
 * @brief Creates a bitmap.
 * @param circle_x The x-coordinate of the circle center.
 * @param circle_y The y-coordinate of the circle center.
 */
void bitmap_create(int circle_x, int circle_y);

/**
 * @brief Updates the shared memory with the bitmap.
 * @param bmp Pointer to the BMP file.
 */
void update_shared_memory(bmpfile_t *bmp);

/**
 * @brief Displays an error message and exits the program.
 * @param msg The error message to display.
 */
void error(const char *msg);

//-----------------------------------------Main ----------------------
int main(int argc, char *argv[])
{
    int first_resize = TRUE;  // Utility variable to avoid triggering resize event on launch
    int sockfd, portno, n;
    char *token;
    token = strtok(argv[1], ":");    /* Extracting the current x position */
    portno = atoi(strtok(NULL, ":"));    /* Extracting the current z position */
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    server = gethostbyname(token);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");


    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);    /* create the shared memory object */
    ftruncate(shm_fd, SIZE);    /* configure the size of the shared memory object */
    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);    /* memory map the shared memory object */
    init_console_ui();  // Initialize UI
    bitmap_create(circle.x, circle.y);

    while (TRUE) {
    int cmd = getch();  // Get input in non-blocking mode
     char buffer[10];   
    switch (cmd) {
        case KEY_RESIZE:
            if (first_resize) {
                first_resize = FALSE;
            } else {
                reset_console_ui();
            }
            break;
        
        case KEY_MOUSE:
            if (getmouse(&event) == OK) {
                if (check_button_pressed(print_btn, &event)) {
                    char buffer[10];
                    bzero(buffer, 10);
                    strcpy(buffer, "p");
                    n = write(sockfd, buffer, strlen(buffer));
                    if (n < 0) 
                        perror("write");

                    print_flag = true;
                    bitmap_create(circle.x, circle.y);
                    mvprintw(LINES - 1, 1, "Print button pressed");
                    refresh();
                    sleep(1);
                    for (int j = 0; j < COLS - BTN_SIZE_X - 2; j++) {
                        mvaddch(LINES - 1, j, ' ');
                    }
                }
            }
            break;
        
        case KEY_LEFT:
        case KEY_RIGHT:
        case KEY_UP:
        case KEY_DOWN:
            
            bzero(buffer, 10);
            sprintf(buffer, "%d", cmd);
            n = write(sockfd, buffer, strlen(buffer));
            if (n < 0) {
                mvprintw(LINES - 1, 1, "ERROR writing to socket");
                sleep(.5);
                mvprintw(LINES - 1, 1, "                            ");
            }

            move_circle(cmd);  // If input is an arrow key, move circle accordingly
            draw_circle();
            bitmap_create(circle.x, circle.y);
            break;
        }

    }
    
    endwin();
    return 0;
}

void update_shared_memory(bmpfile_t *bmp)
{
    for (int x = 0; x < 1600; x++) {
        for (int y = 0; y < 600; y++) {
            rgb_pixel_t *pixel = bmp_get_pixel(bmp, x, y);
            ptr[x * 599 + y] = pixel->green;
        }
    }
}

void bitmap_create(int circle_x, int circle_y)
{
    bmp = bmp_create(width, height, depth);
    int radius = 20;
    rgb_pixel_t pixel = {0, 90, 0, 0};
    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            if (sqrt(x * x + y * y) < radius) {
                bmp_set_pixel(bmp, (circle_x) * (width / (COLS - BTN_SIZE_X - 2)) + x, (circle_y) * (height / LINES) + y, pixel);
            }
        }
    }
    
    if (print_flag) {
        bmp_save(bmp, "screen");
        print_flag = false;
    }
    
    update_shared_memory(bmp);
    bmp_destroy(bmp);
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
