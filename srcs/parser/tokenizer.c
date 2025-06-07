# include "../includes/minishell.h"

/**
 * Create a new token node
 */
static t_token    *create_token(t_token_type type, char *value)
{
    t_token    *token;

    token = malloc(sizeof(t_token));
    if (!token)
        return (NULL);
    token->type = type;
    token->value = value;
    token->next = NULL;
    return (token);
}

/**
 * Add a token to the end of the token list
 */
static void    add_token(t_token **tokens, t_token *new_token)
{
    t_token    *current;

    if (!*tokens)
    {
        *tokens = new_token;
        return;
    }
    current = *tokens;
    while (current->next)
        current = current->next;
    current->next = new_token;
}

/**
 * Check if character is a whitespace
 */
static int    is_whitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n');
}

/**
 * Tokenize the input string
 */
t_token    *tokenize(char *input)
{
    t_token    *tokens;
    int        i;
    int        start;

    tokens = NULL;
    i = 0;
    while (input[i])
    {
        while (input[i] && is_whitespace(input[i]))
            i++;
        if (!input[i])
            break;
        
        if (input[i] == '|')
        {
            add_token(&tokens, create_token(TOKEN_PIPE, strdup("|")));
            i++;
        }
        else if (input[i] == '<')
        {
            if (input[i + 1] == '<')
            {
                add_token(&tokens, create_token(TOKEN_HEREDOC, strdup("<<")));
                i += 2;
            }
            else
            {
                add_token(&tokens, create_token(TOKEN_REDIR_IN, strdup("<")));
                i++;
            }
        }
        else if (input[i] == '>')
        {
            if (input[i + 1] == '>')
            {
                add_token(&tokens, create_token(TOKEN_REDIR_APPEND, strdup(">>")));
                i += 2;
            }
            else
            {
                add_token(&tokens, create_token(TOKEN_REDIR_OUT, strdup(">")));
                i++;
            }
        }
        else
        {
            start = i;
            while (input[i] && !is_whitespace(input[i]) && 
                   input[i] != '|' && input[i] != '<' && input[i] != '>')
            {
                if (input[i] == '\'' || input[i] == '"')
                {
                    char quote = input[i++];
                    while (input[i] && input[i] != quote)
                        i++;
                    if (input[i])
                        i++;
                }
                else
                    i++;
            }
            add_token(&tokens, create_token(TOKEN_WORD, 
                      strndup(input + start, i - start)));
        }
    }
    return (tokens);
}

/**
 * Free all tokens in the token list
 */
void    free_tokens(t_token *tokens)
{
    t_token    *current;
    t_token    *next;

    current = tokens;
    while (current)
    {
        next = current->next;
        free(current->value);
        free(current);
        current = next;
    }
}
