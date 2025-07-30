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

static int	process_current_token(t_command **current_cmd,
			t_token **current_token, t_command **commands, int *advance_token);

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
	cmd->operator = T_EOF;
	cmd->group = NULL;
	cmd->is_group = 0;
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

t_command_group	*create_command_group(void)
{
	t_command_group	*group;

	group = malloc(sizeof(t_command_group));
	if (!group)
		return (NULL);
	group->commands = NULL;
	group->operator = T_EOF;
	group->next = NULL;
	return (group);
}



void	free_command_groups(t_command_group *groups)
{
	t_command_group	*current;
	t_command_group	*next;

	current = groups;
	while (current)
	{
		next = current->next;
		if (current->commands)
			free_commands(current->commands);
		free(current);
		current = next;
	}
}

static t_command	*parse_group(t_token **current_token)
{
	t_command	*group_cmd;
	t_command	*group_commands;
	t_token		*group_start;
	int			paren_count;

	group_cmd = create_command();
	if (!group_cmd)
		return (NULL);
	group_cmd->is_group = 1;
	*current_token = (*current_token)->next; // Skip opening parenthesis
	group_start = *current_token;
	paren_count = 1;
	
	// Find matching closing parenthesis
	while (*current_token && paren_count > 0)
	{
		if ((*current_token)->type == T_LPAREN)
			paren_count++;
		else if ((*current_token)->type == T_RPAREN)
			paren_count--;
		if (paren_count > 0)
			*current_token = (*current_token)->next;
	}
	
	// Skip the closing parenthesis
	if (*current_token && (*current_token)->type == T_RPAREN)
		*current_token = (*current_token)->next;
	
	if (paren_count > 0)
	{
		free(group_cmd);
		return (NULL); // Unmatched parenthesis
	}
	
	// Parse the commands inside the group
	group_commands = parse_token_range(group_start, *current_token);
	group_cmd->group = create_command_group();
	if (group_cmd->group)
		group_cmd->group->commands = group_commands;
	return (group_cmd);
}

t_command	*parse_token_range(t_token *start, t_token *end)
{
	t_command	*commands;
	t_command	*current_cmd;
	t_token		*current_token;
	int			advance_token;

	commands = NULL;
	current_cmd = NULL;
	current_token = start;
	while (current_token && current_token != end)
	{
		if (!process_current_token(&current_cmd, &current_token, &commands, &advance_token))
			return (NULL);
		if (advance_token)
			current_token = current_token->next;
	}
	if (current_cmd)
		add_command(&commands, current_cmd);
	return (commands);
}

static int	has_valid_command_content(t_command *cmd)
{
	return (cmd->args != NULL || cmd->redirections != NULL || cmd->is_group);
}

static t_token	*find_next_valid_token(t_token *token)
{
	while (token && token->type != T_WORD && !is_redirection(token->type))
		token = token->next;
	return (token);
}

static int	handle_pipe_error(t_command *current_cmd, t_token *current_token,
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

static int	handle_redir_error(t_token *current_token, t_command *commands,
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

static int	process_pipe_token(t_command **commands, t_command **current_cmd,
	t_token *current_token)
{
	if (!handle_pipe_error(*current_cmd, current_token, *commands))
		return (0);
	(*current_cmd)->operator = current_token->type;
	add_command(commands, *current_cmd);
	*current_cmd = NULL;
	return (1);
}

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

static int	process_current_token(t_command **current_cmd,
	t_token **current_token, t_command **commands, int *advance_token)
{
	*advance_token = 1; // Default: advance token
	if (!*current_cmd)
		*current_cmd = create_command();
	if ((*current_token)->type == T_PIPE)
	{
		if (!process_pipe_token(commands, current_cmd, *current_token))
			return (0);
	}
	else if ((*current_token)->type == T_AND)
	{
		if (!process_pipe_token(commands, current_cmd, *current_token))
			return (0);
	}
	else if ((*current_token)->type == T_OR)
	{
		if (!process_pipe_token(commands, current_cmd, *current_token))
			return (0);
	}
	else if (is_redirection((*current_token)->type))
	{
		if (!process_redir_token(*current_cmd, current_token, *commands))
			return (0);
	}
	else if ((*current_token)->type == T_LPAREN)
	{
		t_command	*group_cmd;
		
		group_cmd = parse_group(current_token);
		if (!group_cmd)
			return (0);
		if (*current_cmd && (*current_cmd)->args)
		{
			// If we already have a command, finish it first
			add_command(commands, *current_cmd);
		}
		*current_cmd = group_cmd;
		*advance_token = 0; // parse_group already advanced past the closing parenthesis
	}
	else if ((*current_token)->type == T_RPAREN)
	{
		// Skip - handled by parse_group
		return (1);
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
	int			advance_token;

	commands = NULL;
	current_cmd = NULL;
	current_token = tokens;
	while (current_token)
	{
		if (!process_current_token(&current_cmd, &current_token, &commands, &advance_token))
			return (NULL);
		if (advance_token)
			current_token = current_token->next;
	}
	if (current_cmd)
		add_command(&commands, current_cmd);
	return (commands);
}
