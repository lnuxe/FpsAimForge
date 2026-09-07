#!/usr/bin/env python3
"""Verify every Tr() call in the source has a corresponding key in kChineseStrings."""
import re, os, sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
I18N = os.path.join(REPO, 'src/aim/i18n/i18n.cc')

def parse_table_keys(path):
    content = open(path).read()
    start = content.find('kChineseStrings{')
    end = content.find('};', start)
    body = content[start:end]
    keys = set()
    i = 0
    while True:
        b = body.find('{', i)
        if b == -1:
            break
        j = b + 1
        while j < len(body) and body[j] in ' \t\n\r':
            j += 1
        parts = []
        while j < len(body) and body[j] == '"':
            jj = j + 1
            buf = []
            while jj < len(body):
                c = body[jj]
                if c == '\\':
                    buf.append(body[jj:jj + 2]); jj += 2; continue
                if c == '"':
                    jj += 1; break
                buf.append(c); jj += 1
            parts.append(''.join(buf))
            j = jj
            while j < len(body) and body[j] in ' \t\n\r':
                j += 1
        if parts:
            keys.add(''.join(parts))
        i = b + 1
    return keys

def scan_calls(root):
    calls = set()
    for dirpath, _, files in os.walk(root):
        if 'i18n' in dirpath:
            continue
        for f in files:
            if not f.endswith(('.cc', '.h')):
                continue
            c = open(os.path.join(dirpath, f)).read()
            for m in re.finditer(r'Tr\(\s*((?:"(?:[^"\\]|\\.)*"\s*)+)\)', c):
                lits = re.findall(r'"((?:[^"\\]|\\.)*)"', m.group(1))
                calls.add(''.join(lits))
    return calls

table_keys = parse_table_keys(I18N)
calls = scan_calls(os.path.join(REPO, 'src/aim'))

# %s is a dynamic pass-through placeholder, not a translatable key.
missing = sorted(k for k in calls if k and k not in table_keys and k != '%s')
print('翻译表 key 数:', len(table_keys))
print('源码 Tr() 调用 key 数:', len(calls))
if missing:
    print('缺失翻译 key %d 个:' % len(missing))
    for k in missing:
        print('  ', repr(k[:90]))
    sys.exit(1)
print('✅ 所有 Tr() 调用都有对应翻译（完整）')