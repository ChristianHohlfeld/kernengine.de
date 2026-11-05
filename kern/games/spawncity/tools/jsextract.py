"""Finds top-level declarations (function NAME / const|let NAME=) in minified-ish JS by bracket matching."""
import re

def _skip_string(src, i):
    q = src[i]; i += 1
    while i < len(src):
        c = src[i]
        if c == "\\": i += 2; continue
        if q == "`" and c == "$" and src[i+1:i+2] == "{":
            i = _match(src, i + 1) + 1; continue
        if c == q: return i + 1
        i += 1
    raise ValueError("unterminated string")

def _match(src, i):
    """i at an opening bracket; returns the index of its closing bracket."""
    open_, close = src[i], {"{": "}", "(": ")", "[": "]"}[src[i]]
    depth = 0
    while i < len(src):
        c = src[i]
        if c in "\"'`": i = _skip_string(src, i); continue
        if c == "/" and src[i+1:i+2] == "/": i = src.index("\n", i); continue
        if c == "/" and src[i+1:i+2] == "*": i = src.index("*/", i) + 2; continue
        if c == "/" and _regex_ok(src, i):
            j = i + 1; cls = False
            while True:
                d = src[j]
                if d == "\\": j += 2; continue
                if d == "[": cls = True
                elif d == "]": cls = False
                elif d == "/" and not cls: break
                j += 1
            i = j + 1; continue
        if c in "{([": depth += 1
        elif c in "})]":
            depth -= 1
            if depth == 0: return i
        i += 1
    raise ValueError("unbalanced")

def _regex_ok(src, i):
    k = i - 1
    while k >= 0 and src[k] in " \t": k -= 1
    return k < 0 or src[k] in "(,=:[!&|?{};+-*%<>~^\n" or src[k-5:k+1] == "return"

def function(src, name):
    m = re.search(r"(?:^|(?<=[;}\n]))[ \t]*((?:export )?(?:async )?function %s\()" % re.escape(name), src, re.M)
    if not m: raise KeyError(name)
    start = m.start(1)
    body = src.index("{", _match(src, m.end() - 1))
    end = _match(src, body)
    return src[start:end + 1].replace("export function", "function", 1)

def statement(src, prefix):
    """The statement that starts with `prefix` (an if / const / call), up to its end at depth 0."""
    start = src.index(prefix)
    if src.count(prefix) != 1: raise ValueError("ambiguous: " + prefix[:40])
    i = start
    while True:
        c = src[i]
        if c in "\"'`": i = _skip_string(src, i); continue
        if c in "{([":
            j = _match(src, i) + 1
            if c == "{" and src[start:start + 3] == "if(" and src[j:j + 4] != "else":
                return src[start:j]
            i = j; continue
        if c == ";": return src[start:i + 1]
        i += 1
