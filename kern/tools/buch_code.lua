traverse = "topdown"
-- Pandoc filter for the printed book (kern/tools/buch_pdf.py): inline code may break across lines.
-- Identifiers, paths and URLs in `code` are often longer than the space left in a line or a table
-- cell; without break points they run into the margin. After punctuation (/ . _ : , ( ) = - < > [ ])
-- and inside long letter runs (every 8 characters) a line break is allowed, without a hyphen.

local special = {
  ["\\"] = "\\textbackslash{}", ["{"] = "\\{", ["}"] = "\\}", ["#"] = "\\#", ["$"] = "\\$",
  ["%"] = "\\%", ["&"] = "\\&", ["_"] = "\\_", ["^"] = "\\textasciicircum{}", ["~"] = "\\textasciitilde{}",
}
local breakafter = "[/%._:,%(%)=%-<>%[%];|+*]"

local function breakable(s)
  local out, run = {}, 0
  for _, cp in utf8.codes(s) do
    local ch = utf8.char(cp)
    out[#out + 1] = special[ch] or ch
    if ch:match(breakafter) then
      out[#out + 1] = "\\allowbreak{}"
      run = 0
    elseif ch:match("[%w]") then
      run = run + 1
      if run >= 8 then out[#out + 1] = "\\allowbreak{}"; run = 0 end
    else
      run = 0
    end
  end
  return table.concat(out)
end

function Code(el)
  return pandoc.RawInline("latex", "\\texttt{" .. breakable(el.text) .. "}")
end

-- Formulas wider than the line are scaled down to fit (\kernfit, defined in buch_pdf.py), instead
-- of reaching into the margin; short ones stay as they are. Not adjustbox: it takes pandoc's
-- \setkeys{Gin}{width=\maxwidth,...} as defaults and shrank every formula to half a point.
function Math(el)
  if el.mathtype == "DisplayMath" then
    return pandoc.RawInline("latex", "\\[\\kernfit{$\\displaystyle " .. el.text .. "$}\\]")
  elseif #el.text > 30 then
    -- \hfil\penalty50\hfilneg: the line may end before the formula, ragged, instead of the formula
    -- sticking out of the margin; when no break is taken the two glues cancel.
    return pandoc.RawInline("latex", "\\nobreak\\hfil\\penalty50\\hfilneg\\kernfit{$" .. el.text .. "$}")
  end
end

-- Tables with many columns are set smaller, with less space between columns, and may break after
-- a slash ("Magazin/Reserve"), so that they fit the type area.
function Table(tbl)
  tbl = tbl:walk({
    Str = function(s)
      if s.text:find("/") then
        local parts = {}
        for piece in (s.text .. "/"):gmatch("(.-)/") do parts[#parts + 1] = piece end
        local out = {}
        for i, piece in ipairs(parts) do
          if i > 1 then out[#out + 1] = pandoc.RawInline("latex", "/\\allowbreak{}") end
          if piece ~= "" then out[#out + 1] = pandoc.Str(piece) end
        end
        return out
      end
    end,
  })
  if #tbl.colspecs < 6 then return tbl end
  return {
    pandoc.RawBlock("latex", "\\begingroup\\scriptsize\\setlength{\\tabcolsep}{2pt}"),
    tbl,
    pandoc.RawBlock("latex", "\\endgroup"),
  }
end

-- Code in a heading also appears in the PDF bookmarks: pandoc writes it there as plain text only
-- if it is still Code, so headings are left alone (they are short enough not to need breaks).
function Header(h)
  return h, false
end
