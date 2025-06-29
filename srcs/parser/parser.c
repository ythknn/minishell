/* ************************************************************************** */
/* */
/* :::      ::::::::   */
/* parser.c                                           :+:      :+:    :+:   */
/* +:+ +:+         +:+     */
/* By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/* +#+#+#+#+#+   +#+           */
/* Created: 2025/06/11 17:43:02 by yihakan           #+#    #+#             */
/* Updated: 2025/06/29 18:30:06 by mdusunen         ###   ########.fr       */
/* */
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
	return (type == T_REDIR_IN || type == T_REDIR_OUT || type == T_REDIR_APPEND || type == T_HEREDOC);
}

// Yeni eklenecek yardımcı fonksiyon
static void add_redir_to_command(t_command *cmd, t_redir *new_redir)
{
    t_redir *current;

    if (!cmd->redirections)
    {
        cmd->redirections = new_redir;
        return;
    }
    current = cmd->redirections;
    while (current->next)
        current = current->next;
    current->next = new_redir;
}


t_command	*parse(t_token *tokens)
{
	t_command	*commands = NULL;
	t_command	*current_cmd = NULL;
	t_token		*current_token = tokens;

	while (current_token)
	{
		if (!current_cmd)
			current_cmd = create_command();

		if (current_token->type == T_PIPE)
		{
			if (!current_cmd->args && !current_cmd->redirections) // Pipe öncesi komut veya redirection yoksa hata
			{
				print_syntax_error(current_token->value);
				free_commands(commands);
				free_commands(current_cmd); // mevcut komutu da serbest bırak
				return (NULL);
			}
			add_command(&commands, current_cmd);
			current_cmd = NULL;
		}
		else if (is_redirection(current_token->type))
		{
			t_token_type redir_type = current_token->type;
			current_token = current_token->next; // Redirection token'ını geç

			if (!current_token || current_token->type != T_WORD) // Redirection'dan sonra dosya adı bekleniyor
			{
                // HATA BURADA YAKALANACAK: "ls < < file.txt" -> ilk <'ten sonra ikinci < token'ı gelir, bu T_WORD değil.
				if (current_token)
					print_syntax_error(current_token->value);
				else
					print_syntax_error("newline");
				free_commands(commands);
				free_commands(current_cmd); // mevcut komutu da serbest bırak
				return (NULL);
			}
			add_redir_to_command(current_cmd, create_redir(redir_type, current_token->value));
		}
		else if (current_token->type == T_WORD)
		{
			add_arg(current_cmd, current_token->value);
		}
		current_token = current_token->next;
	}
	if (current_cmd) // Son komutu ekle
		add_command(&commands, current_cmd);
	return (commands);
}