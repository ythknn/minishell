#!/bin/bash

MINISHELL=./minishell
LOGFILE=minishell_full_leak_report.log
SUPPRESSION_FILE=readline.supp

# Renk kodları
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[1;33m"
CYAN="\033[0;36m"
BLUE="\033[0;34m"
MAGENTA="\033[0;35m"
RESET="\033[0m"

# Raporu temizle
echo "=== EXTREME Minishell Leak Test (Enhanced Version) ===" > $LOGFILE
echo "Test Başlangıcı: $(date)" >> $LOGFILE
echo "" >> $LOGFILE

# Test kategorileri
BASIT_KOMUTLAR=(
    "echo hello"
    "echo 123 456 789"
    "pwd"
    "ls"
    "env"
    "/bin/ls"
    "/bin/echo test"
    "cat /etc/passwd"
    "whoami"
    "date"
)

ARGUMENTS_HISTORY=(
    "/bin/ls -la"
    "/bin/echo hello world"
    "ls -l -a -h"
    "echo first second third"
    "cat /etc/hosts"
    "grep root /etc/passwd"
    "wc -l /etc/passwd"
    "sort /etc/passwd"
)

ECHO_TESTS=(
    "echo"
    "echo hello"
    "echo -n hello"
    "echo hello world"
    "echo -n"
    "echo -n test"
    "echo multiple words here"
)

EXIT_TESTS=(
    "exit"
    "exit 0"
    "exit 1"
    "exit 42"
    "exit 255" 
    "exit -1"
    "exit 256"
    "exit abc"
    "exit 42 extra"
)

RETURN_VALUE_TESTS=(
    "/bin/true"
    "/bin/false"
    "ls /nonexistent"
    "/bin/echo hello"
    "cat /nonexistent/file"
    "/nonexistent/command"
    "ls /"
)

SIGNAL_TESTS=(
    # Bu testler manuel müdahale gerektirecek, bu yüzden yorum satırı olarak bırakıyorum
    # "cat" # Ctrl+C test için
    # "grep test" # Ctrl+C test için  
    # "sleep 10" # Ctrl+C test için
)

DOUBLE_QUOTES=(
    "echo \"hello world\""
    "echo \"cat lol.c | cat > lol.c\""
    "echo \"test with spaces\""
    "echo \"mix 'single' in double\""
    "echo \"\$HOME should expand\""
    "echo \"whitespace   test\""
    "echo \"\""
    "echo \"unclosed quote"
)

SINGLE_QUOTES=(
    "echo 'hello world'"
    "echo 'test'"
    "echo ''"
    "echo '\$USER should not expand'"
    "echo 'pipes | and > redirects'"
    "echo 'single quote test'"
    "echo 'unclosed quote"
)

ENV_TESTS=(
    "env"
)

EXPORT_TESTS=(
    "export"
    "export VAR1=hello"
    "export VAR2=world"
    "export VAR1=hello && env | grep VAR1"
    "export VAR3= && env | grep VAR3"
    "export 123=invalid"
    "export VAR-INVALID=test"
    "export VAR4=hello world"
)

UNSET_TESTS=(
    "export TEST=hello && unset TEST && env | grep TEST"
    "unset PATH && env | grep PATH"
    "unset NONEXISTENT"
    "unset"
    "export A=1 B=2 && unset A && env | grep A"
)

CD_TESTS=(
    "cd"
    "cd /"
    "cd /tmp"
    "cd .."
    "cd ."
    "cd /nonexistent"
    "cd ../.."
    "cd ~"
    "cd -"
)

PWD_TESTS=(
    "pwd"
    "cd / && pwd"
    "cd /tmp && pwd"
    "cd .. && pwd"
)

RELATIVE_PATH_TESTS=(
    "./minishell --version 2>/dev/null || echo 'relative path test'"
    "ls ./src/ 2>/dev/null || echo 'relative path test'"
    "../minishell 2>/dev/null || echo 'relative path test'"
    "cat ./Makefile 2>/dev/null || echo 'file test'"
)

ENVIRONMENT_PATH_TESTS=(
    "ls"
    "cat"
    "echo test"
    "pwd"
    "unset PATH && ls"
    "export PATH=/bin:/usr/bin && ls"
    "export PATH=/custom/path && ls 2>/dev/null || echo 'path not found'"
)

REDIRECTION_TESTS=(
    "echo test > out.txt && cat out.txt"
    "cat < /etc/passwd | head -1"
    "echo append >> out.txt"
    "ls > file.txt"
    "cat < file.txt"
    "echo overwrite > file.txt"
    "cat << EOF
hello
world
EOF"
    "wc -l < /etc/passwd"
    "ls nonexistent 2> error.log"
    "ls > output.txt 2> error.txt"
)

PIPE_TESTLERI=(
    "ls | wc -l"
    "ls | grep a"
    "cat /etc/passwd | grep root"
    "echo hello | cat"
    "ls | grep . | wc -l"
    "cat /etc/passwd | head | tail"
    "echo test | cat | cat | cat"
    "ls /nonexistent | grep error"
    "echo hello | grep hello | wc -l"
)

QUOTE_TESTLERI=(
    "echo \"hello world\""
    "echo 'single quote test'"
    "echo \"mix 'quotes' and \\\"escapes\\\"\""
    "echo \$HOME"
    "echo '\$HOME literal'"
    "echo \"\$USER in quotes\""
    "echo 'cannot expand \$USER'"
)

LOGICAL_OPS=(
    "echo first && echo second"
    "echo first || echo second"
    "false && echo should_not_print"
    "false || echo should_print"
    "true && echo success"
    "echo hi && false || echo bye"
    "/bin/true && echo true_worked"
    "/bin/false || echo false_worked"
)

SYNTAX_ERRORLAR=(
    "| ls"
    "ls |"
    "|| echo fail"
    "&& echo fail"
    "ls &&"
    "echo \"unclosed"
    "echo 'unclosed"
    "cat <"
    "echo >"
    ";;"
    ";;;"
    "ls ;;; echo test"
)

SUBSHELL_TESTLERI=(
    "(ls)"
    "(echo nested)"
    "(cd /tmp && pwd)"
    "(export TEST=value && echo \$TEST)"
    "(ls | grep a)"
    "(echo test > file.txt)"
)

HEREDOC_TESTLERI=(
    "cat << EOF
test line 1
test line 2
EOF"
    "grep hello << DELIMITER
hello world  
goodbye world
DELIMITER"
    "wc -l << END
line 1
line 2
line 3
END"
)

ENV_EXPAND=(
    "export TEST1=hello && echo \$TEST1"
    "export X=foo Y=bar && echo \$X\$Y"
    "echo \$HOME"
    "echo \$USER"
    "echo \$PATH"
    "echo \$NONEXISTENT"
    "export TEST=value && echo \"\$TEST\""
    "export TEST=value && echo '\$TEST'"
    "unset HOME && echo \$HOME"
)

HISTORY_COMMAND_TESTS=(
    "echo command1"
    "ls -la"
    "pwd"
    "echo test"
    "cat /etc/passwd | head -1"
)

MIXED_COMPLEX_TESTS=(
    "export VAR=hello && echo \$VAR | cat"
    "ls -la | grep ^d | wc -l"
    "echo \"test\" > file && cat file | grep test"
    "(export TEST=nested && echo \$TEST) && echo done"
    "cd /tmp && ls | head -5 && cd -"
    "echo hello && (echo world | cat) || echo failed"
    "ls /nonexistent 2>/dev/null || echo \"directory not found\""
    "/bin/echo test | /bin/cat | /bin/wc -l"
)

STRESS_TESTS=(
    "echo very long command with many arguments here to test argument parsing"
    "ls -la -h -t -r -S"
    "export A=1 B=2 C=3 D=4 E=5 && echo \$A\$B\$C\$D\$E"
    "echo a | echo b | echo c | echo d"
    "((((echo nested))))"
)

declare -A TEST_KATEGORILERI=(
    ["BASIT_KOMUTLAR"]="Temel Komutlar"
    ["ARGUMENTS_HISTORY"]="Arguments & History Tests"
    ["ECHO_TESTS"]="Echo Command Tests"
    ["EXIT_TESTS"]="Exit Command Tests"
    ["RETURN_VALUE_TESTS"]="Return Value Tests"
    ["DOUBLE_QUOTES"]="Double Quote Tests"
    ["SINGLE_QUOTES"]="Single Quote Tests"
    ["ENV_TESTS"]="Environment Variable Display"
    ["EXPORT_TESTS"]="Export Command Tests"
    ["UNSET_TESTS"]="Unset Command Tests"
    ["CD_TESTS"]="CD Command Tests"
    ["PWD_TESTS"]="PWD Command Tests"
    ["RELATIVE_PATH_TESTS"]="Relative Path Tests"
    ["ENVIRONMENT_PATH_TESTS"]="Environment PATH Tests"
    ["REDIRECTION_TESTS"]="Redirection Tests"
    ["PIPE_TESTLERI"]="Pipe Tests"
    ["QUOTE_TESTLERI"]="Quote/Escape Tests"
    ["LOGICAL_OPS"]="Logical Operators (&&, ||)"
    ["SYNTAX_ERRORLAR"]="Syntax Error Tests"
    ["SUBSHELL_TESTLERI"]="Subshell/Parentheses Tests"
    ["HEREDOC_TESTLERI"]="Heredoc Tests"
    ["ENV_EXPAND"]="Environment Variable Expansion"
    ["HISTORY_COMMAND_TESTS"]="History and Command Tests"
    ["MIXED_COMPLEX_TESTS"]="Complex Mixed Tests"
    ["STRESS_TESTS"]="Stress Tests"
)

TOPLAM_TEST=0
TOPLAM_BASARILI=0
TOPLAM_LEAK=0
TOPLAM_STILL_REACHABLE=0

create_test_files() {
    echo "test content" > test_file.txt
    echo -e "line1\nline2\nline3" > multiline.txt
    mkdir -p test_dir 2>/dev/null
    echo "Creating test files for redirection tests..."
}

cleanup_test_files() {
    rm -f out.txt file.txt output.txt error.txt error.log result.txt
    rm -f test_file.txt multiline.txt
    rm -rf test_dir 2>/dev/null
    echo "Cleaning up test files..."
}

run_category() {
    local category_name="$1"
    local category_display_name="${TEST_KATEGORILERI[$category_name]}"
    local -n tests=$category_name

    echo -e "\n${CYAN}### Kategori: $category_display_name (${#tests[@]} test) ###${RESET}\n"
    echo "=== Kategori: $category_display_name ===" >> $LOGFILE

    local passed=0
    local leaked=0
    local still_reachable=0

    for cmd in "${tests[@]}"; do
        ((TOPLAM_TEST++))
        printf "${YELLOW}Test %-3d:${RESET} %-60s ... " "$TOPLAM_TEST" "$cmd"
        echo "=== TEST $TOPLAM_TEST: $cmd ===" >> $LOGFILE

        # Test dosyaları oluştur
        create_test_files >/dev/null 2>&1

        # Heredoc testleri için özel işlem
        if [[ "$cmd" == *"<< EOF"* ]] || [[ "$cmd" == *"<< DELIMITER"* ]] || [[ "$cmd" == *"<< END"* ]]; then
            echo "$cmd" | valgrind --quiet --leak-check=full --show-leak-kinds=all \
                --suppressions=$SUPPRESSION_FILE \
                --log-file=valgrind_tmp.log \
                $MINISHELL >/dev/null 2>&1
        else
            echo "$cmd" | timeout 5 valgrind --quiet --leak-check=full --show-leak-kinds=all \
                --suppressions=$SUPPRESSION_FILE \
                --log-file=valgrind_tmp.log \
                $MINISHELL >/dev/null 2>&1
        fi

        if grep -qE "definitely lost|indirectly lost|possibly lost" valgrind_tmp.log; then
            printf "${RED}❌ Memory Leak Detected${RESET}\n"
            grep -E "definitely lost|indirectly lost|possibly lost" valgrind_tmp.log >> $LOGFILE
            ((leaked++))
            ((TOPLAM_LEAK++))
        elif grep -q "still reachable" valgrind_tmp.log; then
            printf "${YELLOW}⚠️ Still Reachable Memory${RESET}\n"
            grep -E "still reachable" valgrind_tmp.log >> $LOGFILE
            ((TOPLAM_BASARILI++))
            ((passed++))
            ((still_reachable++))
            ((TOPLAM_STILL_REACHABLE++))
        else
            printf "${GREEN}✅ Clean${RESET}\n"
            ((TOPLAM_BASARILI++))
            ((passed++))
        fi

        # Test dosyalarını temizle
        cleanup_test_files >/dev/null 2>&1
        echo "" >> $LOGFILE
    done

    echo -e "${CYAN}Kategori Sonucu:${RESET}"
    echo -e "  ${GREEN}✅ Clean: $passed${RESET}"
    echo -e "  ${YELLOW}⚠️ Still Reachable: $still_reachable${RESET}"
    echo -e "  ${RED}❌ Memory Leaks: $leaked${RESET}"
    echo -e "  Toplam: ${#tests[@]} test"
}

# Başlangıç kontrolü
if [ ! -f "$MINISHELL" ]; then
    echo -e "${RED}Error: $MINISHELL bulunamadı!${RESET}"
    echo -e "${YELLOW}Lütfen minishell'i derleyin: make${RESET}"
    exit 1
fi

if [ ! -f "$SUPPRESSION_FILE" ]; then
    echo -e "${YELLOW}Warning: $SUPPRESSION_FILE bulunamadı.${RESET}"
    echo -e "${YELLOW}Readline kütüphanesinden kaynaklanan false positifleri görmezden gelmek için bu dosyayı oluşturun.${RESET}"
fi

echo -e "${BLUE}=== Enhanced Minishell Memory Leak Tester ===${RESET}"
echo -e "${MAGENTA}Proje gereksinimleri doğrultusunda kapsamlı test yapılıyor...${RESET}\n"

# İlk test dosyaları oluştur
create_test_files

# Tüm kategorileri sırayla çalıştır
run_category BASIT_KOMUTLAR
run_category ARGUMENTS_HISTORY  
run_category ECHO_TESTS
run_category EXIT_TESTS
run_category RETURN_VALUE_TESTS
run_category DOUBLE_QUOTES
run_category SINGLE_QUOTES
run_category ENV_TESTS
run_category EXPORT_TESTS
run_category UNSET_TESTS
run_category CD_TESTS
run_category PWD_TESTS
run_category RELATIVE_PATH_TESTS
run_category ENVIRONMENT_PATH_TESTS
run_category REDIRECTION_TESTS
run_category PIPE_TESTLERI
run_category QUOTE_TESTLERI
run_category LOGICAL_OPS
run_category SYNTAX_ERRORLAR
run_category SUBSHELL_TESTLERI
run_category HEREDOC_TESTLERI
run_category ENV_EXPAND
run_category HISTORY_COMMAND_TESTS
run_category MIXED_COMPLEX_TESTS
run_category STRESS_TESTS

# Son temizlik
cleanup_test_files
rm -f valgrind_tmp.log

echo -e "\n${BLUE}=== Final Report ===${RESET}"
echo -e "${GREEN}✅ Clean Tests: $(($TOPLAM_BASARILI - $TOPLAM_STILL_REACHABLE))${RESET}"
echo -e "${YELLOW}⚠️ Still Reachable: $TOPLAM_STILL_REACHABLE${RESET}"
echo -e "${RED}❌ Memory Leaks: $TOPLAM_LEAK${RESET}"
echo -e "${CYAN}📊 Total Tests: $TOPLAM_TEST${RESET}"
echo ""

if [ $TOPLAM_LEAK -eq 0 ]; then
    echo -e "${GREEN}🎉 Tebrikler! Hiç memory leak tespit edilmedi!${RESET}"
else
    echo -e "${RED}⚠️ $TOPLAM_LEAK test'te memory leak tespit edildi.${RESET}"
    echo -e "${YELLOW}Detaylar için log dosyasını kontrol edin: $LOGFILE${RESET}"
fi

echo -e "\nDetaylı rapor: ${YELLOW}$LOGFILE${RESET}"

# Son durum kodu
if [ $TOPLAM_LEAK -eq 0 ]; then
    exit 0
else
    exit 1
fi
