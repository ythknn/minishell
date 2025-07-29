/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 16:29:36 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/28 18:17:30 by mdusunen         ###   ########.fr       */
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
	(void)content; // Unused parameter - GC handles content cleanup
	
	// GC sistemindeki heredoc memory'yi temizle
	gc_free_all();
	
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
}

static char	*handle_multiple_heredocs(t_redir *heredocs)
{
	t_redir		*current;
	char		*line;
	char		*content = NULL;
	char		*temp;
	char		*expanded_line;
	char		*delimiter;
	size_t		content_size = 1;
	t_shell		shell;
	int			stdin_copy = dup(STDIN_FILENO);
	int			heredoc_count = 0;
	int			current_heredoc = 0;
	int			is_last_heredoc;
	extern char **environ;

	init_shell(&shell, environ);
	current = heredocs;
	while (current)
	{
		if (current->type == T_HEREDOC)
			heredoc_count++;
		current = current->next;
	}
	if (heredoc_count == 0)
	{
		cleanup_heredoc_resources(NULL, NULL, &shell, stdin_copy);
		return (NULL);
	}
	setup_heredoc_signals();
	// free_heredoc(&shell);
	g_signal = 0;
	current = heredocs;
	while (current && current_heredoc < heredoc_count && g_signal != SIGINT)
	{
		if (current->type != T_HEREDOC)
		{
			current = current->next;
			continue ;
		}
		is_last_heredoc = (current_heredoc == heredoc_count - 1);
		delimiter = strip_quotes(current->file);
		if (!delimiter)
		{
			cleanup_heredoc_resources(content, NULL, &shell, stdin_copy);
			return (NULL);
		}
		if (is_last_heredoc && !content)
		{
			content = gc_strdup("", GC_HEREDOC);
			content_size = 1;
			if (!content)
			{
				cleanup_heredoc_resources(NULL, delimiter, &shell, stdin_copy);
				return (NULL);
			}
		}
		while (g_signal != SIGINT)
		{
			line = readline("> ");
			if (!line || g_signal == SIGINT)
			{
				if (line)
					free(line);
				cleanup_heredoc_resources(content, delimiter, &shell, stdin_copy);
				return (NULL);
			}
			if (strcmp(line, delimiter) == 0)
			{
				free(line);
				break ;
			}
			if (is_last_heredoc)
			{
				if (strcmp(current->file, delimiter) == 0)
				{
					expanded_line = expand_env_vars(line, &shell);
					free(line);
					if (!expanded_line)
						continue ;
				}
				else
				{
					expanded_line = line;
				}
				content_size += strlen(expanded_line) + 1;
				temp = gc_malloc(content_size, GC_TEMP_STR);
				if (!temp)
				{
					free(expanded_line);
					cleanup_heredoc_resources(content, delimiter, &shell, stdin_copy);
					return (NULL);
				}
				sprintf(temp, "%s%s\n", content, expanded_line);
				// Eski content'i serbest bırak, yeni temp GC_HEREDOC'a ata
				gc_free_type(GC_HEREDOC);
				content = gc_strdup(temp, GC_HEREDOC);
				gc_free_type(GC_TEMP_STR);
				free(expanded_line);
			}
			else
			{
				free(line);
			}
		}
		free(delimiter);
		current_heredoc++;
		current = current->next;
	}
	if (g_signal == SIGINT)
	{
		cleanup_heredoc_resources(content, NULL, &shell, stdin_copy);
		return (NULL);
	}
	
	dup2(stdin_copy, STDIN_FILENO);
	close(stdin_copy);
	rl_replace_line("", 0);
	rl_point = 0;
	rl_end = 0;
	setup_signals();
	free_shell(&shell);
	
	// Normal completion'da content'i GC'den çıkar ve return et
	char *final_content = NULL;
	if (content)
	{
		final_content = ft_strdup(content);
		gc_free_type(GC_HEREDOC);
	}
	return (final_content);
}

int	setup_redirections(t_redir *redirs)
{
	t_redir	*current;
	int		fd;
	char	*heredoc_content;
	int		heredoc_processed;

	heredoc_content = NULL;
	heredoc_processed = 0;
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
		else if (current->type == T_HEREDOC && !heredoc_processed)
		{
			int	pipe_fd[2];
			
			// Check for signal before processing heredoc
			if (g_signal == SIGINT)
			{
				g_signal = 0;
				return (1);
			}
			
			heredoc_content = handle_multiple_heredocs(redirs);
			if (!heredoc_content)
			{
				if (g_signal == SIGINT)
				{
					g_signal = 0;
				}
				return (1);
			}
			pipe(pipe_fd);
			write(pipe_fd[1], heredoc_content, strlen(heredoc_content));
			close(pipe_fd[1]);
			dup2(pipe_fd[0], STDIN_FILENO);
			close(pipe_fd[0]);
			free(heredoc_content);
			heredoc_processed = 1;
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
