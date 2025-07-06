#include "../includes/minishell.h"

void handle_sigint(int sig)
{
    (void)sig;
    g_signal = SIGINT;
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

void handle_heredoc_sigint(int sig)
{
    (void)sig;
    g_signal = SIGINT;
    rl_replace_line("", 0);
    rl_done = 1;
    close(STDIN_FILENO); // STDIN'i kapatarak readline'ı sonlandır
}

void setup_signals(void)
{
    struct sigaction sa;

    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    
    sa.sa_handler = SIG_IGN;
    sigaction(SIGQUIT, &sa, NULL);
}

void setup_heredoc_signals(void)
{
    struct sigaction sa;

    sa.sa_handler = handle_heredoc_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    
    sa.sa_handler = SIG_IGN;
    sigaction(SIGQUIT, &sa, NULL);
}

void reset_signals(void)
{
    struct sigaction sa;

    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
}