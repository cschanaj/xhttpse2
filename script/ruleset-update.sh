#!/bin/bash

TEMP1=`mktemp`
TEMP2=`mktemp`

PATTERN1='^\s*- \([^\n]*\)$'
PATTERN2='^\s*<target\s\+host\s*=\s*"\([^"]*\)"\s*\/>\s*\?$'
PATTERN3='^[^<]*<ruleset\s\+name\s*=\s*"\([^"]*\)"[^>]*>\s*\?$'

# Extract subdomains from existing rulesets
sed -e "/$PATTERN1/!d" -e "s/$PATTERN1/\1/g" "$1" >> "$TEMP1"
sed -e "/$PATTERN2/!d" -e "s/$PATTERN2/\1/g" "$1" >> "$TEMP1"

NAME=`sed -e "/$PATTERN3/!d" -e "s/$PATTERN3/\1/g" "$1"`

# Backup existing file
mv -f "$1" "$1.bak"

# Rewrite ruleset completely
./bin/dsort "$TEMP1" | uniq > "$TEMP2"
./bin/xhttpse --num-threads 16 --name "$NAME" -o "$1" "$TEMP2"

# Remove temporary files
rm -rf "$TEMP1" "$TEMP2"
