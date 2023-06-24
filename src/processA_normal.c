/**
 * @file ProcessA_normal.cpp
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
#include <signal.h>

const int SIZE = 1600*600*sizeof(int); /**< Size of the shared memory */
bmpfile_t *bmp; /**< Pointer to the BMP file */
rgb_pixel_t pixel = {0, 90, 0, 0}; /**< RGB pixel value */
const char* name = "SharedMemory"; /**< Name of the shared memory */
int shm_fd; /**< File descriptor for the shared memory */
int* ptr; /**< Pointer to the shared memory */
bool print_flag = false; /**< Flag for printing */
int width = 1600; /**< Width of the image */
int height = 600; /**< Height of the image */
int depth = 4; /**< Depth of the image */

/**
 * @brief Creates a bitmap with a circle.
 * @param circle_x The x-coordinate of the circle center.
 * @param circle_y The y-coordinate of the circle center.
 */
void bitmap_creat(int circle_x, int circle_y);

/**
 * @brief Updates the shared memory with the bitmap.
 * @param bmp Pointer to the BMP file.
 */
void update_shared_memory(bmpfile_t *bmp);

/**
 * @brief Signal handler for the SIGKILL signal.
 * @param signo The signal number.
 */
void sig_killhandler(int signo);

  
int main(int argc, char *argv[])
{

	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, SIZE);
	ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    int first_resize = TRUE; /**< Flag for the first resize event */
    init_console_ui(); /**< Initialize the console UI */
    bitmap_creat(circle.x, circle.y); /**< Create the initial bitmap with a circle */
    signal(SIGINT, sig_killhandler); /**< Register the signal handler for the SIGINT signal */
    /**
     * @brief Main loop for handling user input and updating the display.
     */
    while (TRUE)
    {
        int cmd = getch();
        switch (cmd) {
            case KEY_RESIZE:
                if (first_resize) {
                    first_resize = FALSE;
                } else {
                    reset_console_ui();
                }
                break;

            case KEY_MOUSE: {
                MEVENT event;
                if (getmouse(&event) == OK) {
                    if (check_button_pressed(print_btn, &event)) {
                        print_flag = true;
                        bitmap_creat(circle.x, circle.y);
                        mvprintw(LINES - 1, 1, "Print button pressed");
                        refresh();
                        sleep(1);
                        for (int j = 0; j < COLS - BTN_SIZE_X - 2; j++) {
                            mvaddch(LINES - 1, j, ' ');
                        }
                    }
                }
                break;
            }

            case KEY_LEFT:
            case KEY_RIGHT:
            case KEY_UP:
            case KEY_DOWN:
                move_circle(cmd);
                draw_circle();
                bitmap_creat(circle.x, circle.y);
                break;
        }

    }
    
    endwin();
    return 0;
}


void update_shared_memory( bmpfile_t *bmp)
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


void bitmap_creat(int circle_x, int circle_y)
{
    bmp = bmp_create(width, height, depth);
    int radius =20;
    rgb_pixel_t pixel = {0, 90, 0, 0};
     for(int x = -radius; x <= radius; x++) 
     {
      for(int y = -radius; y <= radius; y++) 
      {
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
  update_shared_memory( bmp);
  bmp_destroy(bmp);
}


void sig_killhandler(int signo){
    if (signo == SIGINT){
        printf("I Received SIGINT Signal!\n");

    }
}