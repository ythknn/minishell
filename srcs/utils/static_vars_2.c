#include "../includes/minishell.h"

t_token	*get_current_tokens(t_shell *shell)
{
	return (shell->current_tokens);
}

t_command	*get_current_commands(t_shell *shell)
{
	return (shell->current_commands);
}

void	clear_current_tokens(t_shell *shell)
{
	if (shell->current_tokens)
	{
		free_tokens(shell->current_tokens);
		shell->current_tokens = NULL;
	}
}

void	clear_current_commands(t_shell *shell)
{
	if (shell->current_commands)
	{
		free_commands(shell->current_commands);
		shell->current_commands = NULL;
	}
}
