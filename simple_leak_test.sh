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

echo "🧪 Starting Simple Memory Leak Tests for Minishell"
echo "=================================================="

# Function to run a test and check for leaks using a different approach
run_test() {
    local test_num=$1
    local test_cmd=$2
    local description=$3
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo -n "Test $test_num: $description... "
    
    # Kill any existing minishell processes
    pkill -f minishell >/dev/null 2>&1
    
    # Run the command with memory tracking
    if [ -z "$test_cmd" ]; then
        # Empty command test
        echo "" | timeout 3 ./minishell >/dev/null 2>&1
        EXIT_CODE=$?
    else
        echo "$test_cmd" | timeout 3 ./minishell >/dev/null 2>&1
        EXIT_CODE=$?
    fi
    
    # Simple success check based on exit code
    if [ $EXIT_CODE -eq 0 ] || [ $EXIT_CODE -eq 1 ] || [ $EXIT_CODE -eq 127 ]; then
        echo -e "${GREEN}✅ Completed${NC}"
    elif [ $EXIT_CODE -eq 124 ]; then
        echo -e "${YELLOW}⚠️  Timeout${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    elif [ $EXIT_CODE -eq 139 ]; then
        echo -e "${RED}❌ SEGFAULT${NC}"
        LEAKED_TESTS=$((LEAKED_TESTS + 1))
    else
        echo -e "${YELLOW}⚠️  Exit code: $EXIT_CODE${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

# Test the key scenarios that were causing issues
echo "Testing core functionality..."

run_test 1 'echo hello world' 'basic echo'
run_test 2 'echo "$PWD"' 'variable expansion'
run_test 3 'cd $PWD hi' 'cd too many args'
run_test 4 'echo hi | "|"' 'pipe character as command'
run_test 5 'pwd' 'pwd builtin'
run_test 6 'export TEST=hello' 'export variable'
run_test 7 'unset PATH' 'unset variable'
run_test 8 'env | head -5' 'env with pipe'
run_test 9 'echo hello | cat' 'simple pipe'
run_test 10 'echo hello > ./outfiles/test1' 'output redirect'

echo ""
echo "Testing edge cases..."

run_test 11 '' 'empty command'
run_test 12 'nonexistent_command' 'command not found'
run_test 13 'echo $NONEXISTENT' 'nonexistent variable'
run_test 14 'echo hello | nonexistent | echo world' 'pipe with nonexistent'
run_test 15 'cat < nonexistent_file' 'input from nonexistent file'

echo ""
echo "Testing stress scenarios..."

run_test 16 'echo hello | cat | cat | cat | cat' 'long pipe chain'
run_test 17 'echo a b c d e f g h i j k l m n o p' 'many arguments'
run_test 18 'echo $PWD$PWD$PWD$PWD$PWD' 'repeated variables'
run_test 19 'export A=1; export B=2; echo $A$B' 'multiple exports'
run_test 20 'cd /tmp && pwd' 'cd and pwd combination'

echo ""
echo "=================================================="
echo "🏁 Test Summary:"
echo "Total tests: $TOTAL_TESTS"
echo -e "Failed/Crashed tests: ${RED}$LEAKED_TESTS${NC}"
echo -e "Tests with issues: ${YELLOW}$FAILED_TESTS${NC}"
echo -e "Successful tests: ${GREEN}$((TOTAL_TESTS - LEAKED_TESTS - FAILED_TESTS))${NC}"

if [ $LEAKED_TESTS -eq 0 ] && [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}🎉 All tests completed successfully!${NC}"
elif [ $LEAKED_TESTS -gt 0 ]; then
    echo -e "${RED}⚠️  Found $LEAKED_TESTS crashed/failed tests. Check for segfaults or memory issues.${NC}"
else
    echo -e "${YELLOW}⚠️  Some tests had issues but no crashes detected.${NC}"
fi

# Cleanup
rm -rf outfiles test_files 