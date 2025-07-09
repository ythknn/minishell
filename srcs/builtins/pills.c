#include "../../includes/minishell.h"
#include <termios.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>

extern int g_signal;

static struct termios original_term;

static void display_pills_art(void)
{
    const char *art[] = {
        "    ██████╗ ██╗██╗     ██╗     ███████╗    ████████╗██╗███╗   ███╗███████╗",
        "    ██╔══██╗██║██║     ██║     ██╔════╝    ╚══██╔══╝██║████╗ ████║██╔════╝",
        "    ██████╔╝██║██║     ██║     ███████╗       ██║   ██║██╔████╔██║█████╗  ",
        "    ██╔═══╝ ██║██║     ██║     ╚════██║       ██║   ██║██║╚██╔╝██║██╔══╝  ",
        "    ██║     ██║███████╗███████╗███████║       ██║   ██║██║ ╚═╝ ██║███████╗",
        "    ╚═╝     ╚═╝╚══════╝╚══════╝╚══════╝       ╚═╝   ╚═╝╚═╝     ╚═╝╚══════╝",
        "                                                                      ",
        "   	    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿                  ",
        "   	    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿                  ",
        "   	    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⠉⠉⠉⠀⠀⠉⠙⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿                  ",
        "   	    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠛⠛⠉⠉⠉⠉⠙⠛⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠉⠀⠰⣏⣤⣤⣆⣠⣀⣀⣀⣀⡉⠛⣿⣿⣿⣿⣿⣿⣿                  ",
        "   	    ⣿⣿⣿⣿⣿⣿⣿⠋⠀⢲⢢⢤⣀⡀⠀⠀⠀⠀⠀⠀⠈⢻⣿⣿⣿⣿⣿⡟⠀⠀⠀⠀⠘⣿⣾⡝⠻⢷⠿⠟⡿⠋⠀⠘⣿⣿⣿⣿⣿⣿                  ",
        "   	    ⣿⣿⣿⣿⠟⠋⠀⠠⣔⣮⣭⠉⠙⠉⢁⡥⠞⠀⠀⠀⠀⠈⣿⣿⣿⣿⣿⡇⠂⠀⠀⠒⢄⠈⠉⠿⣧⡄⠲⠟⣿⣶⣤⡄⠈⠻⣿⣿⣿⣿                  ",
        "   	    ⣿⣿⣋⣥⣤⣲⣼⣽⣿⣿⠗⣦⣄⡴⠋⠔⠁⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⡤⠀⠀⠀⠀⠉⠈⢀⠹⠿⡟⠒⠛⢿⣿⣾⣤⠄⠙⢿⣿⣿                  ",
        "   	    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⠏⠉⠀⠀⠀⡰⠀⢩⠂⠀⣆⠀⠀⣸⣿⣿⣿⣿⣿⣷⡤⠀⠒⠢⡄⢄⠀⠡⣄⠑⣤⡄⠀⠙⢻⣿⣿⣿⣾⣿⣿                  ",
        "   	    ⣿⣿⣿⣿⣿⣿⣿⣿⣃⡀⠈⣷⡆⢠⣅⣶⠀⢤⣿⠁⠀⣸⣿⣿⣿⣿⣿⣿⣿⣿⣷⡀⠀⠹⣀⠈⠻⣿⣽⡛⢿⣧⠀⠀⠻⣿⣿⣿⣿⣿                  ",
        "   	    ⣿⣿⣿⣿⣿⣿⣿⠇⠀⠀⣽⡟⢠⣿⡿⠀⠀⡼⠤⢄⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡄⠀⠙⣆⠁⠨⠿⣋⠀⠙⢦⠀⠀⢹⣿⣿⣿⣿                  ",
        "   	    ⣿⣿⣿⣿⣿⣿⣿⠠⡀⠀⣿⠃⠾⡿⠃⠀⢠⠃⠀⢨⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣆⢀⣹⡄⠀⠀⠹⡄⠀⠈⢆⠀⢨⣿⣿⣿⣿                  ",
        "   	    ⣿⣿⣿⣿⣿⣿⣿⣧⣄⣤⠃⠀⢸⠁⠀⠀⣾⣀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⣤⣴⣧⣤⣤⣾⣷⣾⣿⣿⣿⣿                  ",
        "   	    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⣦⣼⣷⣦⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣿⣿⣿⣿⣿                  ",
        "   	    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿                  ",
        "                                                                      ",
        "                          ╔═══════════════════════╗                      ",
        "                          ║    🔴 RED PILL  💊    ║                      ",
        "                          ║    🔵 BLUE PILL 💊    ║                      ",
        "                          ║                       ║                      ",
        "                          ║  \"This is your last   ║                      ",
        "                          ║   chance. After this, ║                      ",
        "                          ║   there is no going   ║                      ",
        "                          ║   back.\"              ║                      ",
        "                          ║                       ║                      ",
        "                          ║     - Morpheus        ║                      ",
        "                          ╚═══════════════════════╝                      ",
		"                                                                      ",
		"      Press Ctrl+C to take the red pill and return to reality...      ",
        NULL
    };
    
    int i = 0;
    while (art[i])
    {
        printf("%s\n", art[i]);
        i++;
    }
}

