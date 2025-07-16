/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:43:02 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/14 20:37:37 by mdusunen         ###   ########.fr       */
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

static void	add_redir_to_command(t_command *cmd, t_redir *new_redir)
{
	t_redir	*current;

	if (!cmd->redirections)
	{
		cmd->redirections = new_redir;
		return ;
	}
	current = cmd->redirections;
	while (current->next)
		current = current->next;
	current->next = new_redir;
}

static int	has_valid_command_content(t_command *cmd)
{
	return (cmd->args != NULL || cmd->redirections != NULL);
}

static t_token	*find_next_valid_token(t_token *token)
{
	while (token && token->type != T_WORD && !is_redirection(token->type))
		token = token->next;
	return (token);
}

t_command	*parse(t_token *tokens)
{
	t_command		*commands;
	t_command		*current_cmd;
	t_token			*current_token;
	t_token_type	redir_type;

	commands = NULL;
	current_cmd = NULL;
	current_token = tokens;
	while (current_token)
	{
		if (!current_cmd)
			current_cmd = create_command();
		if (current_token->type == T_PIPE)
		{
			if (!has_valid_command_content(current_cmd))
			{
				print_syntax_error(current_token->value);
				free_commands(commands);
				free_commands(current_cmd);
				return (NULL);
			}
			if (!find_next_valid_token(current_token->next))
			{
				print_syntax_error("|");
				free_commands(commands);
				free_commands(current_cmd);
				return (NULL);
			}
			add_command(&commands, current_cmd);
			current_cmd = NULL;
		}
		else if (is_redirection(current_token->type))
		{
			redir_type = current_token->type;
			current_token = current_token->next;
			if (!current_token || current_token->type != T_WORD)
			{
				if (current_token)
					print_syntax_error(current_token->value);
				else
					print_syntax_error("newline");
				free_commands(commands);
				free_commands(current_cmd);
				return (NULL);
			}
			add_redir_to_command(current_cmd,
				create_redir(redir_type, current_token->value));
		}
		else if (current_token->type == T_WORD)
		{
			add_arg(current_cmd, current_token->value);
		}
		current_token = current_token->next;
	}
	if (current_cmd)
		add_command(&commands, current_cmd);
	return (commands);
}
