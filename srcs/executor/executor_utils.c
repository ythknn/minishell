/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:23:59 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/12 20:27:03 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	check_executable_path(t_command *cmd, t_shell *shell)
{
	struct stat	st;

	if (cmd->args[0][0] == '/' || (cmd->args[0][0] == '.'
		&& cmd->args[0][1] == '/') || (cmd->args[0][0] == '.'
		&& cmd->args[0][1] == '.' && cmd->args[0][2] == '/'))
	{
		if (access(cmd->args[0], F_OK) == 0)
		{
			if (stat(cmd->args[0], &st) == 0 && S_ISDIR(st.st_mode))
			{
				shell->exit_status = 126;
				return (126);
			}
			if (access(cmd->args[0], X_OK) != 0)
			{
				shell->exit_status = 126;
				return (126);
			}
		}
	}
	else
		print_error(cmd->args[0], NULL, "command not found");
	shell->exit_status = 127;
	return (127);
}

int	handle_redirections_builtin(t_command *cmd, t_shell *shell)
{
	int	stdin_copy;
	int	stdout_copy;
	int	status;

	stdin_copy = dup(STDIN_FILENO);
	stdout_copy = dup(STDOUT_FILENO);
	if (setup_redirections(cmd->redirections) != 0)
	{
		restore_redirections(stdin_copy, stdout_copy);
		shell->exit_status = 1;
		return (1);
	}
	if (cmd->args && cmd->args[0])
		status = execute_builtin(cmd->args, shell);
	else
		status = 0;
	restore_redirections(stdin_copy, stdout_copy);
	shell->exit_status = status;
	return (status);
}
