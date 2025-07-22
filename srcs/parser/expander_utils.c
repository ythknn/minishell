/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:16:30 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/15 18:16:30 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

char	*handle_exit_status(char *result, int *j, int *i, t_shell *shell)
{
	char	*exit_str;
	int		exit_len;

	exit_str = ft_itoa(shell->exit_status);
	if (exit_str)
	{
		exit_len = ft_strlen(exit_str);
		ft_strlcpy(result + *j, exit_str, 4096 - *j);
		*j += exit_len;
		free(exit_str);
	}
	(*i)++;
	return (result);
}

char	*handle_env_var(char *str, int *i, char *result, int *j, t_shell *shell)
{
	char	var_name[256];
	char	*env_value;
	int		k;

	k = 0;
	while (str[*i] && (str[*i] == '_' || (str[*i] >= 'a' && str[*i] <= 'z')
			|| (str[*i] >= 'A' && str[*i] <= 'Z')
			|| (str[*i] >= '0' && str[*i] <= '9')))
	{
		var_name[k++] = str[(*i)++];
	}
	var_name[k] = '\0';
	env_value = get_env_value(shell->env_list, var_name);
	if (env_value)
	{
		ft_strlcpy(result + *j, env_value, 4096 - *j);
		*j += ft_strlen(env_value);
	}
	return (result);
}

char	*handle_dollar_sign(char *str, int *i,
	char *result, int *j, t_shell *shell)
{
	(*i)++;
	if (str[*i] == '?')
		result = handle_exit_status(result, j, i, shell);
	else if (str[*i] && (str[*i] == '_' || (str[*i] >= 'a' && str[*i] <= 'z')
			|| (str[*i] >= 'A' && str[*i] <= 'Z')
			|| (str[*i] >= '0' && str[*i] <= '9')))
		result = handle_env_var(str, i, result, j, shell);
	else
		result[(*j)++] = '$';
	return (result);
}

void	expand_redirections(t_command *cmd, t_shell *shell)
{
	t_redir		*current_redir;
	char		*expanded;

	current_redir = cmd->redirections;
	while (current_redir)
	{
		if (current_redir->type != T_HEREDOC)
		{
			expanded = expand_env_vars(current_redir->file, shell);
			free(current_redir->file);
			current_redir->file = expanded;
		}
		current_redir = current_redir->next;
	}
}

void	expand_args(t_command *cmd, t_shell *shell)
{
	int		i;
	int		j;
	char	*expanded;
	char	**new_args;
	int		count;

	if (!cmd->args)
		return ;
	i = 0;
	while (cmd->args[i])
	{
		expanded = expand_env_vars(cmd->args[i], shell);
		free(cmd->args[i]);
		cmd->args[i] = expanded;
		i++;
	}
	count = 0;
	i = 0;
	while (cmd->args[i])
	{
		if (cmd->args[i][0] != '\0')
			count++;
		i++;
	}
	if (count == 0)
	{
		free_args(cmd->args);
		cmd->args = NULL;
		return ;
	}
	new_args = malloc(sizeof(char *) * (count + 1));
	if (!new_args)
		return ;
	i = 0;
	j = 0;
	while (cmd->args[i])
	{
		if (cmd->args[i][0] != '\0')
		{
			new_args[j] = cmd->args[i];
			j++;
		}
		else
			free(cmd->args[i]);
		i++;
	}
	new_args[j] = NULL;
	free(cmd->args);
	cmd->args = new_args;
}
