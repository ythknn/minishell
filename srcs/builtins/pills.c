/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pills.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:15:45 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/09 19:22:42 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
        "██████╗ ██╗██╗     ██╗     ███████╗  ████████╗██╗███╗   ███╗███████╗",
        "██╔══██╗██║██║     ██║     ██╔════╝  ╚══██╔══╝██║████╗ ████║██╔════╝",
        "██████╔╝██║██║     ██║     ███████╗     ██║   ██║██╔████╔██║█████╗  ",
        "██╔═══╝ ██║██║     ██║     ╚════██║     ██║   ██║██║╚██╔╝██║██╔══╝  ",
        "██║     ██║███████╗███████╗███████║     ██║   ██║██║ ╚═╝ ██║███████╗",
        "╚═╝     ╚═╝╚══════╝╚══════╝╚══════╝     ╚═╝   ╚═╝╚═╝     ╚═╝╚══════╝",
        "                                                                ",
        "    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿       ",
        "    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿       ",
        "    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⠉⠉⠉⠀⠀⠉⠙⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿       ",
        "    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠛⠛⠉⠉⠉⠉⠙⠛⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠉⠀⠰⣏⣤⣤⣆⣠⣀⣀⣀⣀⡉⠛⣿⣿⣿⣿⣿⣿⣿       ",
        "    ⣿⣿⣿⣿⣿⣿⣿⠋⠀⢲⢢⢤⣀⡀⠀⠀⠀⠀⠀⠀⠈⢻⣿⣿⣿⣿⣿⡟⠀⠀⠀⠀⠘⣿⣾⡝⠻⢷⠿⠟⡿⠋⠀⠘⣿⣿⣿⣿⣿⣿       ",
        "    ⣿⣿⣿⣿⠟⠋⠀⠠⣔⣮⣭⠉⠙⠉⢁⡥⠞⠀⠀⠀⠀⠈⣿⣿⣿⣿⣿⡇⠂⠀⠀⠒⢄⠈⠉⠿⣧⡄⠲⠟⣿⣶⣤⡄⠈⠻⣿⣿⣿⣿       ",
        "    ⣿⣿⣋⣥⣤⣲⣼⣽⣿⣿⠗⣦⣄⡴⠋⠔⠁⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⡤⠀⠀⠀⠀⠉⠈⢀⠹⠿⡟⠒⠛⢿⣿⣾⣤⠄⠙⢿⣿⣿       ",
        "    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⠏⠉⠀⠀⠀⡰⠀⢩⠂⠀⣆⠀⠀⣸⣿⣿⣿⣿⣿⣷⡤⠀⠒⠢⡄⢄⠀⠡⣄⠑⣤⡄⠀⠙⢻⣿⣿⣿⣾⣿⣿       ",
        "    ⣿⣿⣿⣿⣿⣿⣿⣿⣃⡀⠈⣷⡆⢠⣅⣶⠀⢤⣿⠁⠀⣸⣿⣿⣿⣿⣿⣿⣿⣿⣷⡀⠀⠹⣀⠈⠻⣿⣽⡛⢿⣧⠀⠀⠻⣿⣿⣿⣿⣿       ",
        "    ⣿⣿⣿⣿⣿⣿⣿⠇⠀⠀⣽⡟⢠⣿⡿⠀⠀⡼⠤⢄⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡄⠀⠙⣆⠁⠨⠿⣋⠀⠙⢦⠀⠀⢹⣿⣿⣿⣿       ",
        "    ⣿⣿⣿⣿⣿⣿⣿⠠⡀⠀⣿⠃⠾⡿⠃⠀⢠⠃⠀⢨⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣆⢀⣹⡄⠀⠀⠹⡄⠀⠈⢆⠀⢨⣿⣿⣿⣿       ",
        "    ⣿⣿⣿⣿⣿⣿⣿⣧⣄⣤⠃⠀⢸⠁⠀⠀⣾⣀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⣤⣴⣧⣤⣤⣾⣷⣾⣿⣿⣿⣿       ",
        "    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⣦⣼⣷⣦⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣿⣿⣿⣿⣿       ",
        "    ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿       ",
        "                                                                ",
        "                      ╔═══════════════════════╗                    ",
        "                      ║    🔴 RED PILL  💊    ║                    ",
        "                      ║    🔵 BLUE PILL 💊    ║                    ",
        "                      ║                       ║                    ",
        "                      ║  \"This is your last   ║                    ",
        "                      ║   chance. After this, ║                    ",
        "                      ║   there is no going   ║                    ",
        "                      ║   back.\"              ║                    ",
        "                      ║                       ║                    ",
        "                      ║     - Morpheus        ║                    ",
        "                      ╚═══════════════════════╝                    ",
		"                                                                  ",
		"  Press Ctrl+C to take the red pill and return to reality...      ",
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
    printf("                          ██╗   ██╗ ██████╗ ██╗   ██╗    ");
    printf("██████╗ ██████╗ ███████╗\n");
    printf("                          ╚██╗ ██╔╝██╔═══██╗██║   ██║   ");
    printf("██╔══██╗██╔══██╗██╔════╝\n");
    printf("                           ╚████╔╝ ██║   ██║██║   ██║   ");
    printf("███████║██████╔╝█████╗  \n");
    printf("                            ╚██╔╝  ██║   ██║██║   ██║   ");
    printf("██╔══██║██╔══██╗██╔══╝  \n");
    printf("                             ██║   ╚██████╔╝╚██████╔╝   ");
    printf("██║  ██║██║  ██║███████╗\n");
    printf("                             ╚═╝    ╚═════╝  ╚═════╝    ");
    printf("╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝\n");
    printf("\n");
    printf("                                 █████╗ ██╗    ██╗ █████╗ ");
    printf("██╗  ██╗███████╗\n");
    printf("                                ██╔══██╗██║    ██║██╔══██╗");
    printf("██║ ██╔╝██╔════╝\n");
    printf("                                ███████║██║ █╗ ██║███████║");
    printf("██████╔╝ █████╗ \n");
    printf("                                ██╔══██║██║███╗██║██╔══██║");
    printf("██╔══██╗ ██╔══╝ \n");
    printf("                                ██║  ██║╚███╔███╔╝██║  ██║");
    printf("██║  ██║ ███████╗██╗\n");
    printf("                                ╚═╝  ╚═╝ ╚══╝╚══╝ ╚═╝  ╚═╝");
    printf("╚═╝  ╚═╝ ╚══════╝╚═╝\n");
    printf("\n\n\n\n");
    fflush(stdout);
    sleep(3);
}

