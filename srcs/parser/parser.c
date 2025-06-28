/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:43:02 by yihakan           #+#    #+#             */
/*   Updated: 2025/06/25 17:35:01 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	add_arg(t_command *cmd, char *arg)
{
	int		i;
	int		size;
	char	**new_args;

	if (!cmd->args)
	{
		cmd->args = malloc(sizeof(char *) * 2);
		cmd->args[0] = strdup(arg);
		cmd->args[1] = NULL;
		return ;
	}
	size = 0;
	while (cmd->args[size])
		size++;
	new_args = malloc(sizeof(char *) * (size + 2));
	i = 0;
	while (i < size)
	{
		new_args[i] = cmd->args[i];
		i++;
	}
	new_args[i] = strdup(arg);
	new_args[i + 1] = NULL;
	free(cmd->args);
	cmd->args = new_args;
}

static t_command	*create_command(void)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->args = NULL;
	cmd->redirections = NULL;
	cmd->next = NULL;
	return (cmd);
}

static void	add_command(t_command **cmds, t_command *new_cmd)
{
	t_command	*current;

	if (!*cmds)
	{
		*cmds = new_cmd;
		return ;
	}
	current = *cmds;
	while (current->next)
		current = current->next;
	current->next = new_cmd;
}

static int	is_redirection(t_token_type type)
{
	return (type == T_REDIR_IN || type == T_REDIR_OUT
		|| type == T_REDIR_APPEND || type == T_HEREDOC);
}

t_command	*parse(t_token *tokens)
{
	t_command	*cmds;
	t_command	*current_cmd;
	t_token		*current;

	cmds = NULL;
	current_cmd = create_command();
	current = tokens;
	while (current)
	{
		if (current->type == T_PIPE)
		{
			add_command(&cmds, current_cmd);
			current_cmd = create_command();
		}
		else if (is_redirection(current->type))
			current = handle_redirection(current, current_cmd);
		else if (current->type == T_WORD)
			add_arg(current_cmd, current->value);
		current = current->next;
	}
	add_command(&cmds, current_cmd);
	return (cmds);
}
