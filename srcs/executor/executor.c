/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:23:11 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/08 21:09:28 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../libft/libft.h"

static void	execute_external_child(t_command *cmd, t_shell *shell, char *path)
{
	if (setup_redirections(cmd->redirections) != 0)
	{
		free(path);
		free_shell(shell);
		exit(1);
	}
	execve(path, cmd->args, shell->env_array);
	print_error(cmd->args[0], NULL, strerror(errno));
	free(path);
	free_shell(shell);
	exit(1);
}

static int	execute_external_parent(pid_t pid, t_shell *shell, char *path)
{
	int	status;

	if (pid < 0)
	{
		print_error("fork", NULL, strerror(errno));
		free(path);
		shell->exit_status = 1;
		return (1);
	}
	waitpid(pid, &status, 0);
	free(path);
	if (WIFEXITED(status))
		shell->exit_status = WEXITSTATUS(status);
	else
		shell->exit_status = 1;
	return (shell->exit_status);
}

static int	execute_external_command(t_command *cmd, t_shell *shell, char *path)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
		execute_external_child(cmd, shell, path);
	return (execute_external_parent(pid, shell, path));
}

static int	execute_single_command(t_command *cmd, t_shell *shell)
{
	char	*path;

	if (!cmd->args || !cmd->args[0] || is_builtin(cmd->args[0]))
		return (handle_redirections_builtin(cmd, shell));
	path = find_executable(cmd->args[0], shell);
	if (!path)
		return (check_executable_path(cmd, shell));
	return (execute_external_command(cmd, shell, path));
}

int	execute_commands(t_command *cmds, t_shell *shell)
{
	int	status;

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
