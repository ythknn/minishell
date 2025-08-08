/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_manager.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 00:00:00 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/08 21:26:49 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

static void	*gc_get_static_ptr_part1(t_shell *shell, t_gc_type type);
static void	*gc_get_static_ptr_part2(t_shell *shell, t_gc_type type);

static void	*gc_get_static_ptr_part1(t_shell *shell, t_gc_type type)
{
	if (type == GC_LINE)
		return (shell->gc_line);
	else if (type == GC_PROCESSED_LINE)
		return (shell->gc_processed_line);
	else if (type == GC_TOKENS)
		return (shell->gc_tokens);
	else if (type == GC_COMMANDS)
		return (shell->gc_commands);
	else if (type == GC_ARGS)
		return (shell->gc_args);
	return (gc_get_static_ptr_part2(shell, type));
}

static void	*gc_get_static_ptr_part2(t_shell *shell, t_gc_type type)
{
	if (type == GC_ENV_VAR)
		return (shell->gc_env_var);
	else if (type == GC_PATH)
		return (shell->gc_path);
	else if (type == GC_EXEC_PATH)
		return (shell->gc_exec_path);
	else if (type == GC_HEREDOC)
		return (shell->gc_heredoc);
	else if (type == GC_TEMP_STR)
		return (shell->gc_temp_str);
	else if (type == GC_ENV_ARRAY)
		return (shell->gc_env_array);
	else if (type == GC_REDIR)
		return (shell->gc_redir);
	else if (type == GC_GENERAL)
		return (shell->gc_general);
	return (NULL);
}

void	*gc_get_static_ptr(t_shell *shell, t_gc_type type)
{
	return (gc_get_static_ptr_part1(shell, type));
}

void	gc_free_all(t_shell *shell)
{
	gc_free_type(shell, GC_LINE);
	gc_free_type(shell, GC_PROCESSED_LINE);
	gc_free_type(shell, GC_HEREDOC);
	gc_free_type(shell, GC_TEMP_STR);
	gc_free_type(shell, GC_GENERAL);
}
