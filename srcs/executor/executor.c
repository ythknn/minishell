/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:23:11 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/09 19:23:53 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/minishell.h"
# include "../libft/libft.h"

static int	execute_single_command(t_command *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;
	char	*path;
	struct stat	st;
	int		stdin_copy;
	int		stdout_copy;

	if (!cmd->args || !cmd->args[0])
	{
		if (cmd->redirections)
		{
			stdin_copy = dup(STDIN_FILENO);
			stdout_copy = dup(STDOUT_FILENO);
			
			if (setup_redirections(cmd->redirections) != 0)
			{
				restore_redirections(stdin_copy, stdout_copy);
				shell->exit_status = 1;
				return (1);
			}
			
			restore_redirections(stdin_copy, stdout_copy);
		}
		return (0);
	}
	
	if (is_builtin(cmd->args[0]))
	{
		stdin_copy = dup(STDIN_FILENO);
		stdout_copy = dup(STDOUT_FILENO);
		
		if (setup_redirections(cmd->redirections) != 0)
		{
			restore_redirections(stdin_copy, stdout_copy);
			shell->exit_status = 1;
			return (1);
		}
		
		status = execute_builtin(cmd->args, shell);
		restore_redirections(stdin_copy, stdout_copy);
		shell->exit_status = status;
		return (status);
	}
	
	path = find_executable(cmd->args[0], shell);
	if (!path)
	{
		if (cmd->args[0][0] == '/' || (cmd->args[0][0] == '.' && cmd->args[0][1] == '/') || 
			(cmd->args[0][0] == '.' && cmd->args[0][1] == '.' && cmd->args[0][2] == '/'))
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
		{
			print_error(cmd->args[0], NULL, "command not found");
		}
		shell->exit_status = 127;
		return (127);
	}
	pid = fork();
	if (pid == 0)
	{
		if (setup_redirections(cmd->redirections) != 0)
			exit(1);
		execve(path, cmd->args, shell->env_array);
		print_error(cmd->args[0], NULL, strerror(errno));
		exit(1);
	}
	else if (pid < 0)
	{
		print_error("fork", NULL, strerror(errno));
		free(path);
		shell->exit_status = 1;
		return (1);
	}
	waitpid(pid, &status, 0);
	free(path);	
	if (WIFEXITED(status))
	{
		shell->exit_status = WEXITSTATUS(status);
		return (WEXITSTATUS(status));
	}
	shell->exit_status = 1;
	return (1);
}

int	execute_commands(t_command *cmds, t_shell *shell)
{
	int		status;

	if (!cmds)
		return (0);
	
	if (!cmds->next)
	{
		status = execute_single_command(cmds, shell);
		shell->exit_status = status;
		return (status);
	}
	status = execute_pipeline(cmds, shell);
	shell->exit_status = status;
	return (status);
}
