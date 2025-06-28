# include "../includes/minishell.h"

char	*expand_env_vars(char *str, t_shell *shell)
{
	char	*result;
	int		i;
	int		j;
	int		in_quotes;
	char	quote_char;

	result = malloc(4096);
	if (!result)
		return (NULL);
	i = 0;
	j = 0;
	in_quotes = 0;
	quote_char = 0;
	while (str[i])
	{
		if ((str[i] == '\'' || str[i] == '"') && (!in_quotes || quote_char == str[i]))
		{
			if (in_quotes)
				in_quotes = 0;
			else
			{
				in_quotes = 1;
				quote_char = str[i];
			}
			i++;
			continue;
		}
		if (str[i] == '$' && (!in_quotes || quote_char != '\''))
			result = handle_dollar_sign(str, &i, result, &j, shell);
		else
			result[j++] = str[i++];
	}
	result[j] = '\0';
	return (result);
}

void	expand_variables(t_command *cmd, t_shell *shell)
{
	t_command	*current_cmd;

	current_cmd = cmd;
	while (current_cmd)
	{
		expand_args(current_cmd, shell);
		expand_redirections(current_cmd, shell);
		current_cmd = current_cmd->next;
	}
}
