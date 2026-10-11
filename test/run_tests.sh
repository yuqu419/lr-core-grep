#!/usr/bin/env bash
# 用法: LRG=./lrg ./run_tests.sh
# 每个用例带 3 秒超时，避免被测程序死循环时脚本挂住

set -u

LRG="${LRG:-./lrg}"
if [ ! -x "$LRG" ]; then
    echo "找不到可执行文件: $LRG" >&2
    exit 1
fi
LRG=$(cd "$(dirname "$LRG")" && pwd)/$(basename "$LRG")

TIMEOUT_BIN=$(command -v timeout || true)
if [ -z "$TIMEOUT_BIN" ]; then
    echo "警告: 找不到 timeout 命令，无法对用例做超时保护" >&2
fi
CASE_TIMEOUT="${CASE_TIMEOUT:-3}"

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
cd "$WORK" || exit 1

# 统一调用入口：带超时
run_lrg() {
    if [ -n "$TIMEOUT_BIN" ]; then
        "$TIMEOUT_BIN" "$CASE_TIMEOUT" "$LRG" "$@"
    else
        "$LRG" "$@"
    fi
}

# ==================== 测试数据 ====================
cat > sample.log <<'EOF'
boot ok
disk full at 03:00
ERROR: retry later
boot ok again
error: give up
EOF

cat > notes.txt <<'EOF'
error handling rules
all good
EOF

cat > literal.txt <<'EOF'
a.c
x*y
end
EOF

cat > dash.txt <<'EOF'
a-b
EOF

: > empty.txt
printf '\n\n' > empty_lines.txt
printf 'no newline at end' > nonl.txt
printf 'first\nsecond\nno newline at end' > mixed.txt
printf 'single line' > single.txt
mkdir -p a_directory

USAGE='usage: lrg [-nic] PATTERN [FILE...]'

# ==================== 测试框架 ====================
PASS=0
FAIL=0

# check NAME exp_rc exp_out exp_err [STDIN] -- lrg 参数...
# 说明: 期望的 exp_out / exp_err 不带尾部换行（与 $() 捕获口径一致）
check() {
    local name="$1" exp_rc="$2" exp_out="$3" exp_err="$4"
    shift 4
    local exp_in="__NONE__"
    if [ "${1:-}" != "--" ]; then
        exp_in="$1"; shift
    fi
    [ "${1:-}" = "--" ] && shift

    local act_out act_err act_rc
    if [ "$exp_in" = "__NONE__" ]; then
        act_out=$(run_lrg "$@" 2>err.tmp </dev/null)
    else
        act_out=$(printf '%s' "$exp_in" | run_lrg "$@" 2>err.tmp)
    fi
    act_rc=$?
    act_err=$(cat err.tmp 2>/dev/null)
    rm -f err.tmp

    local rc_note=""
    if [ "$act_rc" = "124" ] && [ -n "$TIMEOUT_BIN" ]; then
        rc_note=" (超时 ${CASE_TIMEOUT}s)"
    fi

    if [ "$act_rc" = "$exp_rc" ] && [ "$act_out" = "$exp_out" ] && [ "$act_err" = "$exp_err" ]; then
        echo "PASS: $name"; PASS=$((PASS+1))
    else
        echo "FAIL: $name"
        printf '  期望: rc=%s out=[%s] err=[%s]\n' "$exp_rc" "$exp_out" "$exp_err"
        printf '  实际: rc=%s%s out=[%s] err=[%s]\n' "$act_rc" "$rc_note" "$act_out" "$act_err"
        FAIL=$((FAIL+1))
    fi
}

n() { local name="$1" rc="$2" out="$3" err="$4"; shift 4; check "$name" "$rc" "$out" "$err" -- "$@"; }
s() { local name="$1" rc="$2" out="$3" err="$4" in="$5"; shift 5; check "$name" "$rc" "$out" "$err" "$in" -- "$@"; }

