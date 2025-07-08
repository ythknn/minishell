/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 17:18:32 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/05 20:25:32 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	free_args(char **args)
{
	int	i;

	i = 0;
	if (!args)
		return ;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

t_redir	*create_redir(t_token_type type, char *file)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (NULL);
	redir->type = type;
	redir->file = strdup(file);
	redir->next = NULL;
	return (redir);
}

void	free_commands(t_command *cmds)
{
	t_command	*current_cmd;
	t_command	*next_cmd;
	t_redir		*current_redir;
	t_redir		*next_redir;

	current_cmd = cmds;
	while (current_cmd)
	{
		next_cmd = current_cmd->next;
		if (current_cmd->args)
			free_args(current_cmd->args);
		current_redir = current_cmd->redirections;
		while (current_redir)
		{
			next_redir = current_redir->next;
			free(current_redir->file);
			free(current_redir);
			current_redir = next_redir;
		}
		free(current_cmd);
		current_cmd = next_cmd;
	}
}

void	add_redir(t_redir **redirs, t_redir *new_redir)
{
	t_redir	*current;

	if (!*redirs)
	{
		*redirs = new_redir;
		return ;
	}
	current = *redirs;
	while (current->next)
		current = current->next;
	current->next = new_redir;
}

t_token	*handle_redirection(t_token *current, t_command *cmd)
{
	if (current->next && current->next->type == T_WORD)
	{
		add_redir(&cmd->redirections,
			create_redir(current->type, current->next->value));
		return (current->next);
	}
	return (current);
}