static void show_reality_message(void)
{
    clear_terminal();
    printf("\n\n\n\n\n\n");
    printf("            ██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ");
    printf("███╗███████╗    ████████╗ ██████╗ \n");
    printf("            ██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ");
    printf("████║██╔════╝    ╚══██╔══╝██╔═══██╗\n");
    printf("            ██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔");
    printf("██║█████╗         ██║   ██║   ██║\n");
    printf("            ██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝");
    printf("██║██╔══╝         ██║   ██║   ██║\n");
    printf("            ╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ");
    printf("██║███████╗       ██║   ╚██████╔╝\n");
    printf("             ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ");
    printf("╚═╝╚══════╝       ╚═╝    ╚═════╝ \n");
    printf("\n");
    printf("             ████████╗██╗  ██╗███████╗     ██████╗ ███████╗ ");
    printf("█████╗ ██╗     ██╗████████╗██╗   ██╗\n");
    printf("             ╚══██╔══╝██║  ██║██╔════╝     ██╔══██╗██╔════╝");
    printf("██╔══██╗██║     ██║╚══██╔══╝╚██╗ ██╔╝\n");
    printf("                ██║   ███████║█████╗       ██████╔╝█████╗  ");
    printf("███████║██║     ██║   ██║    ╚████╔╝ \n");
    printf("                ██║   ██╔══██║██╔══╝       ██╔══██╗██╔══╝  ");
    printf("██╔══██║██║     ██║   ██║     ╚██╔╝  \n");
    printf("                ██║   ██║  ██║███████╗     ██║  ██║███████╗");
    printf("██║  ██║███████╗██║   ██║      ██║   \n");
    printf("                ╚═╝   ╚═╝  ╚═╝╚══════╝     ╚═╝  ╚═╝╚══════╝");
    printf("╚═╝  ╚═╝╚══════╝╚═╝   ╚═╝      ╚═╝   \n");
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
    
    printf("\n\nChoose your pill (type 'red' or 'blue'): ");
    fflush(stdout);
    set_nonblocking(STDIN_FILENO);
    while (1)
    {
        if (g_signal == SIGINT)
        {
            restore_blocking(STDIN_FILENO);
            return (-2);
        }
        int bytes_read = read(STDIN_FILENO, &buffer, 1);
        
        if (bytes_read > 0)
        {
            if (buffer == 3)
            {
                restore_blocking(STDIN_FILENO);
                return (-2);
            }
            if (buffer == '\n')
            {
                input[input_pos] = '\0';
                break;
            }
            if (input_pos < (int)(sizeof(input) - 1))
            {
                input[input_pos] = buffer;
                input_pos++;
                write(STDOUT_FILENO, &buffer, 1);
            }
        }
        else if (bytes_read == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            restore_blocking(STDIN_FILENO);
            return (-1);
        }
        usleep(10000);
    }
    restore_blocking(STDIN_FILENO);
    int i = 0;
    while (input[i])
    {
        input[i] = tolower(input[i]);
        i++;
    }
    if (strcmp(input, "red") == 0)
        return (1);
    else if (strcmp(input, "blue") == 0)
        return (0);
    else
        return (-1);
}

int ft_pills(char **args, t_shell *shell)
{
    int saved_g_signal;
    int choice;
    
    (void)args;
    (void)shell;
    clear_terminal();
    saved_g_signal = g_signal;
    g_signal = 0;
    enable_ctrl_c();
    display_pills_art();
    while (1)
    {
        choice = get_pill_choice();
        if (choice == -2)
        {
            write(STDOUT_FILENO, "\n", 1);
            clear_terminal();
            restore_terminal();
            break;
        }
        else if (choice == 1)
        {
            restore_terminal();
            g_signal = 0;
            flash_screen();
            show_awakening_message();
            show_reality_message();
            while (1)
            {
                g_signal = 0;
                sleep(1);
            }
        }
        else if (choice == 0)
        {
            clear_terminal();
            printf("You chose the blue pill... back to the comfortable lie.\n\n");
            restore_terminal();
            break;
        }
        else
        {
            printf("Invalid choice. Please type 'red' or 'blue': ");
            fflush(stdout);
        }
    }
    g_signal = saved_g_signal;
    restore_terminal();
    return (0);
} 
