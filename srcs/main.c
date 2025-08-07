/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/18 18:50:53 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/07 04:42:08 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	g_signal = 0;

static void	setup_terminal(void)
{
	struct termios	term;

	if (tcgetattr(STDIN_FILENO, &term) == -1)
		return ;
	term.c_lflag &= ~(ECHOCTL | ISIG);
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
		return ;
}

static void	handle_commands(t_command *commands, t_shell *shell)
{
	if (!commands)
		return ;
	expand_variables(commands, shell);
	execute_commands(commands, shell);
	clear_current_commands(shell);
}

static void	handle_tokens(char *processed_line, t_shell *shell)
{
	t_token		*tokens;
	t_command	*commands;

	tokens = tokenize(processed_line);
	commands = NULL;
	set_current_tokens(shell, tokens);
	if (tokens)
	{
		commands = parse(tokens);
		set_current_commands(shell, commands);
		handle_commands(commands, shell);
		clear_current_tokens(shell);
	}
}

static void	process_command_line(char *line, t_shell *shell)
{
	char	*processed_line;

	if (!*line)
		return ;
	add_to_history(line);
	processed_line = lexer_analyze(line);
	if (processed_line)
	{
		gc_set_static_ptr(shell, processed_line, GC_PROCESSED_LINE);
		handle_tokens(processed_line, shell);
		gc_free_type(shell, GC_PROCESSED_LINE);
	}
}

static void	handle_signal_interrupt(t_shell *shell)
{
	clear_current_tokens(shell);
	clear_current_commands(shell);
	gc_free_all(shell);
	g_signal = 0;
	shell->exit_status = 130;
}

static void	shell_loop(t_shell *shell)
{
    char *line;

    while (1)
    {
        line = display_prompt(shell);
        if (!line)
        {
            clear_current_tokens(shell);
            clear_current_commands(shell);
            gc_free_all(shell);
            break;
        }
        clear_current_tokens(shell);
        clear_current_commands(shell);
        gc_set_static_ptr(shell, line, GC_LINE);
        process_command_line(line, shell);
        gc_free_type(shell, GC_LINE);
        if (g_signal == SIGINT)
            handle_signal_interrupt(shell);
    }
    clear_current_tokens(shell);
    clear_current_commands(shell);
    gc_free_all(shell);
    free_shell(shell);
}

int	main(int argc, char **argv, char **env)
{
    t_shell	shell;

    (void)argc;
    (void)argv;
    setup_terminal();
    setup_signals();
    init_shell(&shell, env);
    shell_loop(&shell);
    return (shell.exit_status);
}
