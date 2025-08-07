/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:19:51 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/22 17:56:18 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char	*ft_strjoin_free(char *s1, char *s2)
{
	char	*result;

	if (!s1 || !s2)
		return (s1);
	result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}

static char	*add_char_to_result(char *result, char c)
{
	char	temp[2];
	char	*new_result;

	temp[0] = c;
	temp[1] = '\0';
	new_result = ft_strjoin(result, temp);
	free(result);
	return (new_result);
}

static char	*handle_variable_expansion(char *str, int *i, t_shell *shell)
{
	char	var_name[256];
	char	*env_value;
	int		j;

	(*i)++;
	if (str[*i] == '?')
	{
		(*i)++;
		return (ft_itoa(shell->exit_status));
	}
	else if (str[*i] && (str[*i] == '_' || ft_isalnum(str[*i])))
	{
		j = 0;
		while (str[*i] && (str[*i] == '_' || ft_isalnum(str[*i])))
			var_name[j++] = str[(*i)++];
		var_name[j] = '\0';
		env_value = get_env_value(shell->env_list, var_name);
		if (env_value)
			return (ft_strdup(env_value));
		return (ft_strdup(""));
	}
	else
		return (ft_strdup("$"));
}

char	*expand_env_vars(char *str, t_shell *shell)
{
	char	*result;
	char	*temp;
	int		i;
	int		in_quotes;
	char	quote_char;

	result = ft_strdup("");
	if (!result)
		return (NULL);
	i = 0;
	in_quotes = 0;
	quote_char = 0;
	while (str[i])
	{
		if ((str[i] == '\'' || str[i] == '"')
			&& (!in_quotes || quote_char == str[i]))
		{
			if (in_quotes)
				in_quotes = 0;
			else
			{
				in_quotes = 1;
				quote_char = str[i];
			}
			i++;
			continue ;
		}
		if (str[i] == '$' && (!in_quotes || quote_char != '\''))
		{
			temp = handle_variable_expansion(str, &i, shell);
			if (temp)
			{
				result = ft_strjoin_free(result, temp);
				free(temp);
			}
		}
		else
		{
			result = add_char_to_result(result, str[i]);
			i++;
		}
	}
	return (result);
}

void	expand_variables(t_command *cmd, t_shell *shell)
{
	t_command	*current_cmd;

	current_cmd = cmd;
	while (current_cmd)
	{
		expand_args(current_cmd, shell);
		expand_redirections(current_cmd, shell);
		current_cmd = current_cmd->next;
	}
}