# 字节级比较（用于“末尾补 \n”这类必须看原始字节的测试）
check_bytes() {
    local name="$1" exp_rc="$2" exp_file="$3" exp_err="$4"
    shift 4
    local exp_in="__NONE__"
    if [ "${1:-}" != "--" ]; then exp_in="$1"; shift; fi
    [ "${1:-}" = "--" ] && shift

    local act_rc act_err
    if [ "$exp_in" = "__NONE__" ]; then
        run_lrg "$@" >actual.out 2>actual.err </dev/null
    else
        printf '%s' "$exp_in" | run_lrg "$@" >actual.out 2>actual.err
    fi
    act_rc=$?
    act_err=$(cat actual.err 2>/dev/null)

    local rc_note=""
    if [ "$act_rc" = "124" ] && [ -n "$TIMEOUT_BIN" ]; then
        rc_note=" (超时 ${CASE_TIMEOUT}s)"
    fi

    if [ "$act_rc" != "$exp_rc" ]; then
        echo "FAIL: $name (rc 期望 $exp_rc 实际 $act_rc$rc_note)"
        FAIL=$((FAIL+1)); return
    fi
    if ! cmp -s actual.out "$exp_file"; then
        echo "FAIL: $name (字节不一致: 期望 $(wc -c <"$exp_file") 字节, 实际 $(wc -c <actual.out) 字节)"
        FAIL=$((FAIL+1)); return
    fi
    if [ "$act_err" != "$exp_err" ]; then
        echo "FAIL: $name (stderr: 期望 [$exp_err] 实际 [$act_err])"
        FAIL=$((FAIL+1)); return
    fi
    echo "PASS: $name"; PASS=$((PASS+1))
}

# ==================== 1. 基本匹配 ====================
echo "===== 1. 基本匹配 ====="
n "基本匹配" 0 "boot ok
boot ok again" "" boot sample.log
n "无匹配退出码1" 1 "" "" zzz sample.log
n "无匹配 -c 输出0且退出1" 1 "0" "" -c zzz sample.log
n "模式是字面量(a.c 不解释为正则)" 0 "a.c" "" a.c literal.txt
n "模式 * 也是字面量" 0 "x*y" "" 'x*y' literal.txt

# ==================== 2. -i ====================
echo "===== 2. -i 忽略大小写 ====="
n "-i 匹配" 0 "ERROR: retry later
error: give up" "" -i error sample.log
n "-i 模式大写匹配小写" 0 "ERROR: retry later
error: give up" "" -i ERROR sample.log
n "-i 匹配 aC" 0 "ERROR: retry later
error: give up" "" -i eRRor sample.log
n "不加 -i 大小写敏感" 0 "error: give up" "" error sample.log

# ==================== 3. -n ====================
echo "===== 3. -n 行号 ====="
n "-n 行号" 0 "5:error: give up" "" -n error sample.log
n "-n 单文件无文件名前缀" 0 "1:boot ok
4:boot ok again" "" -n boot sample.log
s "-n stdin 末行无换行也计数" 0 "2:y" "" "x
y" -n y

# ==================== 4. -c ====================
echo "===== 4. -c 计数 ====="
n "-c 单文件" 0 "1" "" -c error sample.log
n "-c 忽略 -n" 0 "1" "" -n -c error sample.log
n "-c 多文件带前缀" 0 "sample.log:2
notes.txt:1" "" -ci error sample.log notes.txt
s "-c stdin" 0 "2" "" "a
b
a" -c a
n "-c 多文件中0匹配的文件也要输出 file:0, 退出1" 1 "sample.log:0
notes.txt:0" "" -c zzz sample.log notes.txt
n "-c 坏文件退出2且无计数输出" 2 "0" \
  "lrg: no-such-file: No such file or directory" -c error no-such-file

# ==================== 5. 选项解析 ====================
echo "===== 5. 选项解析 ====="
# 修正后
n "-ni 合并" 0 "3:ERROR: retry later
5:error: give up" "" -ni error sample.log
n "-in 顺序无关" 0 "3:ERROR: retry later
5:error: give up" "" -in error sample.log
n "-nic 三选项合并(-c 忽略 -n)" 0 "2" "" -nic error sample.log
n "非法选项 -x"  2 "" "$USAGE" -x pattern file
n "非法选项 -nix" 2 "" "$USAGE" -nix pattern file
n "非法选项 -xn"  2 "" "$USAGE" -xn pattern file
n "只有选项无 PATTERN 用法错误" 2 "" "$USAGE" -n
n "缺 PATTERN 用法错误"       2 "" "$USAGE"
n "只有 -i 无 PATTERN"        2 "" "$USAGE" -i

