#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Counters
TOTAL_TESTS=0
LEAKED_TESTS=0
FAILED_TESTS=0

# Create necessary directories and files
mkdir -p outfiles test_files
touch test_files/infile
echo -e "hello\nhi\nworld" > test_files/infile
echo -e "$(cat /etc/passwd)" > test_files/infile_big
echo "test content" > "test_files/file name with spaces"
touch test_files/invalid_permission
chmod 000 test_files/invalid_permission

# Create additional test files
echo -e "line1\nline2\nline3\nEOF" > test_files/heredoc_test
echo -e "#!/bin/bash\necho 'executable script'" > test_files/script.sh
chmod +x test_files/script.sh
echo "very long content for testing large file operations and memory management" > test_files/large_content
for i in {1..100}; do echo "Line $i with some content to make it longer" >> test_files/large_content; done

# Enable leak detection
export MallocStackLogging=1

echo "🧪 Starting Memory Leak Tests for Minishell"
echo "============================================="

# Function to run a test and check for leaks
run_test() {
    local test_num=$1
    local test_cmd=$2
    local description=$3
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo -n "Test $test_num: $description... "
    
    # Run the command
    if [ -z "$test_cmd" ]; then
        # Empty command test
        echo "" | timeout 5 ./minishell >/dev/null 2>&1
    else
        echo "$test_cmd" | timeout 5 ./minishell >/dev/null 2>&1
    fi
    
    # Check for leaks
    LEAK_OUTPUT=$(leaks minishell 2>/dev/null)
    
    if echo "$LEAK_OUTPUT" | grep -q "0 leaks for 0 total leaked bytes"; then
        echo -e "${GREEN}✅ No leaks${NC}"
    elif echo "$LEAK_OUTPUT" | grep -q "leaks for.*total leaked bytes"; then
        echo -e "${RED}❌ LEAK DETECTED${NC}"
        echo "$LEAK_OUTPUT" | grep "leaks for.*total leaked bytes"
        LEAKED_TESTS=$((LEAKED_TESTS + 1))
    else
        echo -e "${YELLOW}⚠️  Could not determine leak status${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

# Function to run heredoc tests (special handling)
run_heredoc_test() {
    local test_num=$1
    local heredoc_content=$2
    local description=$3
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo -n "Test $test_num: $description... "
    
    # Create temporary heredoc test
    echo "$heredoc_content" | timeout 5 ./minishell >/dev/null 2>&1
    
    # Check for leaks
    LEAK_OUTPUT=$(leaks minishell 2>/dev/null)
    
    if echo "$LEAK_OUTPUT" | grep -q "0 leaks for 0 total leaked bytes"; then
        echo -e "${GREEN}✅ No leaks${NC}"
    elif echo "$LEAK_OUTPUT" | grep -q "leaks for.*total leaked bytes"; then
        echo -e "${RED}❌ LEAK DETECTED${NC}"
        echo "$LEAK_OUTPUT" | grep "leaks for.*total leaked bytes"
        LEAKED_TESTS=$((LEAKED_TESTS + 1))
    else
        echo -e "${YELLOW}⚠️  Could not determine leak status${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

# Basic echo tests
run_test 1 'echo hello world' 'echo hello world'
run_test 2 'echo "hello world"' 'echo "hello world"'
run_test 3 "echo 'hello world'" "echo 'hello world'"
run_test 4 "echo hello'world'" "echo hello'world'"
run_test 5 'echo hello""world' 'echo hello""world'
run_test 6 "echo ''" "echo ''"
run_test 7 'echo "$PWD"' 'echo "$PWD"'
run_test 8 'echo '\''$PWD'\''' 'echo '\''$PWD'\'''
run_test 9 'echo "aspas ->'"'"'"' 'echo "aspas ->'"'"'"'
run_test 10 'echo "aspas -> '"'"' "' 'echo "aspas -> '"'"' "'
run_test 11 'echo '\''aspas ->"'\''' 'echo '\''aspas ->"'\'''
run_test 12 'echo '\''aspas -> " '\''' 'echo '\''aspas -> " '\'''
run_test 13 'echo "> >> < * ? [ ] | ; [ ] || && ( ) & # $  <<"' 'echo special chars'
run_test 14 'echo '\''> >> < * ? [ ] | ; [ ] || && ( ) & # $  <<'\''' 'echo special chars quoted'
run_test 15 'echo "exit_code ->$? user ->$USER home -> $HOME"' 'echo env vars'
run_test 16 'echo '\''exit_code ->$? user ->$USER home -> $HOME'\''' 'echo env vars quoted'
run_test 17 'echo "$"' 'echo "$"'
run_test 18 'echo '\''$'\''' 'echo '\''$'\'''
run_test 19 'echo $' 'echo $'
run_test 20 'echo $?' 'echo $?'
run_test 21 'echo $?HELLO' 'echo $?HELLO'

# PWD and builtins
run_test 22 'pwd' 'pwd'
run_test 23 'pwd oi' 'pwd oi'

# Export tests
run_test 24 'export hello' 'export hello'
run_test 25 'export HELLO=123' 'export HELLO=123'
run_test 26 'export A-' 'export A-'
run_test 27 'export HELLO=123 A' 'export HELLO=123 A'
run_test 28 'export HELLO="123 A-"' 'export HELLO="123 A-"'
run_test 29 'export hello world' 'export hello world'
run_test 30 'export HELLO-=123' 'export HELLO-=123'
run_test 31 'export =' 'export ='
run_test 32 'export 123' 'export 123'

# Unset tests
run_test 33 'unset' 'unset'
run_test 34 'unset HELLO' 'unset HELLO'
run_test 35 'unset HELLO1 HELLO2' 'unset HELLO1 HELLO2'
run_test 36 'unset HOME' 'unset HOME'
run_test 37 'unset PATH' 'unset PATH'
run_test 38 'unset SHELL' 'unset SHELL'

# CD tests
run_test 39 'cd $PWD' 'cd $PWD'
run_test 40 'cd $PWD hi' 'cd $PWD hi'
run_test 41 'cd 123123' 'cd 123123'

# Exit tests
run_test 42 'exit 123' 'exit 123'
run_test 43 'exit 298' 'exit 298'
run_test 44 'exit +100' 'exit +100'
run_test 45 'exit "+100"' 'exit "+100"'
run_test 46 'exit +"100"' 'exit +"100"'
run_test 47 'exit -100' 'exit -100'
run_test 48 'exit "-100"' 'exit "-100"'
run_test 49 'exit -"100"' 'exit -"100"'
run_test 50 'exit hello' 'exit hello'
run_test 51 'exit 42 world' 'exit 42 world'
run_test 52 '' 'empty command'

echo ""
echo "———————————— pipes"

# Pipe tests
run_test 53 'env | sort | grep -v SHLVL | grep -v ^_' 'env pipes'
run_test 54 'cat ./test_files/infile_big | grep oi' 'cat pipe grep'
run_test 55 'cat minishell.h | grep ");"$' 'cat pipe grep pattern'
run_test 56 'export GHOST=123 | env | grep GHOST' 'export pipe'

echo ""
echo "———————————— redirects"

# Redirection tests
run_test 57 'grep hi <./test_files/infile' 'input redirect'
run_test 58 'grep hi "<infile" <         ./test_files/infile' 'multiple input redirects'
run_test 59 'echo hi < ./test_files/infile bye bye' 'input redirect with args'
run_test 60 'grep hi <./test_files/infile_big <./test_files/infile' 'multiple input files'
run_test 61 'echo <"./test_files/infile" "bonjour       42"' 'input redirect quoted'
run_test 62 'cat <"./test_files/file name with spaces"' 'input redirect spaces'
run_test 63 'cat <./test_files/infile_big ./test_files/infile' 'input redirect with file arg'
run_test 64 'cat <"1""2""3""4""5"' 'input redirect concatenated'
run_test 65 'echo <"./test_files/infile" <missing <"./test_files/infile"' 'input redirect with missing'
run_test 66 'echo <missing <"./test_files/infile" <missing' 'missing files'
run_test 67 'cat <"./test_files/infile"' 'cat input redirect'
run_test 68 'echo <"./test_files/infile_big" | cat <"./test_files/infile"' 'pipe with input redirects'
run_test 69 'echo <"./test_files/infile_big" | cat "./test_files/infile"' 'pipe mixed'
run_test 70 'echo <"./test_files/infile_big" | echo <"./test_files/infile"' 'echo pipe echo'
run_test 71 'echo hi | cat <"./test_files/infile"' 'pipe with input redirect'
run_test 72 'echo hi | cat "./test_files/infile"' 'pipe with file arg'
run_test 73 'cat <"./test_files/infile" | echo hi' 'input redirect pipe'
run_test 74 'cat <"./test_files/infile" | grep hello' 'input redirect pipe grep'
run_test 75 'cat <"./test_files/infile_big" | echo hi' 'big file pipe'
run_test 76 'cat <missing' 'cat missing file'
run_test 77 'cat <missing | cat' 'missing file pipe'
run_test 78 'cat <missing | echo oi' 'missing file pipe echo'
run_test 79 'cat <missing | cat <"./test_files/infile"' 'missing and valid files'
run_test 80 'echo <123 <456 hi | echo 42' 'numeric redirects'

# Output redirection tests
run_test 81 'ls >./outfiles/outfile01' 'output redirect'
run_test 82 'ls >         ./outfiles/outfile01' 'output redirect spaced'
run_test 83 'echo hi >         ./outfiles/outfile01 bye' 'output redirect with args'
run_test 84 'ls >./outfiles/outfile01 >./outfiles/outfile02' 'multiple output redirects'
run_test 85 'ls >./outfiles/outfile01 >./test_files/invalid_permission' 'output to invalid file'
run_test 86 'ls >"./outfiles/outfile with spaces"' 'output to file with spaces'
run_test 87 'ls >"./outfiles/outfile""1""2""3""4""5"' 'output concatenated filename'
run_test 88 'ls >"./outfiles/outfile01" >./test_files/invalid_permission >"./outfiles/outfile02"' 'mixed output redirects'
run_test 89 'ls >./test_files/invalid_permission >"./outfiles/outfile01" >./test_files/invalid_permission' 'invalid permissions'
run_test 90 'cat <"./test_files/infile" >"./outfiles/outfile01"' 'input and output redirect'
run_test 91 'echo hi >./outfiles/outfile01 | echo bye' 'output redirect pipe'
run_test 92 'echo hi >./outfiles/outfile01 >./outfiles/outfile02 | echo bye' 'multiple outputs pipe'
run_test 93 'echo hi | echo >./outfiles/outfile01 bye' 'pipe output redirect'
run_test 94 'echo hi | echo bye >./outfiles/outfile01 >./outfiles/outfile02' 'pipe multiple outputs'
run_test 95 'echo hi >./outfiles/outfile01 | echo bye >./outfiles/outfile02' 'both sides output'
run_test 96 'echo hi >./outfiles/outfile01 >./test_files/invalid_permission | echo bye' 'output invalid pipe'
run_test 97 'echo hi >./test_files/invalid_permission | echo bye' 'invalid output pipe'
run_test 98 'echo hi >./test_files/invalid_permission >./outfiles/outfile01 | echo bye' 'mixed invalid output'
run_test 99 'echo hi | echo bye >./test_files/invalid_permission' 'pipe to invalid'
run_test 100 'echo hi | >./outfiles/outfile01 echo bye >./test_files/invalid_permission' 'complex redirects'
run_test 101 'echo hi | echo bye >./test_files/invalid_permission >./outfiles/outfile01' 'invalid then valid'
run_test 102 'cat <"./test_files/infile" >./test_files/invalid_permission' 'input to invalid output'
run_test 103 'cat >./test_files/invalid_permission <"./test_files/infile"' 'invalid output input'
run_test 104 'cat <missing >./outfiles/outfile01' 'missing input valid output'
run_test 105 'cat >./outfiles/outfile01 <missing' 'valid output missing input'
run_test 106 'cat <missing >./test_files/invalid_permission' 'missing input invalid output'
run_test 107 'cat >./test_files/invalid_permission <missing' 'invalid output missing input'
run_test 108 'cat >./outfiles/outfile01 <missing >./test_files/invalid_permission' 'complex missing/invalid'

# Append redirection tests
run_test 109 'ls >>./outfiles/outfile01' 'append redirect'
run_test 110 'ls >>      ./outfiles/outfile01' 'append redirect spaced'
run_test 111 'ls >>./outfiles/outfile01 >./outfiles/outfile01' 'append and output'
run_test 112 'ls >./outfiles/outfile01 >>./outfiles/outfile01' 'output and append'
run_test 113 'ls >./outfiles/outfile01 >>./outfiles/outfile01 >./outfiles/outfile02' 'mixed redirects'
run_test 114 'ls >>./outfiles/outfile01 >>./outfiles/outfile02' 'multiple appends'
run_test 115 'ls >>./test_files/invalid_permission' 'append to invalid'
run_test 116 'ls >>./test_files/invalid_permission >>./outfiles/outfile01' 'append invalid then valid'
run_test 117 'ls >>./outfiles/outfile01 >>./test_files/invalid_permission' 'append valid then invalid'
run_test 118 'ls >./outfiles/outfile01 >>./test_files/invalid_permission >>./outfiles/outfile02' 'mixed append invalid'
run_test 119 'ls <missing >>./test_files/invalid_permission >>./outfiles/outfile02' 'missing input append'
run_test 120 'ls >>./test_files/invalid_permission >>./outfiles/outfile02 <missing' 'append missing input'
run_test 121 'echo hi >>./outfiles/outfile01 | echo bye' 'append pipe'
run_test 122 'echo hi >>./outfiles/outfile01 >>./outfiles/outfile02 | echo bye' 'multiple append pipe'
run_test 123 'echo hi | echo >>./outfiles/outfile01 bye' 'pipe append'
run_test 124 'echo hi | echo bye >>./outfiles/outfile01 >>./outfiles/outfile02' 'pipe multiple append'
run_test 125 'echo hi >>./outfiles/outfile01 | echo bye >>./outfiles/outfile02' 'both sides append'
run_test 126 'echo hi >>./test_files/invalid_permission | echo bye' 'append invalid pipe'
run_test 127 'echo hi >>./test_files/invalid_permission >./outfiles/outfile01 | echo bye' 'append invalid output pipe'
run_test 128 'echo hi | echo bye >>./test_files/invalid_permission' 'pipe append invalid'
run_test 129 'echo hi | echo >>./outfiles/outfile01 bye >./test_files/invalid_permission' 'pipe append output invalid'
run_test 130 'cat <minishell.h>./outfiles/outfile' 'no space redirect'
run_test 131 'cat <minishell.h|ls' 'no space pipe'

echo ""
echo "———————————— extras"

# Extra tests
run_test 132 '' 'empty command'
run_test 133 '$PWD' 'variable as command'
run_test 134 '$EMPTY' 'empty variable'
run_test 135 '$EMPTY echo hi' 'empty var with args'
run_test 136 './test_files/invalid_permission' 'no permission executable'
run_test 137 './missing.out' 'missing executable'
run_test 138 'missing.out' 'missing command'
run_test 139 '"aaa"' 'quoted command'
run_test 140 'test_files' 'directory as command'
run_test 141 './test_files' 'directory as executable'
run_test 142 '/test_files' 'absolute directory'
run_test 143 'minishell.h' 'file as command'
run_test 144 '$' 'single dollar'
run_test 145 '$?' 'exit status as command'
run_test 146 'README.md' 'readme as command'

echo ""
echo "———————————— NEW TESTS - Complex Pipes"

# Complex pipe chains
run_test 147 'echo hello | cat | cat | cat | cat' 'long pipe chain'
run_test 148 'ls | grep "\.c" | head -5 | tail -3' 'complex pipe chain'
run_test 149 'env | sort | grep "=" | head -10 | tail -5' 'env processing pipe'
run_test 150 'echo "a b c d e" | tr " " "\n" | sort | tr "\n" " "' 'complex text processing'
run_test 151 'cat /etc/passwd | head -10 | grep root | wc -l' 'system file processing'
run_test 152 'echo hello | cat | echo world | cat' 'mixed pipe commands'
run_test 153 'pwd | cat | pwd | cat | pwd' 'repeated commands in pipe'
run_test 154 'echo test | "|"' 'pipe character as command'
run_test 155 'echo hello | "" | echo world' 'empty command in pipe'
run_test 156 'echo test | nonexistent | echo world' 'nonexistent command in pipe'

echo ""
echo "———————————— NEW TESTS - Heredocs"

# Heredoc tests
run_heredoc_test 157 $'cat <<EOF\nhello world\nEOF' 'basic heredoc'
run_heredoc_test 158 $'cat <<DELIMITER\nline1\nline2\nDELIMITER' 'custom delimiter'
run_heredoc_test 159 $'cat <<"EOF"\nhello $USER\nEOF' 'quoted heredoc delimiter'
run_heredoc_test 160 $'cat <<EOF\n$PWD\n$USER\nEOF' 'heredoc with variables'
run_heredoc_test 161 $'grep hello <<EOF\nhello world\nhi there\nEOF' 'heredoc with grep'
run_heredoc_test 162 $'cat <<EOF | grep hello\nhello world\nhi there\nEOF' 'heredoc piped'
run_heredoc_test 163 $'cat <<EOF >./outfiles/heredoc_out\nhello\nworld\nEOF' 'heredoc with output redirect'
run_heredoc_test 164 $'cat <<A <<B\nhello\nA\nworld\nB' 'multiple heredocs'
run_heredoc_test 165 $'cat <<EOF\n$?\nEOF' 'heredoc with exit status'
run_heredoc_test 166 $'cat <<EOF\n\nEOF' 'empty heredoc'

echo ""
echo "———————————— NEW TESTS - Advanced Variable Expansion"

# Advanced variable expansion
run_test 167 'echo $USER$HOME' 'concatenated variables'
run_test 168 'echo ${USER}test' 'variable with suffix'
run_test 169 'echo "$USER$HOME"' 'quoted concatenated variables'
run_test 170 'echo $NONEXISTENT' 'nonexistent variable'
run_test 171 'echo "$NONEXISTENT"' 'quoted nonexistent variable'
run_test 172 'echo $USER$NONEXISTENT$HOME' 'mixed existing/nonexistent'
run_test 173 'echo $$$' 'multiple dollar signs'
run_test 174 'echo $?$?$?' 'multiple exit status'
run_test 175 'export TEST=hello; echo $TEST' 'export and use variable'
run_test 176 'export TEST="hello world"; echo $TEST' 'export quoted variable'
run_test 177 'export TEST=$PWD; echo $TEST' 'export variable as variable'
run_test 178 'echo "$USER says: $HOME"' 'variables in sentence'
run_test 179 'echo $USER_HOME' 'underscore in variable name'
run_test 180 'echo $123' 'numeric variable name'

echo ""
echo "———————————— NEW TESTS - Quote Edge Cases"

# Quote edge cases
run_test 181 'echo "hello'\''world"' 'single quote in double quotes'
run_test 182 'echo '\''hello"world'\''' 'double quote in single quotes'
run_test 183 'echo "hello\"world"' 'escaped double quote'
run_test 184 'echo '\''hello'\''world'\''' 'multiple single quotes'
run_test 185 'echo ""hello""' 'empty quoted strings'
run_test 186 'echo '\'''\''hello'\'''\''' 'empty single quotes with content'
run_test 187 'echo """hello"""' 'triple double quotes'
run_test 188 'echo '\''"hello"'\''' 'double quotes in single quotes'
run_test 189 'echo "a"b"c"' 'mixed quoted and unquoted'
run_test 190 'echo '\''a'\''b'\''c'\''' 'multiple single quote segments'

echo ""
echo "———————————— NEW TESTS - Builtin Edge Cases"

# Builtin edge cases
run_test 191 'cd' 'cd without arguments'
run_test 192 'cd ~' 'cd to home'
run_test 193 'cd /' 'cd to root'
run_test 194 'cd /tmp && pwd' 'cd and pwd'
run_test 195 'export' 'export without arguments'
run_test 196 'export TEST=' 'export empty value'
run_test 197 'export TEST=""' 'export quoted empty value'
run_test 198 'export TEST=; echo $TEST' 'export and use empty'
run_test 199 'unset PATH; echo $PATH' 'unset important variable'
run_test 200 'env | wc -l' 'count environment variables'
run_test 201 'exit' 'exit without arguments'
run_test 202 'exit 0' 'exit with zero'
run_test 203 'exit 255' 'exit with max value'
run_test 204 'exit 256' 'exit with overflow'
run_test 205 'exit -1' 'exit with negative'
run_test 206 'pwd | pwd | pwd' 'multiple pwd in pipe'

echo ""
echo "———————————— NEW TESTS - Stress Tests"

# Stress tests
run_test 207 'echo "$(printf "a%.0s" {1..100})"' 'very long string'
run_test 208 'echo hello world with many arguments here and there' 'many arguments'
run_test 209 'cat ./test_files/large_content | head | tail' 'large file processing'
run_test 210 'for i in {1..50}; do echo "test$i"; done | head -20' 'many commands'
run_test 211 'echo hello | cat | cat | cat | cat | cat | cat | cat | cat | cat | cat' 'very long pipe'
run_test 212 'echo a b c d e f g h i j k l m n o p q r s t u v w x y z' 'alphabet arguments'
run_test 213 'echo "'"'"'$(printf "word%.0s " {1..50})"'"'"'"' 'many words'
run_test 214 'echo $PWD$PWD$PWD$PWD$PWD$PWD$PWD$PWD$PWD$PWD' 'repeated variable expansion'
run_test 215 'cat <<EOF | cat | cat | cat\nhello\nworld\nEOF' 'heredoc through long pipe'
run_test 216 'echo hello >./outfiles/f1 >./outfiles/f2 >./outfiles/f3 >./outfiles/f4' 'many redirects'

echo ""
echo "———————————— NEW TESTS - Error Handling"

# Error handling edge cases
run_test 217 'cat /dev/null' 'read empty file'
run_test 218 'cat /nonexistent/path/file' 'deeply nonexistent path'
run_test 219 'cd /nonexistent/path' 'cd to nonexistent path'
run_test 220 'export 123=value' 'export with invalid name'
run_test 221 'unset 123' 'unset invalid name'
run_test 222 'echo hello >&999' 'invalid file descriptor'
run_test 223 '| echo hello' 'pipe at start'
run_test 224 'echo hello |' 'pipe at end'
run_test 225 'echo hello | | echo world' 'double pipe'
run_test 226 'echo hello > > file' 'spaced redirect operators'
run_test 227 'echo hello < < file' 'spaced input operators'
run_test 228 ';' 'semicolon only'
run_test 229 'echo hello;' 'trailing semicolon'
run_test 230 ';echo hello' 'leading semicolon'

echo ""
echo "———————————— NEW TESTS - Mixed Complex Scenarios"

# Mixed complex scenarios
run_test 231 'echo "hello world" | grep hello | cat > ./outfiles/complex1' 'complex pipe with redirect'
run_test 232 'cat < ./test_files/infile | echo "prefix: $(cat)" > ./outfiles/complex2' 'complex input/output'
run_test 233 'export TEST="hello world"; echo $TEST | tr " " "_"' 'export, expand, and process'
run_test 234 'echo $USER | cat | echo "user: $(cat)" | cat' 'nested command processing'
run_test 235 'cd /tmp && echo $PWD && cd - && echo $PWD' 'directory operations'
run_test 236 'echo hello | cat <<EOF\nworld\nEOF' 'pipe with heredoc'
run_test 237 'cat <<EOF | echo "got: $(cat)"\nhello\nEOF' 'heredoc with command substitution'
run_test 238 'echo "start" && echo "middle" | echo "end"' 'mixed operators'
run_test 239 'export A=1; export B=2; echo $A$B' 'multiple exports and use'
run_test 240 'unset PATH; export PATH=/bin:/usr/bin; echo $PATH' 'path manipulation'

echo ""
echo "———————————— NEW TESTS - Executable and Permission Tests"

# Executable and permission tests
run_test 241 './test_files/script.sh' 'executable script'
run_test 242 '/bin/echo hello' 'absolute path command'
run_test 243 'which echo' 'which command'
run_test 244 'ls -la | head -5' 'ls with options'
run_test 245 'echo $0' 'shell name variable'
run_test 246 'echo ${PWD}' 'braced variable'
run_test 247 'touch ./outfiles/newfile && ls ./outfiles/newfile' 'file creation and check'
run_test 248 'mkdir -p ./outfiles/subdir && ls ./outfiles/' 'directory creation'
run_test 249 'test -f ./test_files/infile && echo "exists"' 'file test'
run_test 250 'echo hello | tee ./outfiles/tee_test | cat' 'tee command'

echo ""
echo "============================================="
echo "🏁 Test Summary:"
echo "Total tests: $TOTAL_TESTS"
echo -e "Tests with leaks: ${RED}$LEAKED_TESTS${NC}"
echo -e "Tests with errors: ${YELLOW}$FAILED_TESTS${NC}"
echo -e "Clean tests: ${GREEN}$((TOTAL_TESTS - LEAKED_TESTS - FAILED_TESTS))${NC}"

if [ $LEAKED_TESTS -eq 0 ] && [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}🎉 All tests passed without leaks!${NC}"
else
    echo -e "${RED}⚠️  Some tests have issues. Review the output above.${NC}"
fi

# Cleanup
chmod 755 test_files/invalid_permission
rm -rf outfiles test_files 