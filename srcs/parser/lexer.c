# include "../includes/minishell.h"

/**
 * Lexical analysis of input string
 */
char    *lexer_analyze(char *input)
{
    char    *processed;
    int     i;
    int     j;
    int     in_quotes;
    char    quote_char;

    if (!input)
        return (NULL);
    
    processed = malloc(strlen(input) * 2 + 1);
    if (!processed)
        return (NULL);
    
    i = 0;
    j = 0;
    in_quotes = 0;
    quote_char = 0;
    
    while (input[i])
    {
        if (!in_quotes && (input[i] == '\'' || input[i] == '"'))
        {
            in_quotes = 1;
            quote_char = input[i];
            processed[j++] = input[i++];
        }
        else if (in_quotes && input[i] == quote_char)
        {
            in_quotes = 0;
            processed[j++] = input[i++];
        }
        else if (!in_quotes && (input[i] == '|' || input[i] == '<' || input[i] == '>'))
        {
            // Add spaces around operators if not already present
            if (j > 0 && processed[j - 1] != ' ')
                processed[j++] = ' ';
            
            processed[j++] = input[i++];
            
            // Handle << and >> operators
            if (input[i - 1] == '<' && input[i] == '<')
                processed[j++] = input[i++];
            else if (input[i - 1] == '>' && input[i] == '>')
                processed[j++] = input[i++];
            
            if (input[i] && input[i] != ' ')
                processed[j++] = ' ';
        }
        else
        {
            processed[j++] = input[i++];
        }
    }
    
    processed[j] = '\0';
    return (processed);
}
