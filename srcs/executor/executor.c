/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:23:11 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/30 17:36:38 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include "../libft/libft.h"

static int	handle_redirections_builtin(t_command *cmd, t_shell *shell)
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

static int	check_executable_path(t_command *cmd, t_shell *shell)
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

static int	execute_external_command(t_command *cmd, t_shell *shell, char *path)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == 0)
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
		shell->exit_status = WEXITSTATUS(status);
	else
		shell->exit_status = 1;
	return (shell->exit_status);
}

static int	execute_command_group(t_command *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	if (!cmd->group || !cmd->group->commands)
		return (0);
	
	pid = fork();
	if (pid == 0)
	{
		// Child process - execute the group commands
		if (setup_redirections(cmd->redirections) != 0)
		{
			free_shell(shell);
			exit(1);
		}
		status = execute_commands(cmd->group->commands, shell);
		free_shell(shell);
		exit(status);
	}
	else if (pid < 0)
	{
		print_error("fork", NULL, strerror(errno));
		shell->exit_status = 1;
		return (1);
	}
	
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		shell->exit_status = WEXITSTATUS(status);
	else
		shell->exit_status = 1;
	return (shell->exit_status);
}

static int	execute_single_command(t_command *cmd, t_shell *shell)
{
	char	*path;

	if (cmd->is_group)
		return (execute_command_group(cmd, shell));
	if (!cmd->args || !cmd->args[0] || is_builtin(cmd->args[0]))
		return (handle_redirections_builtin(cmd, shell));
	path = find_executable(cmd->args[0], shell);
	if (!path)
		return (check_executable_path(cmd, shell));
	return (execute_external_command(cmd, shell, path));
}

static int	execute_segment_type(t_command *segment_start)
{
	if (!segment_start->next)
		return (0);
	return (1);
}

static int	execute_segment_command(t_command *segment_start, t_shell *shell,
	int is_pipeline)
{
	if (is_pipeline)
		return (execute_pipeline(segment_start, shell));
	return (execute_single_command(segment_start, shell));
}

static t_command	*find_segment_end(t_command *start_cmd)
{
	t_command	*current;

	current = start_cmd;
	while (current && current->operator != T_AND && current->operator != T_OR)
		current = current->next;
	return (current);
}

static int	execute_logical_segment(t_command *start_cmd, t_shell *shell)
{
	t_command	*current;
	t_command	*segment_start;
	t_command	*next_segment;
	int			status;
	int			is_pipeline;

	segment_start = start_cmd;
	current = find_segment_end(start_cmd);
	if (current)
	{
		next_segment = current->next;
		current->next = NULL;
		is_pipeline = execute_segment_type(segment_start);
		status = execute_segment_command(segment_start, shell, is_pipeline);
		current->next = next_segment;
	}
	else
	{
		is_pipeline = execute_segment_type(segment_start);
		status = execute_segment_command(segment_start, shell, is_pipeline);
	}
	return (status);
}

static t_command	*move_to_next_and_segment(t_command *current)
{
	while (current && current->operator != T_AND && current->operator != T_OR)
		current = current->next;
	if (current && current->operator == T_AND)
		current = current->next;
	else
		current = NULL;
	return (current);
}

static int	execute_and_chain(t_command *cmds, t_shell *shell)
{
	t_command	*current;
	int			status;

	if (!cmds || !shell)
		return (1);
	current = cmds;
	while (current)
	{
		status = execute_logical_segment(current, shell);
		if (shell)
			shell->exit_status = status;
		if (status != 0)
			return (status);
		current = move_to_next_and_segment(current);
	}
	return (shell ? shell->exit_status : status);
}

static t_command	*move_to_next_or_segment(t_command *current)
{
	while (current && current->operator != T_AND && current->operator != T_OR)
		current = current->next;
	if (current && current->operator == T_OR)
		current = current->next;
	else
		current = NULL;
	return (current);
}

static int	execute_or_chain(t_command *cmds, t_shell *shell)
{
	t_command	*current;
	int			status;

	if (!cmds || !shell)
		return (1);
	current = cmds;
	while (current)
	{
		status = execute_logical_segment(current, shell);
		if (shell)
			shell->exit_status = status;
		if (status == 0)
			return (status);
		current = move_to_next_or_segment(current);
	}
	return (shell ? shell->exit_status : status);
}

static int	check_operator_type(t_command *cmds, int *has_and, int *has_or)
{
	t_command	*current;

	*has_and = 0;
	*has_or = 0;
	current = cmds;
	while (current && current->next)
	{
		if (current->operator == T_AND)
		{
			*has_and = 1;
			break ;
		}
		else if (current->operator == T_OR)
		{
			*has_or = 1;
			break ;
		}
		current = current->next;
	}
	return (*has_and || *has_or);
}

int	execute_commands(t_command *cmds, t_shell *shell)
{
	int	status;
	int	has_and_operator;
	int	has_or_operator;

	if (!cmds)
		return (0);
	
	// Preprocess all heredocs before any execution
	if (preprocess_all_heredocs(cmds, shell))
	{
		clear_heredoc_contents();
		return (1);
	}
	if (!cmds->next)
	{
		status = execute_single_command(cmds, shell);
		shell->exit_status = status;
		clear_heredoc_contents();
		return (status);
	}
	check_operator_type(cmds, &has_and_operator, &has_or_operator);
	if (has_and_operator)
	{
		status = execute_and_chain(cmds, shell);
		shell->exit_status = status;
		clear_heredoc_contents();
		return (status);
	}
	else if (has_or_operator)
	{
		status = execute_or_chain(cmds, shell);
		shell->exit_status = status;
		clear_heredoc_contents();
		return (status);
	}
	status = execute_pipeline(cmds, shell);
	shell->exit_status = status;
	clear_heredoc_contents();
	return (status);
}
