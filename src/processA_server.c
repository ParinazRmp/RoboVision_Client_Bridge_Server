/**
 * @file ProcessA_server.cpp
 * @brief This is the main file of the project.
 */

#include "./../include/processA_utilities.h"
#include <bmpfile.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>


bmpfile_t *bmp; /**< Pointer to the BMP file */
rgb_pixel_t pixel = {0, 90, 0, 0}; /**< RGB pixel value */
const int SIZE = 1600*600*sizeof(int); /**< Size of the shared memory */
const char* name = "SharedMemory"; /**< Name of the shared memory */
int shm_fd; /**< File descriptor for the shared memory */
int* ptr; /**< Pointer to the shared memory */
bool print_flag = false; /**< Flag for printing */
bool first = true; /**< Flag for first iteration */
int cmd; /**< Command variable */
int width = 1600; /**< Width of the image */
int height = 600; /**< Height of the image */
int depth = 4; /**< Depth of the image */

/**
 * @brief Creates a circle bitmap.
 * @param circle_x The x-coordinate of the circle center.
 * @param circle_y The y-coordinate of the circle center.
 */
void createCircleBitmap(int circle_x, int circle_y);
/**
 * @brief Updates the shared memory with the bitmap.
 * @param bmp Pointer to the BMP file.
 */
void updateSharedMemoryWithBitmap(bmpfile_t *bmp);
/**
 * @brief Error handler function.
 * @param msg The error message to display.
 */
void Error_Handler(char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int first_resize = TRUE;
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0); /**< Create a socket */
    if (sockfd < 0) 
        Error_Handler("ERROR opening socket"); /**< Error handling for socket creation */
    bzero((char *) &serv_addr, sizeof(serv_addr)); /**< Clear server address structure */
    portno = atoi(argv[1]); /**< Get port number from command line argument */

    serv_addr.sin_family = AF_INET; /**< Set address family */
    serv_addr.sin_addr.s_addr = INADDR_ANY; /**< Bind to any available address */
    serv_addr.sin_port = htons(portno); /**< Set the port number */

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        Error_Handler("ERROR on binding"); /**< Error handling for binding the socket */

    listen(sockfd,5); /**< Listen for incoming connections */
    clilen = sizeof(cli_addr); /**< Get the size of the client address structure */
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); /**< Accept a new connection */

    if (newsockfd < 0) 
        Error_Handler("ERROR on accept"); /**< Error handling for accepting the connection */

    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); /**< Open or create shared memory */
    ftruncate(shm_fd, SIZE); /**< Set the size of the shared memory object */
    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0); /**< Map the shared memory object to the process's address space */


    init_console_ui();
    createCircleBitmap(circle.x, circle.y);

    while (true) {
    if (first) {
        cmd = getch();
        first = false;
    }

    bzero(buffer, 10);
    n = read(newsockfd, buffer, 10);
    if (n < 0) {
        Error_Handler("ERROR reading from socket");
    }

    if (!strcmp(buffer, "p")) {
        print_flag = true;
        createCircleBitmap(circle.x, circle.y);
        mvprintw(LINES - 1, 1, "Print button pressed");
        refresh();
        sleep(1);
        for (int j = 0; j < COLS - BTN_SIZE_X - 2; j++) {
            mvaddch(LINES - 1, j, ' ');
        }
    } else {
        cmd = atoi(buffer);
    }

    refresh();

    switch (cmd) {
        case KEY_RESIZE:
            if (first_resize) {
                first_resize = false;
            } else {
                reset_console_ui();
            }
            break;
            
        case KEY_MOUSE:
            if (getmouse(&event) == OK) {
                if (check_button_pressed(print_btn, &event)) {
                    print_flag = true;
                    createCircleBitmap(circle.x, circle.y);
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
            move_circle(cmd);
            draw_circle();
            createCircleBitmap(circle.x, circle.y);
            break;
            
        default:
            // handle other cases or do nothing
            break;
    }

    }

    endwin();
    return 0;
}

/**
 * @brief Updates the shared memory with the bitmap.
 * @param bmp Pointer to the BMP file.
 */
void updateSharedMemoryWithBitmap( bmpfile_t *bmp)
{
    for(int x = 0; x < 1600; x++)
    {
        for(int y = 0; y < 600; y++)
        {
         rgb_pixel_t *pixel=bmp_get_pixel(bmp ,x, y);
         ptr[x*599+y]= pixel->green;

        }
    }


}
/**
 * @brief Creates a circle bitmap.
 * @param circle_x The x-coordinate of the circle center.
 * @param circle_y The y-coordinate of the circle center.
 */
void createCircleBitmap(int circle_x, int circle_y)
{
    bmp = bmp_create(width, height, depth);
    int radius =20;
    rgb_pixel_t pixel = {0, 90, 0, 0};
     for(int x = -radius; x <= radius; x++) 
     {
      for(int y = -radius; y <= radius; y++) 
      {
       // If distance is smaller, point is within the circle
         if(sqrt(x*x + y*y) < radius) {
          bmp_set_pixel(bmp, (circle_x)*(width/(COLS - BTN_SIZE_X -2)) + x, (circle_y)*(height/LINES) + y, pixel);
         }
       }
      }
if(print_flag == true)
 {
     bmp_save(bmp,"screen" );
  print_flag = false;
 }
  updateSharedMemoryWithBitmap( bmp);
  bmp_destroy(bmp);
}