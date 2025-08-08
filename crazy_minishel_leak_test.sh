#!/bin/bash

MINISHELL=./minishell
LOGFILE=minishell_full_leak_report.log
COMPARISON_LOG=minishell_bash_comparison.log
SUPPRESSION_FILE=readline.supp

# Renk kodları
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[1;33m"
CYAN="\033[0;36m"
BLUE="\033[0;34m"
MAGENTA="\033[0;35m"
BOLD="\033[1m"
RESET="\033[0m"

# Global counters
TOPLAM_TEST=0
TOPLAM_BASARILI=0
TOPLAM_LEAK=0
TOPLAM_STILL_REACHABLE=0
TOPLAM_COMPARISON_DIFF=0

# Comparison mode flag
COMPARISON_MODE=false
if [[ "$1" == "--compare" || "$1" == "-c" ]]; then
    COMPARISON_MODE=true
fi

# Raporu temizle
echo "=== ENHANCED Minishell Leak Test & Bash Comparison (Ultimate Version) ===" > $LOGFILE
echo "Test Başlangıcı: $(date)" >> $LOGFILE
echo "Comparison Mode: $COMPARISON_MODE" >> $LOGFILE
echo "" >> $LOGFILE

if $COMPARISON_MODE; then
    echo "=== BASH COMPARISON REPORT ===" > $COMPARISON_LOG
    echo "Test Başlangıcı: $(date)" >> $COMPARISON_LOG
    echo "" >> $COMPARISON_LOG
fi

# Enhanced test categories based on PDF requirements
BASIT_KOMUTLAR=(
    "echo hello"
    "echo 123 456 789"
    "pwd"
    "ls"
    "env"
    "/bin/ls"
    "/bin/echo test"
    "/bin/cat /etc/passwd"
    "whoami"
    "date"
    "echo test"
    "/bin/pwd"
    "ls -l"
    "/usr/bin/env"
)

ARGUMENTS_HISTORY=(
    "/bin/ls -la"
    "/bin/echo hello world"
    "ls -l -a -h"
    "echo first second third"
    "/bin/cat /etc/hosts"
    "grep root /etc/passwd"
    "wc -l /etc/passwd"
    "/bin/ls -la -h -t"
    "echo arg1 arg2 arg3 arg4 arg5"
    "/bin/echo multiple arguments here"
    "/bin/ls -l -a -h -t -r"
)

ECHO_TESTS=(
    "echo"
    "echo hello"
    "echo -n hello"
    "echo hello world"
    "echo -n"
    "echo -n test"
    "echo multiple words here"
    "echo -n multiple args"
    "echo test with spaces"
    "echo \$USER"
    "echo \"\""
    "echo ''"
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
    "exit 999"
    "exit -999"
    "exit hello"
)

RETURN_VALUE_TESTS=(
    "/bin/true"
    "/bin/false"
    "ls /nonexistent"
    "/bin/echo hello"
    "/bin/cat /nonexistent/file"
    "/nonexistent/command"
    "ls /"
    "expr \$? = 0"
    "expr \$? + \$?"
)

SIGNAL_TESTS=(
    # These will need manual intervention, commented for safety
    # "sleep 1"  # For Ctrl+C testing
    # "cat"      # For Ctrl+C testing
    "echo signal_test"
)

DOUBLE_QUOTES=(
    "echo \"hello world\""
    "echo \"cat lol.c | cat > lol.c\""
    "echo \"test with spaces\""
    "echo \"mix 'single' in double\""
    "echo \"\$HOME should expand\""
    "echo \"whitespace   test\""
    "echo \"\""
    "echo \"multiple   spaces   here\""
    "echo \"tab\ttest\""
    "echo \"\$USER and \$HOME\""
    "echo \"quotes inside \\\"nested\\\"\""
)

SINGLE_QUOTES=(
    "echo 'hello world'"
    "echo 'test'"
    "echo ''"
    "echo '\$USER should not expand'"
    "echo 'pipes | and > redirects'"
    "echo 'single quote test'"
    "echo '\$HOME literal'"
    "echo 'nothing \$USER expands'"
    "echo 'whitespace   preserved'"
    "echo 'special chars !@#\$%^&*()'"
)

ENV_TESTS=(
    "env"
    "env | head -5"
    "env | grep USER"
    "env | grep HOME"
    "env | grep PATH"
)

EXPORT_TESTS=(
    "export"
    "export VAR1=hello"
    "export VAR2=world"
    "export VAR3="
    "export 123=invalid"
    "export VAR-INVALID=test"
    "export VAR4=\"hello world\""
    "export A=1 B=2 C=3"
    "export PATH=/custom/path"
    "export TEST_VAR=test_value"
)

UNSET_TESTS=(
    "unset PATH"
    "unset NONEXISTENT"
    "unset"
    "unset HOME"
    "unset USER"
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
    "cd relative_path_test"
)

PWD_TESTS=(
    "pwd"
    "/bin/pwd"
)

RELATIVE_PATH_TESTS=(
    "ls ./"
    "ls ../"
    "/bin/cat ./Makefile"
    "./test_script.sh"
    "ls ./src/"
    "../minishell"
    "./minishell --version"
)

ENVIRONMENT_PATH_TESTS=(
    "ls"
    "/bin/cat"
    "echo test"
    "pwd"
    "/bin/ls"
    "export PATH=/bin:/usr/bin"
    "export PATH=/custom/path"
    "export PATH=/bin"
    "export PATH=/usr/bin:/bin"
)

REDIRECTION_TESTS=(
    "ls > file.txt"
    "/bin/cat < file.txt"
    "echo overwrite > file.txt"
    "wc -l < /etc/passwd"
    "ls nonexistent 2> error.log"
    "ls > output.txt 2> error.txt"
    "echo redirect_test > test.out"
)

PIPE_TESTLERI=(
    "ls | wc -l"
    "ls | grep a"
    "/bin/cat /etc/passwd | grep root"
    "echo hello | /bin/cat"
    "ls | grep . | wc -l"
    "/bin/cat /etc/passwd | head | tail"
    "echo test | /bin/cat | /bin/cat | /bin/cat"
    "ls /nonexistent | grep error"
    "echo hello | grep hello | wc -l"
    "/bin/echo pipe_test | /bin/cat"
    "ls | head -5"
    "/bin/ls | /bin/grep test"
)

QUOTE_TESTLERI=(
    "echo \"hello world\""
    "echo 'single quote test'"
    "echo \"mix 'quotes' and \\\"escapes\\\"\""
    "echo \$HOME"
    "echo '\$HOME literal'"
    "echo \"\$USER in quotes\""
    "echo 'cannot expand \$USER'"
    "echo \"nested 'single' quotes\""
    "echo 'nested \"double\" quotes'"
    "echo \"\$HOME/\$USER\""
)

SYNTAX_ERRORLAR=(
    "| ls"
    "ls |"
    "/bin/cat <"
    "echo >"
    ";;"
    ";;;"
    "ls ;;; echo test"
    "| | echo fail"
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
    "/bin/cat << MARKER
heredoc test
multiple lines
MARKER"
)

ENV_EXPAND=(
    "echo \$HOME"
    "echo \$USER"
    "echo \$PATH"
    "echo \$NONEXISTENT"
    "echo \"\$USER\" and '\$USER'"
)

HISTORY_COMMAND_TESTS=(
    "echo command1"
    "ls -la"
    "pwd"
    "echo test"
    "/bin/cat /etc/passwd | head -1"
    "/bin/echo history_test"
)

MIXED_COMPLEX_TESTS=(
    "ls -la | grep ^d | wc -l"
    "echo \"test\" > file"
    "cd /tmp"
    "/bin/echo test | /bin/cat | wc -l"
)

STRESS_TESTS=(
    "echo very long command with many arguments here to test argument parsing and memory management"
    "ls -la -h -t -r -S"
    "echo a | echo b | echo c | echo d"
    "/bin/echo stress"
    "echo arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9 arg10"
)

# PDF'den ek örnekler
PDF_SPECIFIC_TESTS=(
    "echo \$USER must print \$USER"
    "echo '\$USER' must print \$USER"
    "echo \"\\\"cat lol.c | cat > lol.c\\\"\""
    "/bin/ls filethatdoesntexist"
    "expr \$? + \$?"
    "/bin/ls > /dev/null"
    "echo \$?"
    "export TEST=value"
    "unset TEST"
    "cd /"
    "cd nonexistent"
    "pwd"
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
    ["SYNTAX_ERRORLAR"]="Syntax Error Tests"
    ["HEREDOC_TESTLERI"]="Heredoc Tests"
    ["ENV_EXPAND"]="Environment Variable Expansion"
    ["HISTORY_COMMAND_TESTS"]="History and Command Tests"
    ["MIXED_COMPLEX_TESTS"]="Complex Mixed Tests"
    ["STRESS_TESTS"]="Stress Tests"
    ["PDF_SPECIFIC_TESTS"]="PDF Requirement Tests"
)

create_test_files() {
    echo "test content" > test_file.txt
    echo -e "line1\nline2\nline3" > multiline.txt
    mkdir -p test_dir 2>/dev/null
    echo "#!/bin/bash\necho 'test script'" > test_script.sh
    chmod +x test_script.sh 2>/dev/null
}

cleanup_test_files() {
    rm -f out.txt file.txt output.txt error.txt error.log result.txt
    rm -f test_file.txt multiline.txt redirect.txt test.out
    rm -f test_script.sh
    rm -rf test_dir 2>/dev/null
}

# ANSI renk kodlarını temizleyen fonksiyon
clean_ansi_codes() {
    sed 's/\x1b\[[0-9;]*m//g'
}

# Minishell prompt'unu temizleyen fonksiyon
clean_minishell_output() {
    local output="$1"
    local cmd="$2"
    
    # ANSI kodlarını temizle
    output=$(echo "$output" | clean_ansi_codes)
    
    # Minishell promptunu kaldır
    # Örnek: "minishell$> echo hello" satırını kaldır
    output=$(echo "$output" | grep -v "^minishell\$>")
    
    # Boş minishell$> satırlarını kaldır
    output=$(echo "$output" | grep -v "^$")
    
    # Komut satırını kaldır (eğer çıktıda varsa)
    if [[ -n "$cmd" ]]; then
        output=$(echo "$output" | grep -v "^${cmd}$")
    fi
    
    # Başındaki ve sonundaki boşlukları temizle
    output=$(echo "$output" | sed 's/^[[:space:]]*//g' | sed 's/[[:space:]]*$//g')
    
    echo "$output"
}

run_bash_command() {
    local cmd="$1"
    local timeout_duration=5
    
    # Special handling for exit commands in bash
    if [[ "$cmd" == "exit"* ]]; then
        echo "$cmd" | timeout $timeout_duration bash 2>/dev/null
        return $?
    fi
    
    # Special handling for heredoc
    if [[ "$cmd" == *"<< EOF"* ]] || [[ "$cmd" == *"<< DELIMITER"* ]] || [[ "$cmd" == *"<< END"* ]] || [[ "$cmd" == *"<< MARKER"* ]]; then
        echo "$cmd" | timeout $timeout_duration bash 2>/dev/null
        return $?
    fi
    
    echo "$cmd" | timeout $timeout_duration bash 2>/dev/null
    return $?
}

run_minishell_command() {
    local cmd="$1"
    local timeout_duration=5
    
    # Special handling for heredoc
    if [[ "$cmd" == *"<< EOF"* ]] || [[ "$cmd" == *"<< DELIMITER"* ]] || [[ "$cmd" == *"<< END"* ]] || [[ "$cmd" == *"<< MARKER"* ]]; then
        echo "$cmd" | timeout $timeout_duration $MINISHELL 2>/dev/null
        return $?
    fi
    
    echo "$cmd" | timeout $timeout_duration $MINISHELL 2>/dev/null
    return $?
}

compare_outputs() {
    local cmd="$1"
    
    # Skip exit commands for comparison
    if [[ "$cmd" == "exit"* ]]; then
        return 0
    fi
    
    create_test_files >/dev/null 2>&1
    
    # Run bash
    bash_output=$(run_bash_command "$cmd" 2>&1)
    bash_exit=$?
    
    # Run minishell  
    minishell_raw_output=$(run_minishell_command "$cmd" 2>&1)
    minishell_exit=$?
    
    # Clean minishell output
    minishell_output=$(clean_minishell_output "$minishell_raw_output" "$cmd")
    
    cleanup_test_files >/dev/null 2>&1
    
    # Compare outputs
    if [[ "$bash_output" != "$minishell_output" ]] || [[ $bash_exit -ne $minishell_exit ]]; then
        echo "=== DIFFERENCE FOUND: $cmd ===" >> $COMPARISON_LOG
        echo "BASH OUTPUT:" >> $COMPARISON_LOG
        echo "'$bash_output'" >> $COMPARISON_LOG
        echo "BASH EXIT CODE: $bash_exit" >> $COMPARISON_LOG
        echo "" >> $COMPARISON_LOG
        echo "MINISHELL RAW OUTPUT:" >> $COMPARISON_LOG
        echo "'$minishell_raw_output'" >> $COMPARISON_LOG
        echo "MINISHELL CLEANED OUTPUT:" >> $COMPARISON_LOG
        echo "'$minishell_output'" >> $COMPARISON_LOG
        echo "MINISHELL EXIT CODE: $minishell_exit" >> $COMPARISON_LOG
        echo "======================================" >> $COMPARISON_LOG
        echo "" >> $COMPARISON_LOG
        return 1
    fi
    return 0
}

run_category() {
    local category_name="$1"
    local category_display_name="${TEST_KATEGORILERI[$category_name]}"
    local -n tests=$category_name

    echo -e "\n${CYAN}### $category_display_name (${#tests[@]} test) ###${RESET}\n"
    echo "=== Kategori: $category_display_name ===" >> $LOGFILE

    local passed=0
    local leaked=0
    local still_reachable=0
    local comparison_diffs=0

    for cmd in "${tests[@]}"; do
        ((TOPLAM_TEST++))
        printf "Test %-3d: %-50s ... " "$TOPLAM_TEST" "$cmd"
        echo "=== TEST $TOPLAM_TEST: $cmd ===" >> $LOGFILE

        # Test dosyaları oluştur
        create_test_files >/dev/null 2>&1

        # Memory leak test
        if [[ "$cmd" == *"<< EOF"* ]] || [[ "$cmd" == *"<< DELIMITER"* ]] || [[ "$cmd" == *"<< END"* ]] || [[ "$cmd" == *"<< MARKER"* ]]; then
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

        # Check memory leaks
        local result=""
        if grep -qE "definitely lost|indirectly lost|possibly lost" valgrind_tmp.log; then
            result="LEAK"
            grep -E "definitely lost|indirectly lost|possibly lost" valgrind_tmp.log >> $LOGFILE
            ((leaked++))
            ((TOPLAM_LEAK++))
        elif grep -q "still reachable" valgrind_tmp.log; then
            result="REACHABLE"
            grep -E "still reachable" valgrind_tmp.log >> $LOGFILE
            ((TOPLAM_BASARILI++))
            ((passed++))
            ((still_reachable++))
            ((TOPLAM_STILL_REACHABLE++))
        else
            result="OK"
            ((TOPLAM_BASARILI++))
            ((passed++))
        fi

        # Comparison test if enabled
        if $COMPARISON_MODE; then
            if compare_outputs "$cmd"; then
                echo "[$result] [MATCH]"
            else
                echo "[$result] [DIFF]"
                ((comparison_diffs++))
                ((TOPLAM_COMPARISON_DIFF++))
            fi
        else
            echo "[$result]"
        fi

        # Test dosyalarını temizle
        cleanup_test_files >/dev/null 2>&1
        echo "" >> $LOGFILE
    done

    echo -e "${CYAN}Kategori Sonucu:${RESET}"
    echo -e "  Clean: $((passed - still_reachable))"
    echo -e "  Still Reachable: $still_reachable"
    echo -e "  Memory Leaks: $leaked"
    if $COMPARISON_MODE; then
        echo -e "  Output Differences: $comparison_diffs"
    fi
    echo -e "  Toplam: ${#tests[@]} test"
}

# Usage function
show_usage() {
    echo -e "${BOLD}Enhanced Minishell Memory Leak & Comparison Tester${RESET}"
    echo -e "${CYAN}Usage:${RESET}"
    echo -e "  $0                   # Memory leak testing only"
    echo -e "  $0 --compare (-c)    # Memory leak testing + bash comparison"
    echo ""
    echo -e "${YELLOW}Features:${RESET}"
    echo -e "  • Comprehensive memory leak detection with Valgrind"
    echo -e "  • Output comparison with real bash (cleans prompts and ANSI codes)"
    echo -e "  • 42 School PDF requirement coverage"
    echo -e "  • Detailed logging and reporting"
    echo ""
}

# Help option
if [[ "$1" == "--help" || "$1" == "-h" ]]; then
    show_usage
    exit 0
fi

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

echo -e "${BLUE}=== Enhanced Minishell Memory Leak & Comparison Tester ===${RESET}"
echo -e "${MAGENTA}42 School proje gereksinimleri doğrultusunda kapsamlı test yapılıyor...${RESET}"

if $COMPARISON_MODE; then
    echo -e "${CYAN}Bash comparison mode enabled - prompts and ANSI codes will be cleaned${RESET}"
fi

echo ""

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
run_category SYNTAX_ERRORLAR
run_category HEREDOC_TESTLERI
run_category ENV_EXPAND
run_category HISTORY_COMMAND_TESTS
run_category MIXED_COMPLEX_TESTS
run_category STRESS_TESTS
run_category PDF_SPECIFIC_TESTS

# Son temizlik
cleanup_test_files
rm -f valgrind_tmp.log

echo -e "\n${BOLD}${BLUE}=== FINAL REPORT ===${RESET}"
echo -e "Clean Tests: $(($TOPLAM_BASARILI - $TOPLAM_STILL_REACHABLE))"
echo -e "Still Reachable: $TOPLAM_STILL_REACHABLE"
echo -e "Memory Leaks: $TOPLAM_LEAK"

if $COMPARISON_MODE; then
    echo -e "Output Differences: $TOPLAM_COMPARISON_DIFF"
fi

echo -e "Total Tests: $TOPLAM_TEST"
echo ""

if [ $TOPLAM_LEAK -eq 0 ]; then
    echo -e "${GREEN}Tebrikler! Hiç memory leak tespit edilmedi!${RESET}"
    if $COMPARISON_MODE && [ $TOPLAM_COMPARISON_DIFF -eq 0 ]; then
        echo -e "${GREEN}Perfect! Bash ile tam uyumluluk!${RESET}"
    elif $COMPARISON_MODE; then
        echo -e "${YELLOW}$TOPLAM_COMPARISON_DIFF farklı çıktı tespit edildi.${RESET}"
    fi
else
    echo -e "${RED}$TOPLAM_LEAK test'te memory leak tespit edildi.${RESET}"
fi

echo -e "\n${BOLD}Log Files:${RESET}"
echo -e "Memory leaks: ${YELLOW}$LOGFILE${RESET}"
if $COMPARISON_MODE; then
    echo -e "Bash comparison: ${YELLOW}$COMPARISON_LOG${RESET}"
fi

# Son durum kodu
if [ $TOPLAM_LEAK -eq 0 ]; then
    exit 0
else
    exit 1
fi
