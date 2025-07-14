/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/09 19:15:29 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/10 19:36:39 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"
#include <string.h>

static char	*str_ndup(const char *s, size_t n)
{
	char	*dup;
	size_t	len;

	len = ft_strlen(s);
	if (n < len)
		len = n;
	dup = malloc(len + 1);
	if (!dup)
		return (NULL);
	ft_strlcpy(dup, s, len + 1);
	return (dup);
}

static int	is_valid_env_char(char c, int first_char)
{
	if (first_char)
		return (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
	return (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
		|| (c >= '0' && c <= '9'));
}

static int	is_valid_env_name(const char *name)
{
	int	i;

	if (!name || !*name)
		return (0);
	if (!is_valid_env_char(name[0], 1))
		return (0);
	i = 1;
	while (name[i])
	{
		if (name[i] == '=')
			break ;
		if (!is_valid_env_char(name[i], 0))
			return (0);
		i++;
	}
	return (1);
}

static void	print_export_error(char *arg)
{
	ft_putstr_fd("minishell: export: `", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}

static void	print_exported_vars(t_env *env)
{
	while (env)
	{
		printf("declare -x %s=\"%s\"\n", env->key, env->value);
		env = env->next;
	}
}

static int	handle_export_arg(char *arg, t_shell *shell)
{
	int		j;
	char	*key;
	char	*value;

	j = 0;
	while (arg[j] && arg[j] != '=')
		j++;
	key = str_ndup(arg, j);
	if (!is_valid_env_name(key))
	{
		print_export_error(arg);
		free(key);
		shell->exit_status = 1;
		return (1);
	}
	if (arg[j] == '=')
	{
		value = ft_strdup(arg + j + 1);
		shell->env_list = add_env_var(shell->env_list, key, value);
		free(value);
	}
	else if (!get_env_value(shell->env_list, key))
		add_env_var(shell->env_list, key, "");
	free(key);
	return (0);
}

int	ft_export(char **args, t_shell *shell)
{
	int	i;
	int	ret;

	if (!args[1])
	{
		print_exported_vars(shell->env_list);
		return (0);
	}
	i = 1;
	ret = 0;
	while (args[i])
	{
		ret = handle_export_arg(args[i], shell);
		if (ret != 0)
		{
			free(shell->env_array);
			shell->env_array = env_list_to_array(shell->env_list);
			return (ret);
		}
		i++;
	}
	i = 0;
	while(shell->env_array[i])
		free(shell->env_array[i++]);
	free(shell->env_array);
	shell->env_array = env_list_to_array(shell->env_list);
	return (ret);
}
