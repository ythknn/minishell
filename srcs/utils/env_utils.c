/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/15 18:35:36 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/21 17:51:42 by mdusunen         ###   ########.fr       */
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

static char	*envjoin(char *s1, char *s2)
{
	char	*result;
	int		i;
	int		j;
	size_t	total_len;

	if (!s1 || !s2)
		return (NULL);
	i = 0;
	total_len = ft_strlen(s1) + ft_strlen(s2);
	result = (char *)malloc((total_len * sizeof(char)) + 1);
	if (result == NULL)
		return (NULL);
	while (s1[i] != '\0')
	{
		result[i] = ((unsigned char *)s1)[i];
		i++;
	}
	j = 0;
	while (s2[j] != '\0')
		result[i++] = ((unsigned char *)s2)[j++];
	result[i] = '\0';
	if (s1)
		free(s1);
	return (result);
}

static char	**double_join(char **doublebase, char *tojoin)
{
	size_t	len;
	char	**ret;
	size_t	i;

	len = 0;
	while (doublebase && doublebase[len])
		len++;
	ret = malloc(sizeof(char *) * (len + 2));
	i = 0;
	while (doublebase && doublebase[i])
	{
		ret[i] = ft_strdup(doublebase[i]);
		free(doublebase[i]);
		doublebase[i] = NULL;
		i++;
	}
	ret[i] = ft_strdup(tojoin);
	i++;
	ret[i] = NULL;
	if (doublebase)
		free(doublebase);
	if (tojoin)
		free(tojoin);
	return (ret);
}

char	**env_list_to_array(t_env *env_list)
{
	char	**env_array;
	t_env	*current;
	char	*tojoin;

	env_array = NULL;
	current = env_list;
	while (current)
	{
		tojoin = ft_strjoin(current->key, "=");
		tojoin = envjoin(tojoin, current->value);
		env_array = double_join(env_array, tojoin);
		current = current->next;
	}
	return (env_array);
}

t_env	*add_env_var(t_env *env_list, char *key, char *value)
{
	t_env	*current;
	t_env	*new_node;

	current = env_list;
	while (current)
	{
		if (strcmp(current->key, key) == 0)
		{
			free(current->value);
			current->value = strdup(value);
			return (env_list);
		}
		current = current->next;
	}
	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return (env_list);
	new_node->key = strdup(key);
	new_node->value = strdup(value);
	new_node->next = env_list;
	return (new_node);
}

void	remove_env_var(t_env **env_list, char *key)
{
	t_env	*current;
	t_env	*prev;

	if (!*env_list)
		return ;
	if (strcmp((*env_list)->key, key) == 0)
	{
		current = *env_list;
		*env_list = (*env_list)->next;
		free(current->key);
		free(current->value);
		free(current);
		return ;
	}
	prev = *env_list;
	current = (*env_list)->next;
	while (current)
	{
		if (strcmp(current->key, key) == 0)
		{
			prev->next = current->next;
			free(current->key);
			free(current->value);
			free(current);
			return ;
		}
		prev = current;
		current = current->next;
	}
}

char	*get_env_value(t_env *env_list, char *key)
{
	t_env	*current;

	current = env_list;
	while (current)
	{
		if (strcmp(current->key, key) == 0)
			return (current->value);
		current = current->next;
	}
	return (NULL);
}
