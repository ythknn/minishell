/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 16:29:36 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/05 20:29:17 by yihakan          ###   ########.fr       */
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

static int	count_heredocs(t_redir *heredocs)
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

static char	*process_heredoc_line_simple(char *line, char *delimiter, char *current_file)
{
	char	*expanded_line;

	if (strcmp(current_file, delimiter) == 0)
	{
		expanded_line = ft_strdup(line);
		free(line);
		if (!expanded_line)
			return (NULL);
	}
	else
		expanded_line = line;
	return (expanded_line);
}

static char	*read_heredoc_line(void)
{
	char	*line;
	if (g_signal == SIGINT)
		return (NULL);

	line = readline("> ");
	if (g_signal == SIGINT || !line)
	{
		if (line)
		{
			free(line);
			line = NULL;
		}
		return (NULL);
	}
	
	return (line);
}

static int	process_single_heredoc_simple(t_redir *current, int current_heredoc, int heredoc_count, char **heredoc_content)
{
	char	*line;
	char	*delimiter;
	char	*expanded_line;
	char	*temp;
	size_t	content_size;
	int		is_last_heredoc;

	is_last_heredoc = (current_heredoc == heredoc_count - 1);
	delimiter = strip_quotes(current->file);
	if (!delimiter)
		return (0);
	while (g_signal != SIGINT)
	{
		line = read_heredoc_line();
		if (!line || g_signal == SIGINT)
		{
			if (line)
				free(line);
			free(delimiter);
			return (0);
		}
		if (strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		if (is_last_heredoc)
		{
			expanded_line = process_heredoc_line_simple(line, delimiter, current->file);
			if (!expanded_line)
				continue ;
			content_size = strlen(*heredoc_content) + strlen(expanded_line) + 2;
			temp = malloc(content_size);
			if (!temp)
			{
				free(expanded_line);
				free(delimiter);
				return (0);
			}
			sprintf(temp, "%s%s\n", *heredoc_content, expanded_line);
			free(*heredoc_content);
			*heredoc_content = temp;
			free(expanded_line);
		}
		else
			free(line);
	}
	free(delimiter);
	return (1);
}

char	*handle_multiple_heredocs(t_redir *heredocs)
{
	t_redir		*current;
	int			stdin_copy;
	int			heredoc_count;
	int			current_heredoc;
	char		*final_content;
	char		*heredoc_content;
	struct termios	term;

	stdin_copy = dup(STDIN_FILENO);
	current_heredoc = 0;
	heredoc_count = count_heredocs(heredocs);
	if (heredoc_count == 0)
	{
		close(stdin_copy);
		return (NULL);
	}
	heredoc_content = ft_strdup("");
	if (!heredoc_content)
	{
		close(stdin_copy);
		return (NULL);
	}
	if (tcgetattr(STDIN_FILENO, &term) == 0)
	{
		term.c_lflag |= ECHO;
		tcsetattr(STDIN_FILENO, TCSANOW, &term);
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
		if (!process_single_heredoc_simple(current, current_heredoc, heredoc_count, &heredoc_content))
		{
			free(heredoc_content);
			close(stdin_copy);
			return (NULL);
		}
		current_heredoc++;
		current = current->next;
	}
	if (g_signal == SIGINT)
	{
		free(heredoc_content);
		close(stdin_copy);
		return (NULL);
	}
	dup2(stdin_copy, STDIN_FILENO);
	close(stdin_copy);
	rl_replace_line("", 0);
	rl_point = 0;
	rl_end = 0;
	rl_done = 0;
	setup_signals();
	final_content = heredoc_content;
	return (final_content);
}

static int	handle_input_redirection(t_redir *current)
{
	int	fd;

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

static int	handle_output_redirection(t_redir *current)
{
	int	fd;

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

static int	handle_append_redirection(t_redir *current)
{
	int	fd;

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

static int	handle_heredoc_redirection(t_redir *redirs, int *heredoc_processed)
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
