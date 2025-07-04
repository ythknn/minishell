/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   redirections.c									 :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: yihakan <yihakan@student.42istanbul.com	+#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2025/06/11 16:29:36 by yihakan		   #+#	#+#			 */
/*   Updated: 2025/06/11 16:35:44 by yihakan		  ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

# include "../includes/minishell.h"

static char *handle_heredoc(char *delimiter)
{
	char *line;
	char *content;
	char *temp;
	char *expanded_line;
	size_t content_size;
	t_shell shell;

	extern char **environ;
	init_shell(&shell, environ);
	content = strdup("");
	if (!content)
		return (NULL);
	content_size = 1;
	
	while (1)
	{
		line = readline("> ");
		if (!line || g_signal == SIGINT)
		{
			free(content);
			free_shell(&shell);
			free(line);
			return (NULL);
		}
		
		if (line[0] != '\0' && strcmp(line, delimiter) == 0)
		{
			free(line);
			break;
		}
		
		expanded_line = expand_env_vars(line, &shell);
		if (expanded_line)
		{
			content_size += strlen(expanded_line) + 1;
			temp = malloc(content_size);
			if (!temp)
			{
				free(content);
				free(line);
				free(expanded_line);
				free_shell(&shell);
				return (NULL);
			}
			sprintf(temp, "%s%s\n", content, expanded_line);
			free(content);
			free(expanded_line);
			content = temp;
		}
		free(line);
	}
	
	free_shell(&shell);
	return (content);
}

int	setup_redirections(t_redir *redirs)
{
	t_redir	*current;
	int		fd;
	char	   *heredoc_content;

	current = redirs;
	while (current)
	{
		if (current->type == T_REDIR_IN)
		{
			fd = open(current->file, O_RDONLY);
			if (fd == -1)
			{
				print_error("minishell", current->file, strerror(errno));
				return (1);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
		}
		else if (current->type == T_REDIR_OUT)
		{
			fd = open(current->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
			{
				print_error("minishell", current->file, strerror(errno));
				return (1);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (current->type == T_REDIR_APPEND)
		{
			fd = open(current->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
			if (fd == -1)
			{
				print_error("minishell", current->file, strerror(errno));
				return (1);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
		}
		else if (current->type == T_HEREDOC)
		{
			int pipe_fd[2];
			
			heredoc_content = handle_heredoc(current->file);
			if (!heredoc_content)
				return (1);
				
			pipe(pipe_fd);
			write(pipe_fd[1], heredoc_content, strlen(heredoc_content));
			close(pipe_fd[1]);
			dup2(pipe_fd[0], STDIN_FILENO);
			close(pipe_fd[0]);
			free(heredoc_content);
		}
		current = current->next;
	}
	return (0);
}

void	restore_redirections(int stdin_copy, int stdout_copy)
{
	dup2(stdin_copy, STDIN_FILENO);
	dup2(stdout_copy, STDOUT_FILENO);
	close(stdin_copy);
	close(stdout_copy);
}