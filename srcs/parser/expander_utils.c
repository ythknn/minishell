/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:16:30 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/12 19:13:53 by mdusunen         ###   ########.fr       */
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

char	*handle_env_var(char *str, int *i, t_expansion_out *out, t_shell *shell)
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
		ft_strlcpy(out->result + *out->j, env_value, 4096 - *out->j);
		*out->j += ft_strlen(env_value);
	}
	return (out->result);
}

char	*handle_dollar_sign(char *str, int *i,
	t_expansion_out *out, t_shell *shell)
{
	(*i)++;
	if (str[*i] == '?')
		out->result = handle_exit_status(out->result, out->j, i, shell);
	else if (str[*i] && (str[*i] == '_' || (str[*i] >= 'a' && str[*i] <= 'z')
			|| (str[*i] >= 'A' && str[*i] <= 'Z')
			|| (str[*i] >= '0' && str[*i] <= '9')))
		out->result = handle_env_var(str, i, out, shell);
	else
		out->result[(*out->j)++] = '$';
	return (out->result);
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
	//int		count;
	//char	**new_args;

	if (!cmd->args)
		return ;
	expand_args_in_place(cmd->args, shell);
	// The count_non_empty and build_filtered_args functions have been removed.
}