static void clear_terminal(void)
{
    // Clear screen and move cursor to top-left
    printf("\033[2J\033[H");
    fflush(stdout);
}

static int restore_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return (-1);
    return (fcntl(fd, F_SETFL, flags & ~O_NONBLOCK));
}

static void enable_ctrl_c(void)
{
    struct termios term;
    
    // Save current terminal settings
    tcgetattr(STDIN_FILENO, &original_term);
    
    // Get current settings and enable ISIG to allow Ctrl+C
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ISIG;  // Enable signal generation
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

static void restore_terminal(void)
{
    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &original_term);
}

static void flash_screen(void)
{
    int i;
    for (i = 0; i < 12; i++)
    {
        if (i % 2 == 0)
        {
            // White background
            printf("\033[47m\033[2J\033[H");
        }
        else
        {
            // Black background
            printf("\033[40m\033[2J\033[H");
        }
        fflush(stdout);
        usleep(100000); // 300ms
    }
    // Reset to normal
    printf("\033[0m\033[2J\033[H");
    fflush(stdout);
}

static void show_awakening_message(void)
{
    clear_terminal();
    printf("\n\n\n\n\n\n\n\n");
    printf("                          ██╗   ██╗ ██████╗ ██╗   ██╗    ██████╗ ██████╗ ███████╗\n");
    printf("                          ╚██╗ ██╔╝██╔═══██╗██║   ██║   ██╔══██╗██╔══██╗██╔════╝\n");
    printf("                           ╚████╔╝ ██║   ██║██║   ██║   ███████║██████╔╝█████╗  \n");
    printf("                            ╚██╔╝  ██║   ██║██║   ██║   ██╔══██║██╔══██╗██╔══╝  \n");
    printf("                             ██║   ╚██████╔╝╚██████╔╝   ██║  ██║██║  ██║███████╗\n");
    printf("                             ╚═╝    ╚═════╝  ╚═════╝    ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝\n");
    printf("\n");
    printf("                                 █████╗ ██╗    ██╗ █████╗ ██╗  ██╗███████╗\n");
    printf("                                ██╔══██╗██║    ██║██╔══██╗██║ ██╔╝██╔════╝\n");
    printf("                                ███████║██║ █╗ ██║███████║██████╔╝ █████╗ \n");
    printf("                                ██╔══██║██║███╗██║██╔══██║██╔══██╗ ██╔══╝ \n");
    printf("                                ██║  ██║╚███╔███╔╝██║  ██║██║  ██║ ███████╗██╗\n");
    printf("                                ╚═╝  ╚═╝ ╚══╝╚══╝ ╚═╝  ╚═╝╚═╝  ╚═╝ ╚══════╝╚═╝\n");
    printf("\n\n\n\n");
    fflush(stdout);
    sleep(3);
}

static void show_reality_message(void)
{
    clear_terminal();
    printf("\n\n\n\n\n\n");
    printf("            ██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ███╗███████╗    ████████╗ ██████╗ \n");
    printf("            ██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ████║██╔════╝    ╚══██╔══╝██╔═══██╗\n");
    printf("            ██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔██║█████╗         ██║   ██║   ██║\n");
    printf("            ██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝██║██╔══╝         ██║   ██║   ██║\n");
    printf("            ╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗       ██║   ╚██████╔╝\n");
    printf("             ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝       ╚═╝    ╚═════╝ \n");
    printf("\n");
    printf("             ████████╗██╗  ██╗███████╗     ██████╗ ███████╗ █████╗ ██╗     ██╗████████╗██╗   ██╗\n");
    printf("             ╚══██╔══╝██║  ██║██╔════╝     ██╔══██╗██╔════╝██╔══██╗██║     ██║╚══██╔══╝╚██╗ ██╔╝\n");
    printf("                ██║   ███████║█████╗       ██████╔╝█████╗  ███████║██║     ██║   ██║    ╚████╔╝ \n");
    printf("                ██║   ██╔══██║██╔══╝       ██╔══██╗██╔══╝  ██╔══██║██║     ██║   ██║     ╚██╔╝  \n");
    printf("                ██║   ██║  ██║███████╗     ██║  ██║███████╗██║  ██║███████╗██║   ██║      ██║   \n");
    printf("                ╚═╝   ╚═╝  ╚═╝╚══════╝     ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚══════╝╚═╝   ╚═╝      ╚═╝   \n");
    printf("\n");
    printf("                                    ███╗   ██╗███████╗ ██████╗ \n");
    printf("                                    ████╗  ██║██╔════╝██╔═══██╗\n");
    printf("                                    ██╔██╗ ██║█████╗  ██║   ██║\n");
    printf("                                    ██║╚██╗██║██╔══╝  ██║   ██║\n");
    printf("                                    ██║ ╚████║███████╗╚██████╔╝██╗\n");
    printf("                                    ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝\n");
    printf("\n\n\n");
    fflush(stdout);
}

