/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:58:29 by mdusunen          #+#    #+#             */
/*   Updated: 2025/06/29 18:37:49 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	process_redir_token(t_command *current_cmd, t_token **current_token,
	t_command *commands)
{
	t_token_type	redir_type;

	redir_type = (*current_token)->type;
	*current_token = (*current_token)->next;
	if (!*current_token || (*current_token)->type != T_WORD)
		return (handle_redir_error(*current_token, commands, current_cmd));
	add_redir_to_command(current_cmd,
		create_redir(redir_type, (*current_token)->value));
	return (1);
}

static int	process_pipe_token(t_command **commands, t_command **current_cmd,
	t_token *current_token)
{
	if (!handle_pipe_error(*current_cmd, current_token, *commands))
		return (0);
	add_command(commands, *current_cmd);
	*current_cmd = NULL;
	return (1);
}

static int	process_current_token(t_command **current_cmd,
	t_token **current_token, t_command **commands)
{
	if (!*current_cmd)
		*current_cmd = create_command();
	if ((*current_token)->type == T_PIPE)
	{
		if (!process_pipe_token(commands, current_cmd, *current_token))
			return (0);
	}
	else if (is_redirection((*current_token)->type))
	{
		if (!process_redir_token(*current_cmd, current_token, *commands))
			return (0);
	}
	else if ((*current_token)->type == T_WORD)
		add_arg(*current_cmd, (*current_token)->value);
	return (1);
}

t_command	*parse_token_loop(t_token *tokens)
{
	t_command	*commands;
	t_command	*current_cmd;
	t_token		*current_token;

	commands = NULL;
	current_cmd = NULL;
	current_token = tokens;
	while (current_token)
	{
		if (!process_current_token(&current_cmd, &current_token, &commands))
			return (NULL);
		current_token = current_token->next;
	}
	if (current_cmd)
		add_command(&commands, current_cmd);
	return (commands);
}

t_command	*parse(t_token *tokens)
{
	return (parse_token_loop(tokens));
}
