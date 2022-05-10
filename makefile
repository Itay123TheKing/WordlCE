# ----------------------------
# Makefile Options
# ----------------------------

NAME = WORDLE
ICON = icon.png
DESCRIPTION = "Wordle, in the calculator"
COMPRESSED = NO
ARCHIVED = YES

CFLAGS = -Wall -Wextra -O2
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)
