/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pills_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yihakan <yihakan@student.42istanbul.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 05:10:00 by yihakan           #+#    #+#             */
/*   Updated: 2025/08/04 05:13:27 by yihakan          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	print_matrix_ascii(void)
{
	printf("\033[32m");
	printf("╔════════════════════════════════════╗\n");
	printf("║                                    ║\n");
	printf("║  ██╗  ██╗███████╗██╗     ██╗       ║\n");
	printf("║  ██║  ██║██╔════╝██║     ██║       ║\n");
	printf("║  ███████║█████╗  ██║     ██║       ║\n");
	printf("║  ██╔══██║██╔══╝  ██║     ██║       ║\n");
	printf("║  ██║  ██║███████╗███████╗███████╗  ║\n");
	printf("║  ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝  ║\n");
	printf("║                                    ║\n");
	printf("╚════════════════════════════════════╝\n");
	printf("\033[0m");
}

void	print_morpheus_quote(void)
{
	printf("\033[36m");
	printf("\n\"I'm going to show you how deep the rabbit hole goes...\"\n");
	printf("                    - Morpheus\n");
	printf("\033[0m");
	printf("\n\033[33mYou find yourself in a dimly lit room...\n");
	printf("A man in dark glasses offers you a choice:\n\n");
}

void	matrix_effect(void)
{
	const char	*lines[] = {
		"   $    @   #     %     &   *     $     @     #     %     &     *   ",
		"    @   #   %   &   *   $   @   #   %   &   *   $@   #   %   &   *",
		"      #     %     &     *     $     @     #     %     &     *   $ ",
		"  %   &   *   $   @   #%   &   *   $   @   #   %   &   *   $   @# ",
		"     &     *     $   @   #     %   &   *     $     @     #     ",
		"  *   $   @   #   %   &   *   $   @   #%   &   *   $   @   #   %   & ",
		"     $     @     #     %     &     *     $     @   %     &     ",
		"  @   #   %   &   *$   @   #   %   &   *   $   @   #   %   &   *   $ ",
		"     #     %     &     *     $     @     #     %     &     * $",
		"  %   &   *   $   @#   %   &   *   $   @ #   %   &   *   $  @   # "
	};
	int			i;

	i = 0;
	printf("\033[32m");
	while (i < 10)
	{
		printf("%s\n", lines[i]);
		usleep(100000);
		i++;
	}
	printf("\033[0m");
}
