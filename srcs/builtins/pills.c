/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pills.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 04:50:00 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/04 05:14:28 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static void	red_pill_sequence(void)
{
	printf("\033[31m");
	printf("\n🌌 The Matrix is collapsing...\n");
	usleep(500000);
	printf("🔴 Reality is breaking apart...\n");
	usleep(500000);
	printf("⚡ You are being unplugged...\n");
	usleep(500000);
	printf("🚪 Welcome to the real world...\n");
	printf("\033[0m");
	usleep(1000000);
}

static void	blue_pill_sequence(void)
{
	printf("\033[34m");
	printf("\n💊 The blue pill takes effect...\n");
	usleep(500000);
	printf("😴 You feel drowsy...\n");
	usleep(500000);
	printf("🌙 Returning to blissful ignorance...\n");
	usleep(500000);
	printf("💭 The story ends, you wake up in your bed...\n");
	printf("\033[0m");
}

static int	handle_pill_choice(const char *input, t_shell *shell)
{
	if (ft_strcmp(input, "red") == 0)
	{
		matrix_effect();
		red_pill_sequence();
		printf("\033[32mYou chose the red pill. Exiting the Matrix.\n\033[0m");
		clear_current_tokens(shell);
		clear_current_commands(shell);
		gc_free_all(shell);
		free_shell(shell);
		exit(0);
	}
	else if (ft_strcmp(input, "blue") == 0)
	{
		blue_pill_sequence();
		printf("\033[34mYou chose the blue pill. The story ends.\n\033[0m");
		return (0);
	}
	printf("\033[31mInvalid choice. Please type 'red' or 'blue'.\n\033[0m");
	return (1);
}

int	ft_pills(char **args, t_shell *shell)
{
	char	*input;
	int		result;

	(void)args;
	print_matrix_ascii();
	print_morpheus_quote();
	printf("🔴 The RED pill: See how deep the rabbit hole goes\n");
	printf("🔵 The BLUE pill: Return to your normal life\n\n\033[0m");
	while (1)
	{
		input = readline("\033[35mChoose your destiny (red/blue): \033[0m");
		if (!input)
		{
			printf("\n\033[33mYou hesitate... the moment passes.\n\033[0m");
			return (0);
		}
		if (ft_strcmp(input, "red") == 0 || ft_strcmp(input, "blue") == 0)
		{
			result = handle_pill_choice(input, shell);
			return (free(input), result);
		}
		handle_pill_choice(input, shell);
		free(input);
	}
	return (0);
}
