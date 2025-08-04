git /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 16:29:36 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/04 19:26:34 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char	*strip_quotes(char *str)
{
	char	*result;
	int		len;

	if (!str)
		return (NULL);
	len = strlen(str);
	if (len >= 2
		&& ((str[0] == '\'' && str[len - 1] == '\'')
			|| (str[0] == '"' && str[len - 1] == '"')))
	{
		result = malloc(len - 1);
		if (!result)
			return (NULL);
		strncpy(result, str + 1, len - 2);
		result[len - 2] = '\0';
		return (result);
	}
	return (ft_strdup(str));
}

static void cleanup_heredoc_resources(char *content, char *delimiter, t_shell *shell, int stdin_copy)
{
	(void)content;
	
	if (shell)
		gc_free_all(shell);
	
	if (delimiter)
		free(delimiter);
	if (shell)
		free_shell(shell);
	if (stdin_copy != -1)
	{
		dup2(stdin_copy, STDIN_FILENO);
		close(stdin_copy);
	}
	setup_signals();
	
	rl_done = 0;
	rl_replace_line("", 0);
	rl_point = 0;
	rl_end = 0;
}

static int count_heredocs(t_redir *heredocs)
{
	t_redir	*current;
	int		heredoc_count;

	heredoc_count = 0;
	current = heredocs;
	while (current)
	{
		if (current->type == T_HEREDOC)
			heredoc_count++;
		current = current->next;
	}
	return (heredoc_count);
}

static char *process_heredoc_line(char *line, t_shell *shell, char *delimiter, char *current_file)
{
	char *expanded_line;

	if (strcmp(current_file, delimiter) == 0)
	{
		expanded_line = expand_env_vars(line, shell);
		free(line);
		if (!expanded_line)
			return (NULL);
	}
	else
		expanded_line = line;
	return (expanded_line);
}

static char *append_to_content(char *content, char *expanded_line, t_shell *shell)
{
	char	*temp;
	size_t	content_size;

	content_size = strlen(content) + strlen(expanded_line) + 2;
	temp = gc_malloc(shell, content_size, GC_TEMP_STR);
	if (!temp)
	{
		free(expanded_line);
		return (NULL);
	}
	sprintf(temp, "%s%s\n", content, expanded_line);
	gc_free_type(shell, GC_HEREDOC);
	content = gc_strdup(shell, temp, GC_HEREDOC);
	gc_free_type(shell, GC_TEMP_STR);
	free(expanded_line);
	return (content);
}

static char *process_single_heredoc(t_redir *current, t_shell *shell, int current_heredoc, int heredoc_count)
{
	char	*line;
	char	*delimiter;
	char	*expanded_line;
	int		is_last_heredoc;

	is_last_heredoc = (current_heredoc == heredoc_count - 1);
	delimiter = strip_quotes(current->file);
	if (!delimiter)
		return (NULL);
	while (g_signal != SIGINT)
	{
		line = readline("> ");
		if (!line || g_signal == SIGINT)
		{
			if (line)
				free(line);
			free(delimiter);
			return (NULL);
		}
		if (strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		if (is_last_heredoc)
		{
			expanded_line = process_heredoc_line(line, shell, delimiter, current->file);
			if (!expanded_line)
				continue ;
			shell->gc_heredoc = append_to_content(shell->gc_heredoc, expanded_line, shell);
			if (!shell->gc_heredoc)
			{
				free(delimiter);
				return (NULL);
			}
		}
		else
			free(line);
	}
	free(delimiter);
	return (shell->gc_heredoc);
}

static char	*handle_multiple_heredocs(t_redir *heredocs)
{
	t_redir		*current;
	t_shell		shell;
	int			stdin_copy = dup(STDIN_FILENO);
	int			heredoc_count;
	int			current_heredoc = 0;
	extern char **environ;

	init_shell(&shell, environ);
	heredoc_count = count_heredocs(heredocs);
	if (heredoc_count == 0)
	{
		cleanup_heredoc_resources(NULL, NULL, &shell, stdin_copy);
		return (NULL);
	}
	shell.gc_heredoc = gc_strdup(&shell, "", GC_HEREDOC);
	if (!shell.gc_heredoc)
	{
		cleanup_heredoc_resources(NULL, NULL, &shell, stdin_copy);
		return (NULL);
	}
	setup_heredoc_signals();
	g_signal = 0;
	current = heredocs;
	while (current && current_heredoc < heredoc_count && g_signal != SIGINT)
	{
		if (current->type != T_HEREDOC)
		{
			current = current->next;
			continue ;
		}
		if (!process_single_heredoc(current, &shell, current_heredoc, heredoc_count))
		{
			cleanup_heredoc_resources(NULL, NULL, &shell, stdin_copy);
			return (NULL);
		}
		current_heredoc++;
		current = current->next;
	}
	if (g_signal == SIGINT)
	{
		cleanup_heredoc_resources(NULL, NULL, &shell, stdin_copy);
		return (NULL);
	}
	dup2(stdin_copy, STDIN_FILENO);
	close(stdin_copy);
	rl_replace_line("", 0);
	rl_point = 0;
	rl_end = 0;
	rl_done = 0;
	setup_signals();
	char *final_content = NULL;
	if (shell.gc_heredoc)
	{
		final_content = ft_strdup(shell.gc_heredoc);
		gc_free_type(&shell, GC_HEREDOC);
	}
	free_shell(&shell);
	return (final_content);
}

static int handle_input_redirection(t_redir *current)
{
	int fd;

	fd = open(current->file, O_RDONLY);
	if (fd == -1)
	{
		print_error("minishell", current->file, strerror(errno));
		return (1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

static int handle_output_redirection(t_redir *current)
{
	int fd;

	fd = open(current->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		print_error("minishell", current->file, strerror(errno));
		return (1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

static int handle_append_redirection(t_redir *current)
{
	int fd;

	fd = open(current->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		print_error("minishell", current->file, strerror(errno));
		return (1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

static int handle_heredoc_redirection(t_redir *redirs, int *heredoc_processed)
{
	char	*heredoc_content;
	int		pipe_fd[2];

	if (g_signal == SIGINT)
	{
		g_signal = 0;
		return (1);
	}
	heredoc_content = handle_multiple_heredocs(redirs);
	if (!heredoc_content)
	{
		if (g_signal == SIGINT)
			g_signal = 0;
		return (1);
	}
	pipe(pipe_fd);
	write(pipe_fd[1], heredoc_content, strlen(heredoc_content));
	close(pipe_fd[1]);
	dup2(pipe_fd[0], STDIN_FILENO);
	close(pipe_fd[0]);
	free(heredoc_content);
	*heredoc_processed = 1;
	return (0);
}

int	setup_redirections(t_redir *redirs)
{
	t_redir	*current;
	int		heredoc_processed;

	heredoc_processed = 0;
	
	// First pass: process all heredocs
	current = redirs;
	while (current)
	{
		if (current->type == T_HEREDOC && !heredoc_processed)
		{
			if (handle_heredoc_redirection(redirs, &heredoc_processed))
				return (1);
		}
		current = current->next;
	}
	
	// Second pass: process other redirections
	current = redirs;
	while (current)
	{
		if (current->type == T_REDIR_IN)
		{
			if (handle_input_redirection(current))
				return (1);
		}
		else if (current->type == T_REDIR_OUT)
		{
			if (handle_output_redirection(current))
				return (1);
		}
		else if (current->type == T_REDIR_APPEND)
		{
			if (handle_append_redirection(current))
				return (1);
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