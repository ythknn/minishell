/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdusunen <mdusunen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 18:50:53 by yihakan           #+#    #+#             */
/*   Updated: 2025/07/28 17:29:10 by mdusunen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	g_signal = 0;

void	cleanup_on_exit(void)
{
	clear_current_tokens();
	clear_current_commands();
	gc_free_all();
}

static void	process_input_line(char *line, t_shell *shell)
{
	char	*processed_line;

	if (*line)
	{
		add_to_history(line);
		processed_line = lexer_analyze(line);
		process_lexed_line(processed_line, shell);
	}
}

static void	cleanup_and_exit(void)
{
	clear_current_tokens();
	clear_current_commands();
	gc_free_all();
}

static int	handle_prompt_input(t_shell *shell)
{
	char	*line;

	line = display_prompt();
	if (!line)
	{
		cleanup_and_exit();
		return (0);
	}
	clear_current_tokens();
	clear_current_commands();
	gc_set_static_ptr(line, GC_LINE);
	process_input_line(line, shell);
	gc_free_type(GC_LINE);
	if (g_signal == SIGINT)
		handle_sigint_cleanup(shell);
	return (1);
}

int	main(int argc, char **argv, char **env)
{
	t_shell	shell;

	(void)argc;
	(void)argv;
	init_minishell(&shell, env);
	while (handle_prompt_input(&shell))
		continue ;
	cleanup_and_exit();
	gc_free_all();
	free_shell(&shell);
	return (shell.exit_status);
}
