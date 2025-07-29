/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:23:11 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/18 20:00:39 by mdusunen         ###   ########.fr       */
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

static int	execute_logical_segment(t_command *start_cmd, t_shell *shell)
{
	t_command *current;
	t_command *segment_start;
	int status;

	segment_start = start_cmd;
	current = start_cmd;
	
	// Find the end of this pipeline segment (until we hit && or || or end)
	while (current && current->operator != T_AND && current->operator != T_OR)
		current = current->next;
	
	// Temporarily break the chain to isolate this segment
	if (current)
	{
		t_command *next_segment = current->next;
		current->next = NULL;
		
		// Execute this segment (might be single command or pipeline)
		if (!segment_start->next)
			status = execute_single_command(segment_start, shell);
		else
			status = execute_pipeline(segment_start, shell);
			
		// Restore the chain
		current->next = next_segment;
	}
	else
	{
		// This is the last segment
		if (!segment_start->next)
			status = execute_single_command(segment_start, shell);
		else
			status = execute_pipeline(segment_start, shell);
	}
	
	return status;
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
		// Execute this logical segment
		status = execute_logical_segment(current, shell);
		if (shell)
			shell->exit_status = status;
		
		// If command failed, stop executing the chain
		if (status != 0)
			return (status);
			
		// Move to the next logical segment
		while (current && current->operator != T_AND && current->operator != T_OR)
			current = current->next;
			
		if (current && current->operator == T_AND)
			current = current->next;
		else
			break;
	}
	return (shell ? shell->exit_status : status);
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
		// Execute this logical segment
		status = execute_logical_segment(current, shell);
		if (shell)
			shell->exit_status = status;
		
		// If command succeeded, stop executing the chain
		if (status == 0)
			return (status);
			
		// Move to the next logical segment
		while (current && current->operator != T_AND && current->operator != T_OR)
			current = current->next;
			
		if (current && current->operator == T_OR)
			current = current->next;
		else
			break;
	}
	return (shell ? shell->exit_status : status);
}

int	execute_commands(t_command *cmds, t_shell *shell)
{
	int	status;
	t_command *current;
	int has_and_operator;
	int has_or_operator;

	if (!cmds)
		return (0);
	if (!cmds->next)
	{
		status = execute_single_command(cmds, shell);
		shell->exit_status = status;
		return (status);
	}
	
	// Check for logical operators (&& or ||)
	has_and_operator = 0;
	has_or_operator = 0;
	current = cmds;
	while (current && current->next)
	{
		if (current->operator == T_AND)
		{
			has_and_operator = 1;
			break;
		}
		else if (current->operator == T_OR)
		{
			has_or_operator = 1;
			break;
		}
		current = current->next;
	}
	
	if (has_and_operator)
	{
		status = execute_and_chain(cmds, shell);
		shell->exit_status = status;
		return (status);
	}
	else if (has_or_operator)
	{
		status = execute_or_chain(cmds, shell);
		shell->exit_status = status;
		return (status);
	}
	
	// Default to pipeline execution for | operators
	status = execute_pipeline(cmds, shell);
	shell->exit_status = status;
	return (status);
}
