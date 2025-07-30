/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_manager.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 00:00:00 by mdusunen          #+#    #+#             */
/*   Updated: 2025/07/30 17:49:55 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

/**
 * Returns the pointer for the specified GC type
 */
void *gc_get_static_ptr(t_shell *shell, t_gc_type type)
{
	if (!shell)
		return (NULL);
	if (type == GC_LINE)
		return (shell->gc.gc_line);
	else if (type == GC_PROCESSED_LINE)
		return (shell->gc.gc_processed_line);
	else if (type == GC_TOKENS)
		return (shell->gc.gc_tokens);
	else if (type == GC_COMMANDS)
		return (shell->gc.gc_commands);
	else if (type == GC_ARGS)
		return (shell->gc.gc_args);
	else if (type == GC_ENV_VAR)
		return (shell->gc.gc_env_var);
	else if (type == GC_PATH)
		return (shell->gc.gc_path);
	else if (type == GC_EXEC_PATH)
		return (shell->gc.gc_exec_path);
	else if (type == GC_HEREDOC)
		return (shell->gc.gc_heredoc);
	else if (type == GC_TEMP_STR)
		return (shell->gc.gc_temp_str);
	else if (type == GC_ENV_ARRAY)
		return (shell->gc.gc_env_array);
	else if (type == GC_REDIR)
		return (shell->gc.gc_redir);
	else if (type == GC_GENERAL)
		return (shell->gc.gc_general);
	return (NULL);
}

/**
 * Sets the pointer for the specified GC type
 */
void gc_set_static_ptr(t_shell *shell, void *ptr, t_gc_type type)
{
	if (!shell)
		return ;
	if (type == GC_LINE)
		shell->gc.gc_line = ptr;
	else if (type == GC_PROCESSED_LINE)
		shell->gc.gc_processed_line = ptr;
	else if (type == GC_TOKENS)
		shell->gc.gc_tokens = ptr;
	else if (type == GC_COMMANDS)
		shell->gc.gc_commands = ptr;
	else if (type == GC_ARGS)
		shell->gc.gc_args = ptr;
	else if (type == GC_ENV_VAR)
		shell->gc.gc_env_var = ptr;
	else if (type == GC_PATH)
		shell->gc.gc_path = ptr;
	else if (type == GC_EXEC_PATH)
		shell->gc.gc_exec_path = ptr;
	else if (type == GC_HEREDOC)
		shell->gc.gc_heredoc = ptr;
	else if (type == GC_TEMP_STR)
		shell->gc.gc_temp_str = ptr;
	else if (type == GC_ENV_ARRAY)
		shell->gc.gc_env_array = ptr;
	else if (type == GC_REDIR)
		shell->gc.gc_redir = ptr;
	else if (type == GC_GENERAL)
		shell->gc.gc_general = ptr;
}

/**
 * Allocates memory and assigns it to the specified GC type
 */
void *gc_malloc(t_shell *shell, size_t size, t_gc_type type)
{
	void *ptr;

	if (!shell)
		return (malloc(size));
	
	// Free the old pointer first
	gc_free_type(shell, type);
	
	ptr = malloc(size);
	if (!ptr)
		return (NULL);
	
	// Assign new pointer to GC
	gc_set_static_ptr(shell, ptr, type);
	return (ptr);
}

/**
 * Creates a duplicate string with GC management
 */
char *gc_strdup(t_shell *shell, const char *s, t_gc_type type)
{
	char *new_str;
	size_t len;

	if (!s)
		return (NULL);
		
	len = ft_strlen(s);
	new_str = gc_malloc(shell, len + 1, type);
	if (!new_str)
		return (NULL);
		
	ft_strlcpy(new_str, s, len + 1);
	return (new_str);
}

/**
 * Joins two strings with GC management
 */
char *gc_strjoin(t_shell *shell, const char *s1, const char *s2, t_gc_type type)
{
	char *new_str;
	size_t len1, len2;

	if (!s1 || !s2)
		return (NULL);
		
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	new_str = gc_malloc(shell, len1 + len2 + 1, type);
	if (!new_str)
		return (NULL);
		
	ft_strlcpy(new_str, s1, len1 + 1);
	ft_strlcat(new_str, s2, len1 + len2 + 1);
	return (new_str);
}

/**
 * Allocates an array with GC management
 */
char **gc_malloc_array(t_shell *shell, size_t count, t_gc_type type)
{
	char **array;

	array = gc_malloc(shell, sizeof(char *) * count, type);
	return (array);
}

/**
 * Frees memory for a specific GC type
 */
void gc_free_type(t_shell *shell, t_gc_type type)
{
	void *ptr;

	if (!shell)
		return ;
	
	ptr = gc_get_static_ptr(shell, type);
	if (ptr)
	{
		free(ptr);
		// Set pointer to NULL
		gc_set_static_ptr(shell, NULL, type);
	}
}

/**
 * Frees all GC-managed memory
 */
void gc_free_all(t_shell *shell)
{
	if (!shell)
		return ;
	gc_free_type(shell, GC_LINE);
	gc_free_type(shell, GC_PROCESSED_LINE);
	gc_free_type(shell, GC_HEREDOC);
	gc_free_type(shell, GC_TEMP_STR);
	gc_free_type(shell, GC_GENERAL);
}
