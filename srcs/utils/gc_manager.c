/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gc_manager.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 00:00:00 by mdusunen          #+#    #+#             */
/*   Updated: 2025/08/04 04:35:56 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void	*gc_get_static_ptr(t_shell *shell, t_gc_type type)
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
	else if (type == GC_ENV_VAR)
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

void	gc_set_static_ptr(t_shell *shell, void *ptr, t_gc_type type)
{
	if (type == GC_LINE)
		shell->gc_line = ptr;
	else if (type == GC_PROCESSED_LINE)
		shell->gc_processed_line = ptr;
	else if (type == GC_TOKENS)
		shell->gc_tokens = ptr;
	else if (type == GC_COMMANDS)
		shell->gc_commands = ptr;
	else if (type == GC_ARGS)
		shell->gc_args = ptr;
	else if (type == GC_ENV_VAR)
		shell->gc_env_var = ptr;
	else if (type == GC_PATH)
		shell->gc_path = ptr;
	else if (type == GC_EXEC_PATH)
		shell->gc_exec_path = ptr;
	else if (type == GC_HEREDOC)
		shell->gc_heredoc = ptr;
	else if (type == GC_TEMP_STR)
		shell->gc_temp_str = ptr;
	else if (type == GC_ENV_ARRAY)
		shell->gc_env_array = ptr;
	else if (type == GC_REDIR)
		shell->gc_redir = ptr;
	else if (type == GC_GENERAL)
		shell->gc_general = ptr;
}

void	gc_free_all(t_shell *shell)
{
	gc_free_type(shell, GC_LINE);
	gc_free_type(shell, GC_PROCESSED_LINE);
	gc_free_type(shell, GC_HEREDOC);
	gc_free_type(shell, GC_TEMP_STR);
	gc_free_type(shell, GC_GENERAL);
}
