/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:35:36 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/22 17:56:18 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static char	*ft_strndup(const char *s, size_t n)
{
	char	*dup;
	size_t	i;

	dup = malloc(n + 1);
	if (!dup)
		return (NULL);
	i = 0;
	while (i < n)
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

static void	free_env_list(t_env *env_list)
{
	t_env	*current;

	while (env_list)
	{
		current = env_list;
		env_list = env_list->next;
		free(current->key);
		free(current->value);
		free(current);
	}
}

static t_env	*create_env_node(char *env_str)
{
	t_env	*new_node;
	char	*key;
	char	*value;
	int		j;

	j = 0;
	while (env_str[j] && env_str[j] != '=')
		j++;
	key = ft_strndup(env_str, j);
	if (!key)
		return (NULL);
	value = ft_strdup(env_str + j + 1);
	if (!value)
		return (free(key), NULL);
	new_node = malloc(sizeof(t_env));
	if (!new_node)
	{
		free(key);
		free(value);
		return (NULL);
	}
	new_node->key = key;
	new_node->value = value;
	new_node->next = NULL;
	return (new_node);
}

static void	add_node_to_list(t_env **env_list, t_env *new_node)
{
	t_env	*current;

	if (!*env_list)
		*env_list = new_node;
	else
	{
		current = *env_list;
		while (current->next)
			current = current->next;
		current->next = new_node;
	}
}

t_env	*create_env_list(char **env)
{
	t_env	*env_list;
	t_env	*new_node;
	int		i;

	if (!env)
		return (NULL);
	env_list = NULL;
	i = 0;
	while (env[i])
	{
		new_node = create_env_node(env[i]);
		if (!new_node)
		{
			free_env_list(env_list);
			return (NULL);
		}
		add_node_to_list(&env_list, new_node);
		i++;
	}
	return (env_list);
}
