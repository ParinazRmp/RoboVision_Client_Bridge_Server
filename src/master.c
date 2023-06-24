/**master.c
 * ARP 2022-2023 - Third ASSIGNMENT
 * @brief This file contains a program that creates child processes based on user input.
 * The program shows a menu to the user and the user can select a mode of operation.
 * The selected mode will determine the program and arguments to be used when spawning child processes.
 * @authors Parinaz Ramezanpour, Danial Sabzevari
 * @version 1.0
 * @date June 2023
 */

// Include necessary headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

// Define constants for paths to executable files and maximum lengths for port and IP
#define PROCESS_A_NORMAL "./bin/processA_normal"
#define PROCESS_A_SERVER "./bin/processA_server"
#define PROCESS_A_CLIENT "./bin/processA_client"
#define PROCESS_B "./bin/processB"
#define KONSOLE "/usr/bin/konsole"
#define MAX_PORT_LEN 10
#define MAX_IP_LEN 20

// Declare global variables
pid_t pid_A;
pid_t pid_B;
char server_params[50]; // Will store either port or ip:port based on the mode selected by user
bool show_menu_enabled = true; // Flag to control showing of menu
char process[50]; // Will store path to executable based on the mode selected by user

/**
 * @brief Spawns a child process.
 * @param program The program to execute in the child process.
 * @param arg_list The argument list to pass to the program.
 * @return The PID of the child process, or exits on error.
 * This function first forks the current process to create a child.
 * The child process then tries to replace its image with the provided program by calling execvp.
 * If the child process fails to replace its image, it prints an error message and exits with failure status.
 * The parent process simply returns the PID of the child process.
 */
int spawn(const char *program, char *arg_list[]) {
  pid_t child_pid = fork();

  if(child_pid < 0) {
    perror("Error while forking"); // Print error if fork fails
    exit(EXIT_FAILURE);
  } else if(child_pid != 0) {
    return child_pid; // If in parent process, return PID of child
  } else {
    execvp(program, arg_list); // In child process, replace image with provided program
    perror("Exec failed"); // If execvp returns, it means there was an error
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief Handles SIGINT signals, killing child processes and enabling the menu.
 * @param signo The signal number.
 * This function is the signal handler for SIGINT signals.
 * When a SIGINT signal is received, it prints a message, kills child processes, and enables the menu.
 */
void sig_int_handler(int signo) {
  if (signo == SIGINT) {
    printf("Received SIGINT signal\n"); // Print message on receiving SIGINT
    kill(pid_A, SIGKILL); // Kill child process A
    kill(pid_B, SIGKILL); // Kill child process B
    show_menu_enabled = true; // Enable menu
  }
}

/**
 * @brief Shows a menu for the user to select the mode of operation.
 * Also handles user inputs for different modes.
 * The user can choose between normal mode, server mode, client mode, and exit.
 * Based on the chosen mode, different processes will be spawned.
 * In server and client modes, the user is asked to enter further details like port and IP.
 */
void show_menu() {
  int mode; // To store user's mode choice
  char port[MAX_PORT_LEN]; // To store user's port input

  const char* menu_str = "\n"
    "\n====================================================\n"
    "||                   MODE SELECTION                ||\n"
    "====================================================\n"
    "|| [1] Normal Mode                                 ||\n"
    "|| [2] Server Mode                                 ||\n"
    "|| [3] Client Mode                                 ||\n"
    "|| [4] Exit                                        ||\n"
    "====================================================\n"
    "Please enter the mode: "
    "\n\n";

  printf("%s", menu_str); // Display menu

  if(scanf("%d", &mode) != 1) {
    fprintf(stderr, "Invalid input\n"); // Print error if input is not a number
    return;
  }

  printf("mode = %d\n", mode);

  // Handle different modes
  switch (mode) {
    case 1: // Normal mode
      strncpy(process, PROCESS_A_NORMAL, sizeof(process));
      break;
    case 2: // Server mode
      strncpy(process, PROCESS_A_SERVER, sizeof(process));
      printf("Enter port:\n");
      if(scanf("%s", port) != 1) {
        fprintf(stderr, "Invalid input\n");
        return;
      }
      printf("port = %s\n", port);
      strncpy(server_params, port, sizeof(server_params)); // Store port as server parameter
      break;
    case 3: // Client mode
      strncpy(process, PROCESS_A_CLIENT, sizeof(process));
      printf("Enter port:\n");
      if(scanf("%s", port) != 1) {
        fprintf(stderr, "Invalid input\n");
        return;
      }
      printf("Enter IP:\n");
      char ip[MAX_IP_LEN];
      if(scanf("%s", ip) != 1) {
        fprintf(stderr, "Invalid input\n");
        return;
      }
      printf("IP = %s\n", ip);
      snprintf(server_params, sizeof(server_params), "%s:%s", ip, port); // Store IP and port as server parameter
      break;
    case 4: // Exit
      exit(EXIT_SUCCESS);
    default:
      fprintf(stderr, "wrong input\n"); // Print error if input is not between 1 and 4
      break;
  }

  // Define argument lists for child processes A and B
  char *arg_list_A[] = {KONSOLE, "-e", process, server_params, NULL};
  char *arg_list_B[] = {KONSOLE, "-e", PROCESS_B, NULL};

  // Spawn child processes A and B
  pid_A = spawn(KONSOLE, arg_list_A);
  pid_B = spawn(KONSOLE, arg_list_B);
}

/**
 * @brief Main function of the program. Sets up signal handling and continually shows menu based on flag.
 * The main function first sets up the signal handler for SIGINT signals.
 * It then enters an infinite loop where it checks the show_menu_enabled flag and shows the menu if the flag is true.
 * After showing the menu once, the flag is set to false.
 * The flag can be set to true again in the signal handler function.
 * @return Exit code.
 */
int main() {
  signal(SIGINT, sig_int_handler); // Set up signal handler

  while(true) { // Enter infinite loop
    if(show_menu_enabled) { // If flag is true, show menu
      show_menu();
      show_menu_enabled = false; // Set flag to false after showing menu
    }
  }

  return 0; // Program should never reach here
}
