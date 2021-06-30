# -----------------------------
# Options effecting formatting.
# -----------------------------
with section("format"):

  # How wide to allow formatted cmake files
  line_width = 120

  # How many spaces to tab for indent
  tab_size = 4

  # If true, separate flow control names from their parentheses with a space
  separate_ctrl_name_with_space = False

  # If true, separate function names from parentheses with a space
  separate_fn_name_with_space = False

  # If a statement is wrapped to more than one line, than dangle the closing
  # parenthesis on its own line.
  dangle_parens = True

  # What style line endings to use in the output.
  line_ending = 'unix'

  # Format command names consistently as 'lower' or 'upper' case
  command_case = 'lower'

  # Format keywords consistently as 'lower' or 'upper' case
  keyword_case = 'upper'

  # If true, the parsers may infer whether or not an argument list is sortable
  # (without annotation).
  autosort = True
