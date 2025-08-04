/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:15:29 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/03 10:56:32 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <string.h>

static void	add_env_variable(char *arg, char *key, t_shell *shell)
{
	char	*value;

	if (arg[ft_strlen(key)] == '=')
	{
		value = ft_strdup(arg + ft_strlen(key) + 1);
		shell->env_list = add_env_var(shell->env_list, key, value);
		free(value);
	}
	else if (!get_env_value(shell->env_list, key))
		shell->env_list = add_env_var(shell->env_list, key, "");
}

static int	process_export_arg(char *arg, t_shell *shell)
{
	char	*key;

	key = extract_key(arg);
	if (!key)
		return (1);
	if (!is_valid_env_name(key))
	{
		print_export_error(arg);
		free(key);
		shell->exit_status = 1;
		return (1);
	}
	add_env_variable(arg, key, shell);
	free(key);
	return (0);
}

static void	refresh_env_array(t_shell *shell)
{
	int	i;

	if (shell->env_array)
	{
		i = 0;
		while (shell->env_array[i])
			free(shell->env_array[i++]);
		free(shell->env_array);
	}
	shell->env_array = env_list_to_array(shell->env_list);
}

int	ft_export(char **args, t_shell *shell)
{
	int	i;
	int	ret;

	if (!args[1])
		return (print_exported_vars(shell->env_list), 0);
	i = 1;
	ret = 0;
	while (args[i])
	{
		ret = process_export_arg(args[i], shell);
		if (ret != 0)
			break ;
		i++;
	}
	refresh_env_array(shell);
	return (ret);
}
