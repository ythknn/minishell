/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:16:30 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/30 17:27:40 by yihakan          ###   ########.fr       */
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

char	*handle_env_var(t_expand_ctx *ctx, t_shell *shell)
{
	char	var_name[256];
	char	*env_value;
	int		k;

	k = 0;
	while (ctx->str[*ctx->i] && (ctx->str[*ctx->i] == '_' 
		|| (ctx->str[*ctx->i] >= 'a' && ctx->str[*ctx->i] <= 'z')
		|| (ctx->str[*ctx->i] >= 'A' && ctx->str[*ctx->i] <= 'Z')
		|| (ctx->str[*ctx->i] >= '0' && ctx->str[*ctx->i] <= '9')))
	{
		var_name[k++] = ctx->str[(*ctx->i)++];
	}
	var_name[k] = '\0';
	env_value = get_env_value(shell->env_list, var_name);
	if (env_value)
	{
		ft_strlcpy(ctx->result + *ctx->j, env_value, 4096 - *ctx->j);
		*ctx->j += ft_strlen(env_value);
	}
	return (ctx->result);
}

char	*handle_dollar_sign(t_expand_ctx *ctx, t_shell *shell)
{
	(*ctx->i)++;
	if (ctx->str[*ctx->i] == '?')
		ctx->result = handle_exit_status(ctx->result, ctx->j, ctx->i, shell);
	else if (ctx->str[*ctx->i] && (ctx->str[*ctx->i] == '_' 
		|| (ctx->str[*ctx->i] >= 'a' && ctx->str[*ctx->i] <= 'z')
		|| (ctx->str[*ctx->i] >= 'A' && ctx->str[*ctx->i] <= 'Z')
		|| (ctx->str[*ctx->i] >= '0' && ctx->str[*ctx->i] <= '9')))
		ctx->result = handle_env_var(ctx, shell);
	else
		ctx->result[(*ctx->j)++] = '$';
	return (ctx->result);
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
			if (!expanded)
				return ;
			free(current_redir->file);
			current_redir->file = expanded;
		}
		current_redir = current_redir->next;
	}
}

static char	**expand_wildcard_args(char **args)
{
	char	**result;
	char	**matches;
	int		total_count;
	int		result_index;
	int		i;
	int		j;

	total_count = 0;
	i = 0;
	while (args[i])
	{
		if (has_wildcard(args[i]))
		{
			matches = expand_wildcard(args[i]);
			if (matches)
			{
				j = 0;
				while (matches[j])
				{
					total_count++;
					j++;
				}
				free_args(matches);
			}
			else
				total_count++;
		}
		else
			total_count++;
		i++;
	}
	result = malloc(sizeof(char *) * (total_count + 1));
	if (!result)
		return (NULL);
	result_index = 0;
	i = 0;
	while (args[i])
	{
		if (has_wildcard(args[i]))
		{
			matches = expand_wildcard(args[i]);
			if (matches)
			{
				sort_matches(matches);
				j = 0;
				while (matches[j])
				{
					result[result_index] = ft_strdup(matches[j]);
					result_index++;
					j++;
				}
				free_args(matches);
			}
			else
			{
				result[result_index] = ft_strdup(args[i]);
				result_index++;
			}
		}
		else
		{
			result[result_index] = ft_strdup(args[i]);
			result_index++;
		}
		i++;
	}
	result[result_index] = NULL;
	return (result);
}

void	expand_args(t_command *cmd, t_shell *shell)
{
	int		i;
	int		j;
	char	*expanded;
	char	**new_args;
	char	**wildcard_expanded;
	int		count;

	if (!cmd || !cmd->args)
		return ;
	i = 0;
	while (cmd->args[i])
	{
		expanded = expand_env_vars(cmd->args[i], shell);
		if (!expanded)
		{
			free_args(cmd->args);
			cmd->args = NULL;
			return ;
		}
		free(cmd->args[i]);
		cmd->args[i] = expanded;
		i++;
	}
	wildcard_expanded = expand_wildcard_args(cmd->args);
	if (!wildcard_expanded)
	{
		free_args(cmd->args);
		cmd->args = NULL;
		return ;
	}
	free_args(cmd->args);
	cmd->args = wildcard_expanded;
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
	{
		free_args(cmd->args);
		cmd->args = NULL;
		return ;
	}
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