# ==================== 6. 多文件前缀 ====================
echo "===== 6. 多文件前缀 ====="
n "多文件带文件名" 0 "sample.log:error: give up
notes.txt:error handling rules" "" error sample.log notes.txt
n "多文件+n" 0 "sample.log:5:error: give up
notes.txt:1:error handling rules" "" -n error sample.log notes.txt
n "文件名按命令行原样输出" 0 "./sample.log:error: give up
sample.log:error: give up" "" error ./sample.log sample.log
n "多文件含空文件(空文件0匹配)" 0 "sample.log:boot ok
sample.log:boot ok again" "" boot empty.txt sample.log

# ==================== 7. stdin 与 - ====================
echo "===== 7. stdin 与 - 的处理 ====="
s "stdin 无前缀" 0 "1:boot ok
4:boot ok again" "" "boot ok
disk full at 03:00
ERROR: retry later
boot ok again
error: give up" -n boot
s "空 PATTERN 匹配所有行(stdin)" 0 "hello
world" "" "hello
world" ""
n "空 PATTERN 匹配所有行(文件,含无换行尾行)" 0 "first
second
no newline at end" "" "" mixed.txt
s "空 PATTERN 匹配空行" 0 "" "" $'\n\n' ""
n "单独的 - 作 PATTERN(字面量匹配)" 0 "a-b" "" - dash.txt

# ==================== 8. 错误处理与退出码 ====================
echo "===== 8. 错误处理与退出码 ====="
n "打开失败继续处理, 退出2" 2 "sample.log:error: give up" \
  "lrg: no-such-file: No such file or directory" error no-such-file sample.log
n "只有坏文件 退出2" 2 "" \
  "lrg: no-such-file: No such file or directory" pattern no-such-file
n "多个坏文件逐个报错, 退出2" 2 "" "lrg: no-such-a: No such file or directory
lrg: no-such-b: No such file or directory" pattern no-such-a no-such-b
n "PATTERN 之后的 -n 按文件名处理(打开失败)" 2 "" \
  "lrg: -n: No such file or directory" pattern -n
n "目录作为 FILE: 读取失败报错, 退出2" 2 "" \
  "lrg: a_directory: Is a directory" pattern a_directory

# ==================== 9. 边界：行尾无换行 ====================
echo "===== 9. 边界：行尾无换行 ====="
printf 'no newline at end\n' > expected_nonl.out
check_bytes "最后一行无换行补\\n" 0 expected_nonl.out "" -- 'no newline' nonl.txt

printf 'single line\n' > expected_single.out
check_bytes "整个文件单行无换行" 0 expected_single.out "" -- 'single' single.txt

# ==================== 10. 边界：空文件 ====================
echo "===== 10. 边界：空文件 ====="
n "空文件无匹配" 1 "" "" pattern empty.txt
n "空文件 -c 输出0退出1" 1 "0" "" -c pattern empty.txt

# ==================== 11. 1MiB 超长行 ====================
echo "===== 11. 1MiB 超长行（字节比对） ====="
head -c 1048576 /dev/zero | tr '\0' 'a' > bigA.txt
{ head -c 1048576 /dev/zero | tr '\0' 'a'; printf '\n'; } > bigA.expected
check_bytes "1MiB 超长行" 0 bigA.expected "" -- 'a' bigA.txt

printf 'a\n' > b.txt
{ printf 'bigA.txt:'; head -c 1048576 /dev/zero | tr '\0' 'a'; printf '\n'; printf 'b.txt:a\n'; } > big_multi.expected
check_bytes "1MiB 超长行+文件名前缀" 0 big_multi.expected "" -- 'a' bigA.txt b.txt
echo
echo "结果: PASS=$PASS FAIL=$FAIL"
[ "$FAIL" -eq 0 ]