/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/11 17:43:02 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/21 17:48:27 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	add_redir_to_command(t_command *cmd, t_redir *new_redir)
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

int	handle_pipe_error(t_command *current_cmd, t_token *current_token,
	t_command *commands)
{
	if (!has_valid_command_content(current_cmd))
	{
		print_syntax_error(current_token->value);
		free_commands(commands);
		free_commands(current_cmd);
		return (0);
	}
	if (!find_next_valid_token(current_token->next))
	{
		print_syntax_error("|");
		free_commands(commands);
		free_commands(current_cmd);
		return (0);
	}
	return (1);
}

int	handle_redir_error(t_token *current_token, t_command *commands,
	t_command *current_cmd)
{
	if (current_token)
		print_syntax_error(current_token->value);
	else
		print_syntax_error("newline");
	free_commands(commands);
	free_commands(current_cmd);
	return (0);
}
