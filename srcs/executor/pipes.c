/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   pipes.c											:+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: yihakan <yihakan@student.42istanbul.com	+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2025/06/11 16:29:19 by yihakan		   #+#	#+#			 */
/*   Updated: 2025/06/11 16:29:21 by yihakan		  ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

# include "../includes/minishell.h"

int	execute_pipeline(t_command *cmds, t_shell *shell)
{
	t_command	*current;
	int		  pipe_fd[2];
	int		  prev_pipe_read;
	pid_t		pid;
	int		  status;
	int		  last_status;
	char		 *path;

	current = cmds;
	prev_pipe_read = -1;
	last_status = 0;
	
	while (current)
	{
		if (current->next)
			pipe(pipe_fd);
		
		if (!current->args || !current->args[0])
		{
			if (current->next)
			{
				close(pipe_fd[1]);
				prev_pipe_read = pipe_fd[0];
			}
			current = current->next;
			continue;
		}
		if (is_builtin(current->args[0]) && !current->next && !prev_pipe_read)
		{
			last_status = execute_builtin(current->args, shell);
			current = current->next;
			continue;
		}
		
		path = find_executable(current->args[0], shell);
		if (!path && !is_builtin(current->args[0]))
		{
			print_command_not_found(current->args[0]);
			last_status = 127;
			if (current->next)
			{
				close(pipe_fd[0]);
				close(pipe_fd[1]);
			}
			if (prev_pipe_read != -1)
				close(prev_pipe_read);
			current = current->next;
			continue;
		}
		
		pid = fork();
		if (pid == 0)
		{
			if (prev_pipe_read != -1)
			{
				dup2(prev_pipe_read, STDIN_FILENO);
				close(prev_pipe_read);
			}
			if (current->next)
			{
				close(pipe_fd[0]);
				dup2(pipe_fd[1], STDOUT_FILENO);
				close(pipe_fd[1]);
			}
			setup_redirections(current->redirections);
			if (is_builtin(current->args[0]))
				exit(execute_builtin(current->args, shell));
			execve(path, current->args, shell->env_array);
			print_error(current->args[0], NULL, strerror(errno));
			exit(1);
		}
		if (prev_pipe_read != -1)
			close(prev_pipe_read);
		if (current->next)
		{
			close(pipe_fd[1]);
			prev_pipe_read = pipe_fd[0];
		}
		if (path)
			free(path);
		current = current->next;
	}
	while (wait(&status) > 0)
		if (WIFEXITED(status))
			last_status = WEXITSTATUS(status);
	
	return (last_status);
}
