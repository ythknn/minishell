#include "../includes/minishell.h"

void	setup_terminal(void)
{
	struct termios	term;

	if (tcgetattr(STDIN_FILENO, &term) == -1)
		return ;
	term.c_lflag &= ~(ECHOCTL | ISIG);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
		return ;
}

void	handle_signal_interrupt(t_shell *shell)
{
	clear_current_tokens(shell);
	clear_current_commands(shell);
	gc_free_all(shell);
	g_signal = 0;
	shell->exit_status = 130;
}