static int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return (-1);
    return (fcntl(fd, F_SETFL, flags | O_NONBLOCK));
}

static int get_pill_choice(void)
{
    char input[10];
    char buffer;
    int input_pos = 0;
    
    printf("\n\n                    Choose your pill (type 'red' or 'blue'): ");
    fflush(stdout);
    
    // Set stdin to non-blocking mode
    set_nonblocking(STDIN_FILENO);
    
    // Read input character by character while checking for Ctrl+C
    while (1)
    {
        // Check for Ctrl+C signal
        if (g_signal == SIGINT)
        {
            restore_blocking(STDIN_FILENO);
            return (-2); // Signal to exit
        }
        
        // Try to read one character
        int bytes_read = read(STDIN_FILENO, &buffer, 1);
        
        if (bytes_read > 0)
        {
            // Check if it's Ctrl+C (ASCII 3)
            if (buffer == 3)
            {
                restore_blocking(STDIN_FILENO);
                return (-2); // Signal to exit
            }
            
            // Check if it's Enter (newline)
            if (buffer == '\n')
            {
                input[input_pos] = '\0';
                break;
            }
            
            // Add character to input if there's space
            if (input_pos < (int)(sizeof(input) - 1))
            {
                input[input_pos] = buffer;
                input_pos++;
                // Echo the character back to terminal
                write(STDOUT_FILENO, &buffer, 1);
            }
        }
        else if (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            // Real error occurred
            restore_blocking(STDIN_FILENO);
            return (-1);
        }
        
        // Small sleep to avoid busy waiting
        usleep(10000); // 10ms
    }
    
    // Restore blocking mode
    restore_blocking(STDIN_FILENO);
    
    // Convert to lowercase for comparison
    int i = 0;
    while (input[i])
    {
        input[i] = tolower(input[i]);
        i++;
    }
    
    if (strcmp(input, "red") == 0)
        return (1); // Red pill
    else if (strcmp(input, "blue") == 0)
        return (0); // Blue pill
    else
        return (-1); // Invalid input
}

int ft_pills(char **args, t_shell *shell)
{
    int saved_g_signal;
    int choice;
    
    (void)args;
    (void)shell;
    
    // Clear the terminal
    clear_terminal();
    
    // Save the current global signal state
    saved_g_signal = g_signal;
    g_signal = 0;
    
    // Enable Ctrl+C signal generation
    enable_ctrl_c();
    
    // Display the ASCII art
    display_pills_art();
    
    // Get user's pill choice
    while (1)
    {
        choice = get_pill_choice();
        
        if (choice == -2) // Ctrl+C was pressed
        {
            // Allow exit before red pill is chosen
            write(STDOUT_FILENO, "\n", 1);
            clear_terminal();
            restore_terminal();
            break;
        }
        else if (choice == 1) // Red pill
        {
            // Disable Ctrl+C from this point on
            restore_terminal();
            
            // Reset g_signal to ignore future Ctrl+C
            g_signal = 0;
            
            // Flash screen effect
            flash_screen();
            
            // Show "YOU ARE AWAKE" message
            show_awakening_message();
            
            // Show "WELCOME TO THE REALITY NEO" message
            show_reality_message();
            
            // User cannot exit from red pill - infinite loop showing reality
            // Ctrl+C is completely ignored from this point on
            while (1)
            {
                g_signal = 0; // Continuously reset signal to ignore Ctrl+C
                sleep(1);
                // No way to exit, user is stuck in reality
            }
        }
        else if (choice == 0) // Blue pill
        {
            // Clear screen and return to minishell
            clear_terminal();
            printf("You chose the blue pill... back to the comfortable lie.\n\n");
            restore_terminal();
            break;
        }
        else
        {
            // Invalid input, ask again
            printf("Invalid choice. Please type 'red' or 'blue': ");
            fflush(stdout);
        }
    }
    
    // Restore the global signal state and terminal settings
    g_signal = saved_g_signal;
    restore_terminal();
    
    return (0);
} 