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


# Labels passed to Input* widgets via these setters/ctors must be wrapped in Tr()
# to reach the translation table. Detect English literals that exist in the table
# (i.e. they ARE translatable) but were passed WITHOUT Tr().
_LABEL_PATTERNS = [
    r'set_label\("((?:[^"\\]|\\.)*)"\)',
    r'set_id_and_label\("((?:[^"\\]|\\.)*)"\)',
    r'set_optional_secondary_label\("((?:[^"\\]|\\.)*)"\)',
    r'WithLabelAsId\("((?:[^"\\]|\\.)*)"\)',
    r'InputBool\("((?:[^"\\]|\\.)*)"',
    r'InputInt\("((?:[^"\\]|\\.)*)"',
    r'InputFloat\("((?:[^"\\]|\\.)*)"',
]


def scan_unwrapped_labels(root):
    """Return English label literals that are in the table but passed without Tr()."""
    bad = set()
    for dirpath, _, files in os.walk(root):
        if 'i18n' in dirpath:
            continue
        for f in files:
            if not f.endswith(('.cc', '.h')):
                continue
            c = open(os.path.join(dirpath, f)).read()
            for pat in _LABEL_PATTERNS:
                for m in re.finditer(pat, c):
                    label = m.group(1)
                    if not label:
                        continue
                    # Skip IDs (##-prefixed) and purely numeric/symbolic values.
                    if label.startswith('##') or label.startswith('%'):
                        continue
                    if label in table_keys:
                        bad.add(label)
    return bad


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

# Second check: English labels that are translatable but were not wrapped in Tr().
unwrapped = scan_unwrapped_labels(os.path.join(REPO, 'src/aim'))
if unwrapped:
    print('⚠️  以下英文标签在翻译表中但未用 Tr() 包裹（界面会显示英文）:')
    for k in sorted(unwrapped):
        print('  ', repr(k))
    sys.exit(1)
print('✅ 所有可翻译标签均已通过 Tr() 包裹')