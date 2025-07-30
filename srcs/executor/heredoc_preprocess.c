/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_preprocess.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:30:00 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/30 17:35:48 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

typedef struct s_heredoc_content
{
	char						*content;
	struct s_heredoc_content	*next;
}	t_heredoc_content;

static t_heredoc_content	*g_heredoc_contents = NULL;

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

static char	*collect_heredoc_content(char *delimiter, t_shell *shell)
{
	char	*line;
	char	*content;
	char	*temp;
	char	*expanded_line;
	size_t	content_size;

	content = ft_strdup("");
	content_size = 1;
	if (!content)
		return (NULL);
	
	while (1)
	{
		line = readline("> ");
		if (!line || g_signal == SIGINT)
		{
			if (line)
				free(line);
			free(content);
			return (NULL);
		}
		if (strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		
		expanded_line = expand_env_vars(line, shell);
		free(line);
		if (!expanded_line)
			continue ;
		
		content_size += strlen(expanded_line) + 1;
		temp = malloc(content_size);
		if (!temp)
		{
			free(expanded_line);
			free(content);
			return (NULL);
		}
		sprintf(temp, "%s%s\n", content, expanded_line);
		free(content);
		free(expanded_line);
		content = temp;
	}
	return (content);
}

static void	add_heredoc_content(char *content)
{
	t_heredoc_content	*new_node;
	t_heredoc_content	*current;

	new_node = malloc(sizeof(t_heredoc_content));
	if (!new_node)
		return ;
	new_node->content = content;
	new_node->next = NULL;
	
	if (!g_heredoc_contents)
	{
		g_heredoc_contents = new_node;
		return ;
	}
	
	current = g_heredoc_contents;
	while (current->next)
		current = current->next;
	current->next = new_node;
}

static char	*get_next_heredoc_content(void)
{
	t_heredoc_content	*node;
	char				*content;

	if (!g_heredoc_contents)
		return (NULL);
	
	node = g_heredoc_contents;
	content = node->content;
	g_heredoc_contents = node->next;
	free(node);
	return (content);
}

void	clear_heredoc_contents(void)
{
	t_heredoc_content	*current;
	t_heredoc_content	*next;

	current = g_heredoc_contents;
	while (current)
	{
		next = current->next;
		free(current->content);
		free(current);
		current = next;
	}
	g_heredoc_contents = NULL;
}

int	preprocess_heredocs_in_command(t_command *cmd, t_shell *shell)
{
	t_redir	*current;
	char	*delimiter;
	char	*content;

	if (!cmd)
		return (0);
	
	current = cmd->redirections;
	while (current)
	{
		if (current->type == T_HEREDOC)
		{
			if (g_signal == SIGINT)
				return (1);
			
			delimiter = strip_quotes(current->file);
			if (!delimiter)
				return (1);
			
			setup_heredoc_signals();
			content = collect_heredoc_content(delimiter, shell);
			setup_signals();
			free(delimiter);
			
			if (!content || g_signal == SIGINT)
			{
				if (content)
					free(content);
				return (1);
			}
			
			add_heredoc_content(content);
		}
		current = current->next;
	}
	return (0);
}

int	preprocess_all_heredocs(t_command *cmds, t_shell *shell)
{
	t_command	*current;

	clear_heredoc_contents();
	current = cmds;
	while (current)
	{
		if (current->is_group)
		{
			if (preprocess_all_heredocs(current->group->commands, shell))
				return (1);
		}
		else
		{
			if (preprocess_heredocs_in_command(current, shell))
				return (1);
		}
		current = current->next;
	}
	return (0);
}

char	*get_preprocessed_heredoc_content(void)
{
	return (get_next_heredoc_content());
} 