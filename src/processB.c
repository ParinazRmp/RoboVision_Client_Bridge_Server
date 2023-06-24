#include "./../include/processB_utilities.h"
#include <bmpfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

const int SIZE = 1600*600*sizeof(int);
bmpfile_t *bmp;
int center_x;
int center_y;
const char* name = "SharedMemory";
int shm_fd;
int* ptr;
void load_shared_memory(void);
void find_center(void);

int main(int argc, char const *argv[])
{
    sleep(1);
    /* open the shared memory object */
    shm_fd = shm_open(name, O_RDONLY, 0666);
    /* memory map the shared memory object */
    ptr = mmap(0, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    // Utility variable to avoid trigger resize event on launch
    int first_resize = TRUE;
    // Initialize UI
    init_console_ui();

    // Infinite loop
    while (TRUE) {
         // Get input in non-blocking mode
        int cmd = getch();
        // If user resizes screen, re-draw UI...
        if(cmd == KEY_RESIZE) {
            if(first_resize) {
                first_resize = FALSE;
            }
            else {
                reset_console_ui();
            }
        }
        else {

            load_shared_memory();
            find_center();
            bmp_destroy(bmp);


        }
    }

    endwin();
    return 0;
}


void find_center() {
    int y_min = 600;
    int y_max = 0;
    int x_min = 1600;
    int x_max = 0;
    int center_x;
    int center_y;

    for (int x = 0; x < 1600; x++) {
        for (int y = 0; y < 600; y++) {
            rgb_pixel_t *pixel = bmp_get_pixel(bmp, x, y);
            if (pixel->green == 90) {
                if (x < x_min) {
                    x_min = x;
                }
                if (x > x_max) {
                    x_max = x;
                }
                if (y < y_min) {
                    y_min = y;
                }
                if (y > y_max) {
                    y_max = y;
                }
            }
        }
    }

    center_x = (x_min + x_max) / 2;
    center_y = (y_min + y_max) / 2;

    center_x = center_x / (1600 / COLS);
    center_y = center_y / (600 / LINES);

    mvprintw(LINES - 1, 1, "center_x: %d  ", center_x);
    mvprintw(LINES - 2, 1, "center_y: %d  ", center_y);

    mvaddch(center_y, center_x, '0');
    refresh();
    refresh();
}



void load_shared_memory() {
    bmp = bmp_create(1600, 600, 4);

    for (int x = 0; x < 1600; x++) {
        for (int y = 0; y < 600; y++) {
            if (ptr[x * 599 + y] == 90) {
                rgb_pixel_t pixel = {0, ptr[x * 599 + y], 0, 0};
                bmp_set_pixel(bmp, x, y, pixel);
            } else {
                rgb_pixel_t pixel = {255, 255, 255, 0};
                bmp_set_pixel(bmp, x, y, pixel);
            }
        }
    }
}
